/**
 * psg_ayumi_test.cpp
 * Milestone 1-2: ayumi ライブラリを使った PSG 音源エミュレーション検証
 *
 * 出力: output_ayumi.wav
 * 内容: C メジャースケール（C4〜C5）を各音 0.4 秒、ステレオで鳴らす
 * 計測: サンプル生成に要した CPU 時間
 */

#include <cstdio>
#include <cmath>
#include <chrono>
#include "wav_writer.h"

extern "C" {
#include "ayumi.h"
}

// AY-3-8910 のクロック周波数（MSX: 1.789773MHz, X1: 1MHz系）
static constexpr double AY_CLOCK_MSX = 1789773.0;
static constexpr int SAMPLE_RATE = 44100;

// 音名を AY のトーンピリオドへ変換
// AY period = clock / (16 * f)
static int freq_to_period(double freq, double clock) {
    return static_cast<int>(clock / (16.0 * freq) + 0.5);
}

// 標準 A4=440Hz からの音程を周波数へ
static double midi_to_freq(int midi_note) {
    return 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
}

int main() {
    printf("=== ayumi PSG emulation test ===\n");
    printf("Clock: %.0f Hz / Sample rate: %d Hz\n", AY_CLOCK_MSX, SAMPLE_RATE);

    struct ayumi ay;
    if (!ayumi_configure(&ay, 1 /*YM2149*/, AY_CLOCK_MSX, SAMPLE_RATE)) {
        fprintf(stderr, "ayumi_configure failed\n");
        return 1;
    }

    // ch0: 中央、ch1: 左、ch2: 右
    ayumi_set_pan(&ay, 0, 0.5, 0);
    ayumi_set_pan(&ay, 1, 0.1, 0);
    ayumi_set_pan(&ay, 2, 0.9, 0);

    // ノイズ無効・エンベロープ無効・音量 12
    for (int ch = 0; ch < 3; ch++) {
        ayumi_set_mixer(&ay, ch, /*t_off=*/0, /*n_off=*/1, /*e_on=*/0);
        ayumi_set_volume(&ay, ch, 12);
    }

    WavWriter wav;
    if (!wav.open("output_ayumi.wav", SAMPLE_RATE, 2)) {
        fprintf(stderr, "Cannot open output_ayumi.wav\n");
        return 1;
    }

    // C メジャースケール: C4(60)〜C5(72) 各 0.4 秒
    const int scale[] = {60, 62, 64, 65, 67, 69, 71, 72};
    const int note_samples = static_cast<int>(SAMPLE_RATE * 0.4);

    auto t_start = std::chrono::high_resolution_clock::now();
    long long total_samples = 0;

    for (int note : scale) {
        double freq = midi_to_freq(note);
        int period = freq_to_period(freq, AY_CLOCK_MSX);
        ayumi_set_tone(&ay, 0, period);
        // ch1 を 1オクターブ上でハーモニー
        ayumi_set_tone(&ay, 1, freq_to_period(freq * 2.0, AY_CLOCK_MSX));
        // ch2 は 5 度上（7 半音）
        ayumi_set_tone(&ay, 2, freq_to_period(midi_to_freq(note + 7), AY_CLOCK_MSX));

        printf("  note=%d freq=%.1f Hz period=%d\n", note, freq, period);

        for (int s = 0; s < note_samples; s++) {
            // 音の後半は音量を下げてリリース風に
            int vol = (s < note_samples * 0.8) ? 12 :
                      static_cast<int>(12.0 * (1.0 - (s - note_samples * 0.8) / (note_samples * 0.2)));
            ayumi_set_volume(&ay, 0, vol);
            ayumi_set_volume(&ay, 1, vol / 2);
            ayumi_set_volume(&ay, 2, vol / 2);

            ayumi_process(&ay);
            ayumi_remove_dc(&ay);
            wav.write_stereo(ay.left, ay.right);
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
    printf("  出力ファイル   : output_ayumi.wav\n");

    if (cpu_ratio < 0.05)
        printf("  判定: ✅ 余裕あり（リアルタイム処理に問題なし）\n");
    else if (cpu_ratio < 0.3)
        printf("  判定: ✅ 許容範囲内\n");
    else
        printf("  判定: ⚠️  要最適化\n");

    return 0;
}
