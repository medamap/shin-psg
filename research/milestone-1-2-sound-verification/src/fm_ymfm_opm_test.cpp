/**
 * fm_ymfm_opm_test.cpp
 * Milestone 1-2: ymfm ライブラリを使った OPM 音源エミュレーション検証
 *
 * 対象チップ: YM2151 (OPM)
 * 搭載機:
 *   - Sharp X1 系 + FM 音源ボード (CZ-8BS1 等)  ← 拡張ボードで OPM 追加
 *   - Sharp X1turboZ                             ← 標準搭載
 *   - Sharp X68000 系                            ← 標準搭載
 * OPM クロック: 4.0 MHz (X1系/X68000 共通)
 *
 * 出力: output_ymfm_opm.wav
 * 内容: FM ch0 で C メジャースケール（4オペ FM、アルゴリズム 5）
 * 計測: サンプル生成に要した CPU 時間
 *
 * 【OPM サンプルレート】
 *   ymfm の sample_rate() = clock / (prescale * OPERATORS)
 *   YM2151: prescale=2, OPERATORS=32 → 4000000/64 = 62500 Hz
 *   WAV はこの内部レートで直接書き出す（レート変換なし）。
 *
 * 【KC レジスタのノートエンコーディング】
 *   YM2151 の KC レジスタ [7:4]=オクターブ, [3:0]=ノート
 *   ノート値（半音）→ KC[3:0] の対応:
 *     C#=0, D=1, D#=2, (3=unused), E=4, F=5, F#=6,
 *     (7=unused), G=8, G#=9, A=10, (11=unused), A#=12, B=13, C=14, (15=unused)
 *   C は各オクターブの末尾（14）に位置することに注意。
 */

#include <cstdio>
#include <cmath>
#include <chrono>
#include <cstring>
#include "wav_writer.h"

#include "ymfm_opm.h"

static constexpr uint32_t OPM_CLOCK = 4000000; // X1/X1turboZ/X68000: 4.0 MHz

// ymfm 最小インターフェース
class minimal_chip_interface : public ymfm::ymfm_interface {
public:
    void ymfm_sync_mode_write(uint8_t) override {}
    void ymfm_sync_check_interrupts() override {}
    void ymfm_set_timer(uint32_t, int32_t) override {}
    void ymfm_set_busy_end(uint32_t) override {}
    uint8_t ymfm_external_read(ymfm::access_class, uint32_t) override { return 0; }
    void ymfm_external_write(ymfm::access_class, uint32_t, uint8_t) override {}
};

// YM2151 ラッパー
struct OPM {
    minimal_chip_interface iface;
    ymfm::ym2151 chip;

    OPM() : chip(iface) {}

    void write(uint8_t reg, uint8_t val) {
        chip.write_address(reg);
        chip.write_data(val);
    }

    void generate(ymfm::ym2151::output_data* out) {
        chip.generate(out);
    }

    void reset() { chip.reset(); }

    uint32_t sample_rate() const {
        return chip.sample_rate(OPM_CLOCK);
    }
};

// -----------------------------------------------------------------------
// OPM KC（Key Code）エンコーディング
// MIDI ノート → KC レジスタ値（[7:4]=オクターブ, [3:0]=ノートコード）
// -----------------------------------------------------------------------
// MIDI ノートの音名（%12）→ OPM ノートコード
// OPM は C を 14 に置き、C# から次オクターブ先頭（0）が始まる
static const uint8_t midi_note_to_kc_note[12] = {
    14, // C  → KC=14（オクターブ末尾）
     0, // C# → KC=0（次オクターブ先頭として扱うので +1 オクターブ）
     1, // D
     2, // D#
     4, // E  （3 はスキップ）
     5, // F
     6, // F#
     8, // G  （7 はスキップ）
     9, // G#
    10, // A
    12, // A# （11 はスキップ）
    13, // B
};

// C# 以上のノートはオクターブをずらす必要があるかのフラグ
static const bool midi_note_needs_octave_shift[12] = {
    false, // C
    true,  // C# (0 は次オクターブの先頭なので octave を +1 しない）
    false, // D
    false, // D#
    false, // E
    false, // F
    false, // F#
    false, // G
    false, // G#
    false, // A
    false, // A#
    false, // B
};

// MIDI ノート番号 → KC バイト
// MIDI 0=C-1, 12=C0, 24=C1, ..., 60=C4, 72=C5
static uint8_t midi_to_kc(int midi_note) {
    // MIDI オクターブ: midi_note / 12 - 1 (0=C-1, ..., 5=C4)
    int semitone = midi_note % 12;
    int octave   = midi_note / 12 - 1; // MIDI の C4 = octave 4 に合わせる
    // C# 以上のとき OPM KC でオクターブが 1 増える（C から始まるため）
    // 例: C# (midi %12=1) → KC note=0 は「次のオクターブの C#」
    // → octave はそのまま（KC では C# が 0 で同オクターブ扱い）
    // ただし C (semitone=0, kc_note=14) の場合はオクターブそのまま
    // OPM オクターブ範囲: 0〜7 (clamp)
    int kc_octave = octave;
    if (kc_octave < 0) kc_octave = 0;
    if (kc_octave > 7) kc_octave = 7;
    uint8_t kc_note = midi_note_to_kc_note[semitone];
    return static_cast<uint8_t>((kc_octave << 4) | kc_note);
}

// -----------------------------------------------------------------------
// OPM レジスタヘルパー
// -----------------------------------------------------------------------
// opoffs = op * 8 + ch (op=0..3, ch=0..7)
static void set_op(OPM& opm, int ch, int op,
                   int dt1, int mul, int tl, int ks, int ar,
                   int d1r, int d2r, int d1l, int rr) {
    int base = op * 8 + ch;
    opm.write(0x40 + base, ((dt1 & 7) << 4) | (mul & 0xF)); // DT1/MUL
    opm.write(0x60 + base, tl & 0x7F);                       // TL
    opm.write(0x80 + base, ((ks & 3) << 6) | (ar & 0x1F));  // KS/AR
    opm.write(0xA0 + base, d1r & 0x1F);                      // D1R (DR)
    opm.write(0xC0 + base, d2r & 0x1F);                      // DT2/D2R (SR) ※DT2=0
    opm.write(0xE0 + base, ((d1l & 0xF) << 4) | (rr & 0xF));// D1L/RR
}

// ブラス風音色（ch=0, ALG=5: op0→op1, op2→op3, op1+op3→out）
// 4オペ FM の定番的な設定
static void setup_opm_voice(OPM& opm, int ch) {
    // 0x20+ch: RL=11b (両出力), FL=5 (フィードバック), CON=5 (アルゴリズム5)
    opm.write(0x20 + ch, 0b11000000 | (5 << 3) | 5); // RL=11, FB=5, ALG=5

    // ALG 5 のオペレータ役割:
    //   op0 (M1): OP2 を変調するモジュレータ
    //   op1 (C1): キャリア（出力）← op0 に変調される
    //   op2 (M2): OP4 を変調するモジュレータ
    //   op3 (C2): キャリア（出力）← op2 に変調される

    // op0: M1 モジュレータ (DT1=3, MUL=1, TL=50, AR=31, D1R=10, D2R=4, D1L=3, RR=8)
    set_op(opm, ch, 0,  3, 1, 50, 0, 31, 10,  4, 3, 8);
    // op1: C1 キャリア  (DT1=0, MUL=1, TL=0,  AR=31, D1R=5,  D2R=2, D1L=4, RR=6)
    set_op(opm, ch, 1,  0, 1,  0, 0, 31,  5,  2, 4, 6);
    // op2: M2 モジュレータ (DT1=7, MUL=2, TL=45, AR=25, D1R=15, D2R=8, D1L=5, RR=8)
    set_op(opm, ch, 2,  7, 2, 45, 0, 25, 15,  8, 5, 8);
    // op3: C2 キャリア  (DT1=0, MUL=2, TL=5,  AR=31, D1R=6,  D2R=3, D1L=4, RR=6)
    set_op(opm, ch, 3,  0, 2,  5, 0, 31,  6,  3, 4, 6);
}

// KC 書き込み（KF=0）
static void opm_set_note(OPM& opm, int ch, uint8_t kc) {
    opm.write(0x28 + ch, kc); // KC
    opm.write(0x30 + ch, 0);  // KF = 0
}

// Key ON: 全オペレータ（0x78 = bits[6:3] = 1111）
static void opm_key_on(OPM& opm, int ch) {
    opm.write(0x08, 0x78 | (ch & 7));
}

// Key OFF
static void opm_key_off(OPM& opm, int ch) {
    opm.write(0x08, 0x00 | (ch & 7));
}

int main() {
    printf("=== ymfm OPM (YM2151) FM emulation test ===\n");
    printf("Target hardware:\n");
    printf("  - Sharp X1 + FM sound board (CZ-8BS1 etc.)\n");
    printf("  - Sharp X1turboZ (built-in OPM)\n");
    printf("  - Sharp X68000 series (built-in OPM)\n");
    printf("Clock: %u Hz\n", OPM_CLOCK);

    OPM opm;
    opm.reset();

    // LFO リセット解除
    opm.write(0x01, 0x02);  // LFO reset = 0

    uint32_t SAMPLE_RATE = opm.sample_rate();
    printf("Internal sample rate: %u Hz\n", SAMPLE_RATE);

    setup_opm_voice(opm, 0);

    WavWriter wav;
    if (!wav.open("output_ymfm_opm.wav", SAMPLE_RATE, 2)) {
        fprintf(stderr, "Cannot open output_ymfm_opm.wav\n");
        return 1;
    }

    // C メジャースケール: C4(60)〜C5(72) 各 0.4 秒
    const int scale[] = {60, 62, 64, 65, 67, 69, 71, 72};
    const int note_samples    = static_cast<int>(SAMPLE_RATE * 0.4);
    const int release_samples = static_cast<int>(SAMPLE_RATE * 0.15);

    // OPM 出力正規化係数（ymfm の OPM は ±32768 スケール）
    static constexpr double OPM_NORM = 1.0 / 32768.0;

    auto t_start = std::chrono::high_resolution_clock::now();
    long long total_samples = 0;

    for (int note : scale) {
        uint8_t kc = midi_to_kc(note);
        printf("  note=%d KC=0x%02X (oct=%d note=%d)\n",
               note, kc, (kc >> 4), (kc & 0xF));

        opm_set_note(opm, 0, kc);
        opm_key_on(opm, 0);

        for (int s = 0; s < note_samples + release_samples; s++) {
            if (s == note_samples) opm_key_off(opm, 0);

            ymfm::ym2151::output_data out;
            opm.generate(&out);

            // OPM OUTPUTS=2 (L, R ステレオ)
            double l = out.data[0] * OPM_NORM;
            double r = out.data[1] * OPM_NORM;
            wav.write_stereo(l, r);
            total_samples++;
        }
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    wav.close();

    double elapsed = std::chrono::duration<double>(t_end - t_start).count();
    double audio_duration = static_cast<double>(total_samples) / SAMPLE_RATE;
    double cpu_ratio = elapsed / audio_duration;

    printf("\n--- 計測結果 ---\n");
    printf("  生成サンプル数 : %lld\n", total_samples);
    printf("  音声長さ       : %.2f 秒\n", audio_duration);
    printf("  CPU 時間       : %.4f 秒\n", elapsed);
    printf("  CPU 負荷比     : %.4f (1.0 = リアルタイム上限)\n", cpu_ratio);
    printf("  WAV サンプルレート: %u Hz\n", SAMPLE_RATE);
    printf("  出力ファイル   : output_ymfm_opm.wav\n");

    if (cpu_ratio < 0.1)
        printf("  判定: ✅ 余裕あり（リアルタイム処理に問題なし）\n");
    else if (cpu_ratio < 0.5)
        printf("  判定: ✅ 許容範囲内\n");
    else
        printf("  判定: ⚠️  要最適化\n");

    return 0;
}
