# Milestone 1-2 Sound Emulator Verification

Milestone 1-2 の音源エミュレーション検証コードです。

## ライブラリの取得

```bash
cd libs/
git clone --depth=1 https://github.com/true-grue/ayumi.git
git clone --depth=1 https://github.com/digital-sound-antiques/emu2149.git
git clone --depth=1 https://github.com/aaronsgiles/ymfm.git
# miniaudio (ヘッダオンリー)
curl -LO https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
```

## ビルド（CMake）

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## ビルド（g++ 直接）

```bash
mkdir -p build

# ayumi PSG テスト
gcc -O2 -c libs/ayumi/ayumi.c -I libs/ayumi -o build/ayumi.o
g++ -O2 -std=c++17 src/psg_ayumi_test.cpp build/ayumi.o -I src -I libs/ayumi -lm -o build/psg_ayumi_test

# emu2149 PSG テスト
gcc -O2 -c libs/emu2149/emu2149.c -I libs/emu2149 -o build/emu2149.o
g++ -O2 -std=c++17 src/psg_emu2149_test.cpp build/emu2149.o -I src -I libs/emu2149 -lm -o build/psg_emu2149_test

# ymfm FM テスト
for f in libs/ymfm/src/*.cpp; do
    g++ -O2 -std=c++17 -c "$f" -I libs/ymfm/src -o build/$(basename ${f%.cpp}).o
done
g++ -O2 -std=c++17 src/fm_ymfm_test.cpp build/ymfm_*.o -I src -I libs/ymfm/src -lm -o build/fm_ymfm_test
```

## 実行

```bash
cd build
./psg_ayumi_test    # → output_ayumi.wav
./psg_emu2149_test  # → output_emu2149.wav
./fm_ymfm_test      # → output_ymfm_opn.wav
```

## 計測結果（Linux / g++ 11.4 / -O2）

| ライブラリ | CPU負荷比 | 判定 |
|---|---|---|
| ayumi | 0.0138 | ✅ |
| emu2149 | 0.0066 | ✅ |
| ymfm (OPN) | 0.0104 | ✅ |

詳細は `docs/ja/milestone-1-2-sound-verification.md` を参照。
