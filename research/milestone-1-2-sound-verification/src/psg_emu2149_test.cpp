/**
 * psg_emu2149_test.cpp
 * Milestone 1-2: emu2149 ライブラリを使った PSG 音源エミュレーション検証
 *
 * 出力: output_emu2149.wav
 * 内容: ayumi と同じ C メジャースケールを鳴らして精度・負荷を比較
 * 計測: サンプル生成に要した CPU 時間
 */

#include <cstdio>
#include <cmath>
#include <chrono>
#include "wav_writer.h"

extern "C" {
#include "emu2149.h"
}

static constexpr uint32_t AY_CLOCK_MSX = 1789773;
static constexpr uint32_t SAMPLE_RATE  = 44100;

// 周波数 → AY トーンピリオド
static uint16_t freq_to_period(double freq, uint32_t clock) {
    return static_cast<uint16_t>(clock / (16.0 * freq) + 0.5);
}

static double midi_to_freq(int midi_note) {
    return 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
}

// AY レジスタ書き込みヘルパー
static void psg_write(PSG* psg, uint32_t reg, uint32_t val) {
    PSG_writeReg(psg, reg, val);
}

// トーンピリオドをレジスタペアに書く (reg = ch*2)
static void psg_set_tone(PSG* psg, int ch, uint16_t period) {
    psg_write(psg, ch * 2,     period & 0xFF);
    psg_write(psg, ch * 2 + 1, (period >> 8) & 0x0F);
}

// ミキサー設定: ch ごとにトーン ON、ノイズ OFF
// ミキサーレジスタ (R7): bit = 0 で有効
// ch0=bit0(tone), bit3(noise) など
static void psg_set_mixer_tone_only(PSG* psg) {
    // ビット 0-2: トーン OFF フラグ（0=ON）
    // ビット 3-5: ノイズ OFF フラグ（1=OFF）
    psg_write(psg, 7, 0b00111000); // tone all ON, noise all OFF
}

int main() {
    printf("=== emu2149 PSG emulation test ===\n");
    printf("Clock: %u Hz / Sample rate: %u Hz\n", AY_CLOCK_MSX, SAMPLE_RATE);

    PSG* psg = PSG_new(AY_CLOCK_MSX, SAMPLE_RATE);
    if (!psg) {
        fprintf(stderr, "PSG_new failed\n");
        return 1;
    }
    PSG_reset(psg);
    PSG_setQuality(psg, 1);  // 高品質モード

    psg_set_mixer_tone_only(psg);
    // 音量設定（ch0: 15, ch1: 8, ch2: 8）
    psg_write(psg, 8, 15);
    psg_write(psg, 9, 8);
    psg_write(psg, 10, 8);

    WavWriter wav;
    if (!wav.open("output_emu2149.wav", SAMPLE_RATE, 1)) { // モノラル
        fprintf(stderr, "Cannot open output_emu2149.wav\n");
        PSG_delete(psg);
        return 1;
    }

    const int scale[] = {60, 62, 64, 65, 67, 69, 71, 72};
    const int note_samples = static_cast<int>(SAMPLE_RATE * 0.4);

    auto t_start = std::chrono::high_resolution_clock::now();
    long long total_samples = 0;

    for (int note : scale) {
        double freq = midi_to_freq(note);
        uint16_t period = freq_to_period(freq, AY_CLOCK_MSX);
        psg_set_tone(psg, 0, period);
        psg_set_tone(psg, 1, freq_to_period(freq * 2.0, AY_CLOCK_MSX));
        psg_set_tone(psg, 2, freq_to_period(midi_to_freq(note + 7), AY_CLOCK_MSX));

        printf("  note=%d freq=%.1f Hz period=%u\n", note, freq, period);

        for (int s = 0; s < note_samples; s++) {
            // リリース処理
            int vol = (s < note_samples * 0.8) ? 15 :
                      static_cast<int>(15.0 * (1.0 - (s - note_samples * 0.8) / (note_samples * 0.2)));
            psg_write(psg, 8, vol);
            psg_write(psg, 9, vol / 2);
            psg_write(psg, 10, vol / 2);

            int16_t sample = PSG_calc(psg);
            // int16 をそのまま書く（emu2149 は int16 出力）
            wav.write(&sample, 1);
            total_samples++;
        }
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    wav.close();
    PSG_delete(psg);

    double elapsed = std::chrono::duration<double>(t_end - t_start).count();
    double audio_duration = static_cast<double>(total_samples) / SAMPLE_RATE;
    double cpu_ratio = elapsed / audio_duration;

    printf("\n--- 計測結果 ---\n");
    printf("  生成サンプル数 : %lld\n", total_samples);
    printf("  音声長さ       : %.2f 秒\n", audio_duration);
    printf("  CPU 時間       : %.4f 秒\n", elapsed);
    printf("  CPU 負荷比     : %.4f (1.0 = リアルタイム上限)\n", cpu_ratio);
    printf("  出力ファイル   : output_emu2149.wav\n");

    if (cpu_ratio < 0.05)
        printf("  判定: ✅ 余裕あり（リアルタイム処理に問題なし）\n");
    else if (cpu_ratio < 0.3)
        printf("  判定: ✅ 許容範囲内\n");
    else
        printf("  判定: ⚠️  要最適化\n");

    return 0;
}
