# シン・PSG プロジェクト構造ガイド v0.1

本ドキュメントはシン・PSGリポジトリのディレクトリ構成・ビルドシステム・開発方針を定義する。
新規参加者のオンボーディングおよびドキュメント・コード配置の判断基準として使用する。

---

## 改訂履歴

| バージョン | 内容 |
|---|---|
| v0.1 | 初版。ディレクトリ構成・プラットフォーム分類・ビルドシステム方針・ブランチ戦略を定義。 |

---

## ディレクトリ構成

```
shin-psg/
│
├── README.md                    # 日本語READMEルート
├── README.en.md                 # 英語README（将来作成）
├── CMakeLists.txt               # CMakeルート（モダンターゲット統括）
│
├── docs/                        # ドキュメント
│   ├── ja/                      # 日本語ドキュメント（一次言語・正）
│   ├── en/                      # 英語ドキュメント（翻訳・スキルで生成）
│   └── progress/                # マイルストン完了記録・意思決定サマリー
│
├── src/                         # モダンプラットフォーム向けソースコード（C++17）
│   ├── core/                    # プラットフォーム非依存コア
│   │   └── CMakeLists.txt
│   ├── platform/                # モダンOS・環境向け実装
│   │   ├── windows/             # Windows固有（音声出力・ファイルIO等）
│   │   ├── macos/
│   │   ├── linux/
│   │   ├── android/             # Android NDK側C++コード（JNIバインディング等）
│   │   ├── ios/
│   │   ├── wasm/                # Emscriptenバインディング・JSグルーコード
│   │   └── rpi-baremetal/       # RPiベアメタル（gcc-arm-none-eabi・CMake管轄内）
│   └── retro/                   # 将来予備（現在未使用）
│
├── retro/                       # レトロ・アセンブラ系（CMake管轄外）
│   └── z80/                     # Z80アーキテクチャ共通
│       ├── core/                # Z80共通コード（各機種からINCLUDEで参照）
│       │   ├── driver/          # サウンドドライバアルゴリズム本体
│       │   ├── feel/            # 感情エンジン簡略実装（Phase 2以降）
│       │   └── mml/             # MMLバイナリデコード処理
│       ├── x1/                  # Sharp X1固有
│       │   ├── platform/        # CTC割り込みハンドラ・X1固有レジスタ定義
│       │   ├── AILZ80ASM.json   # 出力形式：MZT/CMT
│       │   └── Makefile
│       ├── msx/                 # MSX固有
│       │   ├── platform/        # VDP割り込みハンドラ・MSX固有
│       │   ├── AILZ80ASM.json   # 出力形式：ROM/CAS
│       │   └── Makefile
│       └── pc88/                # PC-88固有（詳細調査待ち）
│           ├── platform/
│           ├── AILZ80ASM.json
│           └── Makefile
│
├── platform/                    # IDEプロジェクトファイル・ツールチェーン定義
│   ├── windows/                 # Visual Studioソリューション・.vcxproj等
│   ├── android/                 # Android Studioプロジェクト一式
│   │                            #   → src/core/ src/platform/android/ を相対パス参照
│   ├── ios/                     # Xcodeプロジェクト
│   └── cmake/
│       └── toolchains/          # CMakeクロスコンパイル用ツールチェーンファイル
│           ├── android.cmake
│           ├── wasm.cmake
│           └── rpi-baremetal.cmake
│
├── res/                         # リソースファイル
│   ├── mml/                     # サンプルMMLファイル
│   ├── performers/              # サンプル演者定義ファイル（PML）
│   └── conductors/              # サンプル指揮者定義ファイル
│
└── research/                    # 検証用コード（マイルストン別・一時的なものは削除）
    └── milestone-X-Y-name/      # 例：milestone-1-2-sound-verification
```

---

## プラットフォーム分類

### モダンプラットフォーム（`src/` 管轄）

C++17を使用し、フルスタックのシン・PSG仕様を実装する対象。

| プラットフォーム | 場所 | ビルドシステム | 備考 |
|---|---|---|---|
| Windows | `src/platform/windows/` | CMake | VS2022推奨 |
| macOS | `src/platform/macos/` | CMake | Xcode or CLT |
| Linux | `src/platform/linux/` | CMake | |
| Android | `src/platform/android/` | CMake + NDK | IDEは`platform/android/`に分離 |
| iOS | `src/platform/ios/` | CMake + Xcode | IDEは`platform/ios/`に分離 |
| WASM | `src/platform/wasm/` | CMake + Emscripten | |
| RPi ベアメタル | `src/platform/rpi-baremetal/` | CMake + gcc-arm-none-eabi | OSなし・C/C++ |

### レトロプラットフォーム（`retro/` 管轄）

Z80アセンブラを使用し、サブセット実装を行う対象。ビルドシステムはCMakeの管轄外。

| プラットフォーム | 場所 | アセンブラ | 出力形式 |
|---|---|---|---|
| Sharp X1 | `retro/z80/x1/` | AILZ80ASM | MZT / CMT |
| MSX | `retro/z80/msx/` | AILZ80ASM | ROM / CAS |
| PC-88 | `retro/z80/pc88/` | AILZ80ASM | 調査待ち |

---

## ビルドシステム方針

### CMake（モダンターゲット）

- ルート `CMakeLists.txt` が全モダンターゲットを統括する
- 各ディレクトリに `CMakeLists.txt` を配置し、親からサブディレクトリとして追加する
- クロスコンパイル用ツールチェーンファイルは `platform/cmake/toolchains/` に集約する
- AILZ80ASMのビルドは `add_custom_target` で任意呼び出し可能にする（詳細は `RETRO_GUIDE.md` 参照）

### AILZ80ASM（レトロターゲット）

- 各機種ディレクトリの `Makefile` から呼び出す
- `retro/z80/core/` は各機種からINCLUDEディレクティブで参照する（アセンブル単位は各機種ごと）
- 詳細は [RETRO_GUIDE.md](shin_psg_retro_guide_v0.1.md) を参照

---

## ブランチ戦略

```
main        ← 安定版・タグ付きリリース
└── develop ← 統合ブランチ
    ├── milestone/1-1-license-review
    ├── milestone/1-2-sound-verification
    ├── milestone/1-3-tech-selection
    └── milestone/1-4-project-structure
```

- 各マイルストンは `milestone/X-Y-name`（英語小文字・ハイフン区切り）のブランチで作業する
- マイルストン完了後に `develop` へマージし、`docs/progress/` に意思決定サマリーを追加してコミットする
- `main` へのマージはPhaseの節目等、安定したタイミングで行う

---

## コード・ドキュメントの配置ルール

### ドキュメント

- 日本語が一次言語。日本語版を先に更新し、英語版はスキルで生成する
- 仕様・設計ドキュメントは `docs/ja/` に配置する
- マイルストン完了時の意思決定サマリーは `docs/progress/` に追加する
- `research/` 内の検証コードは、検証結果をドキュメント化してから不要になったものを削除する

### ソースコード

- `src/core/` にはプラットフォーム固有のヘッダ・ライブラリ・APIを一切含めない
- `retro/z80/core/` にはハードウェアレジスタ直接アクセスを含めない（機種依存部分は各機種の `platform/` に分離する）
- リソースファイルのパスは `res/` からの相対パスで記述する

---

## 検証コードの運用

`research/` 以下はマイルストン番号に対応したサブディレクトリで管理する。

| 種別 | 扱い |
|---|---|
| 一時的な動作確認コード | 検証結果を `docs/progress/` に記録後、削除する |
| 資料として残す価値があるもの | そのまま `research/` に保持する |
| 本番実装の雛形になったもの | `src/` または `retro/` に移植後、`research/` 版は削除してよい |
