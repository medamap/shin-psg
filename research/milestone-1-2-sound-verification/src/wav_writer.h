/**
 * wav_writer.h
 * シンプルな WAV ファイル書き出しユーティリティ
 * 外部ライブラリ不要・ヘッダオンリー
 */
#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>

struct WavWriter {
    FILE* fp = nullptr;
    uint32_t sample_rate;
    uint32_t num_channels;
    uint32_t data_bytes = 0;

    bool open(const std::string& path, uint32_t sr, uint32_t ch) {
        sample_rate = sr;
        num_channels = ch;
        fp = fopen(path.c_str(), "wb");
        if (!fp) return false;
        // プレースホルダーヘッダを書く（後でシークして上書き）
        write_header();
        return true;
    }

    // int16_t サンプルをインターリーブで書き込む
    void write(const int16_t* samples, uint32_t count) {
        fwrite(samples, sizeof(int16_t), count, fp);
        data_bytes += count * sizeof(int16_t);
    }

    // 左右 double サンプル（-1.0〜1.0）を int16 に変換して書き込む
    void write_stereo(double left, double right) {
        auto clamp = [](double v) -> int16_t {
            int32_t s = static_cast<int32_t>(v * 32767.0);
            if (s > 32767) s = 32767;
            if (s < -32768) s = -32768;
            return static_cast<int16_t>(s);
        };
        int16_t buf[2] = { clamp(left), clamp(right) };
        write(buf, 2);
    }

    // モノラル double サンプルを書き込む
    void write_mono(double v) {
        auto clamp = [](double val) -> int16_t {
            int32_t s = static_cast<int32_t>(val * 32767.0);
            if (s > 32767) s = 32767;
            if (s < -32768) s = -32768;
            return static_cast<int16_t>(s);
        };
        int16_t s = clamp(v);
        write(&s, 1);
    }

    void close() {
        if (!fp) return;
        // WAV ヘッダを正しいサイズで書き直す
        fseek(fp, 0, SEEK_SET);
        write_header();
        fclose(fp);
        fp = nullptr;
    }

private:
    void write_u32(uint32_t v) { fwrite(&v, 4, 1, fp); }
    void write_u16(uint16_t v) { fwrite(&v, 2, 1, fp); }

    void write_header() {
        uint32_t byte_rate = sample_rate * num_channels * 2; // 16bit
        uint32_t block_align = num_channels * 2;
        uint32_t data_chunk_size = data_bytes;
        uint32_t riff_size = 36 + data_chunk_size;

        fwrite("RIFF", 1, 4, fp);
        write_u32(riff_size);
        fwrite("WAVE", 1, 4, fp);
        fwrite("fmt ", 1, 4, fp);
        write_u32(16);               // fmt チャンクサイズ
        write_u16(1);                // PCM
        write_u16(num_channels);
        write_u32(sample_rate);
        write_u32(byte_rate);
        write_u16(block_align);
        write_u16(16);               // ビット深度
        fwrite("data", 1, 4, fp);
        write_u32(data_chunk_size);
    }
};
