/**
 * fm_ymfm_test.cpp
 * Milestone 1-2: ymfm ライブラリを使った FM 音源エミュレーション検証
 *
 * 対象チップ: YM2203 (OPN) — PC-88 で使われた FM 3ch + PSG 3ch
 * 出力: output_ymfm_opn.wav
 * 内容: FM ch0 で C メジャースケール（アルゴリズム 4: FB+OP1→OP2）
 * 計測: サンプル生成に要した CPU 時間
 */

#include <cstdio>
#include <cmath>
#include <chrono>
#include <cstring>
#include "wav_writer.h"

// ymfm インターフェース
#include "ymfm_opn.h"

static constexpr uint32_t OPN_CLOCK    = 3993600; // PC-88: 3.9936 MHz
static constexpr uint32_t SAMPLE_RATE  = 44100;

// ymfm が要求する「外部インターフェース」の最小実装
class minimal_chip_interface : public ymfm::ymfm_interface {
public:
    // ymfm がサンプル出力タイミングで呼ぶコールバック（今回は何もしない）
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
    uint32_t sample_rate;
    uint32_t clock;

    OPN(uint32_t clk, uint32_t sr)
        : chip(iface), sample_rate(sr), clock(clk) {}

    void write(uint8_t reg, uint8_t val) {
        chip.write_address(reg);
        chip.write_data(val);
    }

    // generate_samples: 指定数のサンプルを output_buf へ書く
    // output_buf: stereo interleaved int32_t (ymfm の生出力)
    void generate(ymfm::ym2203::output_data* out, uint32_t count = 1) {
        chip.generate(out, count);
    }

    void reset() { chip.reset(); }
};

// 周波数 → OPN の F-Number / ブロック計算
// F-Number = f * 2^(20-block) / clock * 144
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
// slot: 0=OP1, 1=OP3, 2=OP2, 3=OP4 (OPN の並び)
static void set_op(OPN& opn, int ch, int slot,
                   int dt, int mul, int tl, int ks, int ar,
                   int dr, int sr, int sl, int rr) {
    int base = 0x30 + slot * 4 + ch;
    opn.write(base,        (dt << 4) | mul);         // DT/MUL
    opn.write(base + 0x10, tl & 0x7F);               // TL
    opn.write(base + 0x20, (ks << 6) | ar);          // KS/AR
    opn.write(base + 0x30, dr & 0x1F);               // DR
    opn.write(base + 0x40, sr & 0x1F);               // SR
    opn.write(base + 0x50, (sl << 4) | rr);          // SL/RR
}

// 音の設定（ch=0 にピアノ風の2オペ音色）
static void setup_fm_voice(OPN& opn) {
    // アルゴリズム 4 (OP1→OP2 直列、FB=4)、ch=0
    opn.write(0xB0, (4 << 3) | 4); // FB=4, ALG=4

    // L/R 出力 ON
    opn.write(0xB4, 0xC0);

    // OP1 (モジュレータ): DT=0, MUL=1, TL=20, AR=31, DR=10, SR=5, SL=5, RR=8
    set_op(opn, 0, 0, 0, 1, 20, 0, 31, 10, 5, 5, 8);
    // OP3 (キャリア): DT=0, MUL=1, TL=0, AR=31, DR=8, SR=3, SL=3, RR=6
    set_op(opn, 0, 2, 0, 1,  0, 0, 31,  8, 3, 3, 6);
    // OP2, OP4 は使わない（TL=127 で無音）
    set_op(opn, 0, 1, 0, 1, 127, 0, 0, 0, 0, 0, 0);
    set_op(opn, 0, 3, 0, 1, 127, 0, 0, 0, 0, 0, 0);
}

// Key ON/OFF
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
    printf("Clock: %u Hz / Sample rate: %u Hz\n", OPN_CLOCK, SAMPLE_RATE);

    OPN opn(OPN_CLOCK, SAMPLE_RATE);
    opn.reset();

    // サンプルレートを設定（ymfm はクロックからサンプルレートを計算）
    // generate() は clock/144 サンプル/秒 の速度で動く
    // → バッファサイズを計算して呼ぶ

    setup_fm_voice(opn);

    WavWriter wav;
    if (!wav.open("output_ymfm_opn.wav", SAMPLE_RATE, 2)) {
        fprintf(stderr, "Cannot open output_ymfm_opn.wav\n");
        return 1;
    }

    const int scale[] = {60, 62, 64, 65, 67, 69, 71, 72};
    const int note_samples = static_cast<int>(SAMPLE_RATE * 0.4);
    const int release_samples = static_cast<int>(SAMPLE_RATE * 0.1);

    // ymfm の内部クロックレート: OPN_CLOCK / 144 サンプル/秒
    // host samplerate との比率でアップサンプリングが必要
    // ymfm::ym2203::generate() は内部レート（OPN_CLOCK/144）で 1 サンプル生成する
    // → 44100Hz に合わせるため 44100 / (OPN_CLOCK/144) の比でループ
    const double ymfm_rate = (double)OPN_CLOCK / 144.0;
    const double rate_ratio = (double)SAMPLE_RATE / ymfm_rate;

    auto t_start = std::chrono::high_resolution_clock::now();
    long long total_samples = 0;

    for (int note : scale) {
        double freq = midi_to_freq(note);
        uint16_t fnum; uint8_t block;
        freq_to_fnum_block(freq, OPN_CLOCK, fnum, block);

        printf("  note=%d freq=%.1f Hz fnum=%u block=%u\n",
               note, freq, fnum, block);

        key_on(opn, 0, fnum, block);

        // note_samples 分生成
        double accumulator = 0.0;
        ymfm::ym2203::output_data out;

        for (int s = 0; s < note_samples + release_samples; s++) {
            if (s == note_samples) key_off(opn, 0);

            // rate_ratio 分だけ ymfm を進める
            accumulator += rate_ratio;
            int32_t sum_l = 0, sum_r = 0;
            int steps = 0;
            while (accumulator >= 1.0) {
                opn.generate(&out);
                // ch0 の FM 出力 (out.data[0])
                // OPN の出力は out.data[0]=FM, out.data[1]=PSG
                sum_l += out.data[0];
                sum_r += out.data[0]; // OPN モノラルなので両ch同じ
                accumulator -= 1.0;
                steps++;
            }
            if (steps > 0) {
                // 正規化（OPN 出力は ~±32768 程度）
                double norm = 32768.0 * steps;
                wav.write_stereo(sum_l / norm, sum_r / norm);
            } else {
                wav.write_stereo(0.0, 0.0);
            }
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
    printf("  出力ファイル   : output_ymfm_opn.wav\n");

    if (cpu_ratio < 0.1)
        printf("  判定: ✅ 余裕あり（リアルタイム処理に問題なし）\n");
    else if (cpu_ratio < 0.5)
        printf("  判定: ✅ 許容範囲内\n");
    else
        printf("  判定: ⚠️  要最適化\n");

    return 0;
}
