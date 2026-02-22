/**
 * fm_ymfm_test.cpp
 * Milestone 1-2: ymfm ライブラリを使った FM 音源エミュレーション検証
 *
 * 対象チップ: YM2203 (OPN) — PC-88 で使われた FM 3ch + PSG 3ch
 * 出力: output_ymfm_opn.wav
 * 内容: FM ch0 で C メジャースケール（アルゴリズム 4: 2オペ FM）
 * 計測: サンプル生成に要した CPU 時間
 *
 * 【修正内容】
 * 旧バージョンは内部サンプルレートを OPN_CLOCK/144 と誤算していた。
 * 正しくは chip.sample_rate(clock) を呼ぶこと。
 * YM2203 は OPN_FIDELITY_MIN 設定で clock/24 ≒ 166400 Hz の内部レートを持つ。
 * ここでは内部レートで WAV を直接書き出す（レート変換なし、最も正確）。
 */

#include <cstdio>
#include <cmath>
#include <chrono>
#include <cstring>
#include "wav_writer.h"

// ymfm インターフェース
#include "ymfm_opn.h"

static constexpr uint32_t OPN_CLOCK    = 3993600; // PC-88: 3.9936 MHz

// ymfm が要求する「外部インターフェース」の最小実装
class minimal_chip_interface : public ymfm::ymfm_interface {
public:
    void ymfm_sync_mode_write(uint8_t) override {}
    void ymfm_sync_check_interrupts() override {}
    void ymfm_set_timer(uint32_t, int32_t) override {}
    void ymfm_set_busy_end(uint32_t) override {}
    uint8_t ymfm_external_read(ymfm::access_class, uint32_t) override { return 0; }
    void ymfm_external_write(ymfm::access_class, uint32_t, uint8_t) override {}
};

// YM2203 ラッパー
struct OPN {
    minimal_chip_interface iface;
    ymfm::ym2203 chip;

    OPN() : chip(iface) {}

    void write(uint8_t reg, uint8_t val) {
        chip.write_address(reg);
        chip.write_data(val);
    }

    void generate(ymfm::ym2203::output_data* out) {
        chip.generate(out);
    }

    void reset() { chip.reset(); }

    // 正しいサンプルレートを取得（ymfm API 使用）
    uint32_t sample_rate() const {
        return chip.sample_rate(OPN_CLOCK);
    }
};

// 周波数 → OPN の F-Number / ブロック計算
static void freq_to_fnum_block(double freq, uint32_t clock,
                               uint16_t& fnum, uint8_t& block) {
    block = 0;
    for (int b = 7; b >= 0; b--) {
        double fn = freq * (1 << (20 - b)) / (double)clock * 144.0;
        if (fn >= 512 && fn < 1024) {
            block = b;
            fnum  = static_cast<uint16_t>(fn + 0.5);
            return;
        }
    }
    // フォールバック
    block = 4;
    fnum  = static_cast<uint16_t>(freq * (1 << (20 - 4)) / (double)clock * 144.0 + 0.5);
}

static double midi_to_freq(int midi_note) {
    return 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
}

// FM の 1 オペレータを設定するヘルパー（ch=0, slot=0〜3）
static void set_op(OPN& opn, int ch, int slot,
                   int dt, int mul, int tl, int ks, int ar,
                   int dr, int sr, int sl, int rr) {
    int base = 0x30 + slot * 4 + ch;
    opn.write(base,        (dt << 4) | mul);
    opn.write(base + 0x10, tl & 0x7F);
    opn.write(base + 0x20, (ks << 6) | ar);
    opn.write(base + 0x30, dr & 0x1F);
    opn.write(base + 0x40, sr & 0x1F);
    opn.write(base + 0x50, (sl << 4) | rr);
}

// ピアノ風 2オペ音色（ch=0、アルゴリズム 4）
static void setup_fm_voice(OPN& opn) {
    opn.write(0xB0, (4 << 3) | 4); // FB=4, ALG=4
    opn.write(0xB4, 0xC0);          // L/R 出力 ON

    // OP1 (モジュレータ): 短めのエンベロープ
    set_op(opn, 0, 0, 0, 1, 22, 0, 31, 10, 5, 5, 8);
    // OP3 (キャリア): 緩やかなリリース
    set_op(opn, 0, 2, 0, 1,  0, 0, 31,  8, 3, 3, 6);
    // OP2, OP4 は未使用（TL=127 で無音化）
    set_op(opn, 0, 1, 0, 1, 127, 0, 0, 0, 0, 0, 0);
    set_op(opn, 0, 3, 0, 1, 127, 0, 0, 0, 0, 0, 0);
}

static void key_on(OPN& opn, int ch, uint16_t fnum, uint8_t block) {
    opn.write(0xA4 + ch, ((block << 3) | (fnum >> 8)) & 0x3F);
    opn.write(0xA0 + ch, fnum & 0xFF);
    opn.write(0x28, 0xF0 | ch);  // KEY ON: 全オペレータ
}

static void key_off(OPN& opn, int ch) {
    opn.write(0x28, 0x00 | ch);
}

int main() {
    printf("=== ymfm OPN (YM2203) FM emulation test ===\n");
    printf("Clock: %u Hz\n", OPN_CLOCK);

    OPN opn;
    opn.reset();

    // FIDELITY_MIN: clock/24 ≒ 166400 Hz の内部レート（処理負荷と精度のバランス）
    opn.chip.set_fidelity(ymfm::OPN_FIDELITY_MIN);

    uint32_t SAMPLE_RATE = opn.sample_rate();
    printf("Internal sample rate: %u Hz\n", SAMPLE_RATE);
    // ↑ ymfm の chip.sample_rate() を使って正しいレートを取得する

    setup_fm_voice(opn);

    WavWriter wav;
    if (!wav.open("output_ymfm_opn.wav", SAMPLE_RATE, 2)) {
        fprintf(stderr, "Cannot open output_ymfm_opn.wav\n");
        return 1;
    }

    const int scale[] = {60, 62, 64, 65, 67, 69, 71, 72};
    const int note_samples     = static_cast<int>(SAMPLE_RATE * 0.4);
    const int release_samples  = static_cast<int>(SAMPLE_RATE * 0.15);

    // OPN output_data: OUTPUTS=2 (FM ch, SSG ch)
    // out.data[0] = FM 出力, out.data[1] = SSG 出力
    // OPN FM は内部的にモノラル → L/R 同値でステレオ化
    static constexpr double OPN_NORM = 1.0 / 32768.0;

    auto t_start = std::chrono::high_resolution_clock::now();
    long long total_samples = 0;

    for (int note : scale) {
        double freq = midi_to_freq(note);
        uint16_t fnum; uint8_t block;
        freq_to_fnum_block(freq, OPN_CLOCK, fnum, block);

        printf("  note=%d freq=%.1f Hz fnum=%u block=%u\n",
               note, freq, fnum, block);

        key_on(opn, 0, fnum, block);

        for (int s = 0; s < note_samples + release_samples; s++) {
            if (s == note_samples) key_off(opn, 0);

            ymfm::ym2203::output_data out;
            opn.generate(&out);

            // FM 出力を正規化してステレオ WAV に書く
            double v = out.data[0] * OPN_NORM;
            wav.write_stereo(v, v);
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
    printf("  出力ファイル   : output_ymfm_opn.wav\n");

    if (cpu_ratio < 0.1)
        printf("  判定: ✅ 余裕あり（リアルタイム処理に問題なし）\n");
    else if (cpu_ratio < 0.5)
        printf("  判定: ✅ 許容範囲内\n");
    else
        printf("  判定: ⚠️  要最適化\n");

    return 0;
}
