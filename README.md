# shin-psg

**シン・PSG** — 日本の電子音楽を継承し、進化させた演奏哲学とその実装プロジェクト

*A performance philosophy and implementation project inheriting and evolving Japanese electronic music.*

---

## これは何か / What is this?

シン・PSGは、音源の種類ではなく**演奏方法そのもの**を定義する電子音楽の概念です。

PSG・FM・PCMといった電子音源を用いた音楽において、作曲家の感情的意図（フィールトラック）が指揮者・演者を通じてどのように演奏に反映されるかを体系化します。同じ楽曲データであっても、指揮者と演者の定義によって毎回異なる演奏が生まれます。

*Shin-PSG is a concept that defines not the type of sound source, but the act of performance itself.*

*It systematizes how a composer's emotional intent (Feel Track) is reflected in performance through conductors and performers, using electronic sound sources such as PSG, FM, and PCM. Even with the same musical data, different performances emerge each time depending on the definitions of the conductor and performers.*

---

## チップチューンとの関係 / Relationship with Chiptune

チップチューンは西洋のデモシーン・ゲームミュージック文化を起源とする素晴らしい概念です。

一方、日本の電子音楽はMSX・Sharp X1・PC-88・ファミリーコンピュータ等を基盤として独自の進化を遂げた別の系譜を持ちます。「電子チップを使用しているからチップチューン」という等式は成立しません。両者は起源・文化的背景・発展の経緯が異なる、独立した概念として区別されます。

シン・PSGは日本の電子音楽の系譜に立脚しており、チップチューンを否定するものではなく、明確に区別されるものとして定義されます。

*Chiptune is a wonderful concept originating from Western demoscene and game music culture.*

*Japanese electronic music, on the other hand, has its own lineage that evolved uniquely on platforms such as MSX, Sharp X1, PC-88, and Famicom. The equation "uses electronic chips = chiptune" does not hold. The two are distinguished as independent concepts with different origins, cultural backgrounds, and developmental histories.*

*Shin-PSG is rooted in the lineage of Japanese electronic music. It does not deny chiptune but is defined as something clearly distinct from it.*

---

## 命名について / About the Name

「シン・PSG」の「シン」には複数の意味が重なっています。

- **新** — 新しい定義・概念としての電子音楽
- **真** — 日本の電子音楽の本質を継承するという意志
- **進** — 時代に合わせて進化させた電子音楽
- **深** — 演奏表現の深化

日本の電子音楽はPSGを出発点として発展しました。この名称はその歴史的経緯に基づきます。なお以下の名称も同一の仕様・概念を指します。

- **シン・BEEP** — BEEPサウンドを起源として捉える場合
- **シン・FM** — FM音源を中心として捉える場合
- **シン・PCM** — PCM音源を中心として捉える場合

*The "Shin" in "Shin-PSG" carries multiple meanings: New / True / Progressive / Deep.*

*Japanese electronic music evolved from PSG as its starting point. The following names also refer to the same specification and concept: Shin-BEEP / Shin-FM / Shin-PCM.*

---

## 主な概念 / Key Concepts

### フィールトラック / Feel Track

作曲家が定義する感情の設計図。9種類の感情パラメータ（希望・怒り・悲しみ・喜び・楽しみ・嫉妬・貪欲・虚無・不安）が時間軸上の独立したカーブとして記述されます。

*The composer's emotional blueprint. Nine emotional parameters (Hope, Anger, Sadness, Joy, Fun, Envy, Greed, Void, Anxiety) are described as independent curves on a time axis.*

### 演者 / Performer

各音符トラックを演奏する主体。感情エンベロープ（ADSR）・感情アルゴリズム・演奏癖（ビブラート・トレモロ・ポルタメント）などのパラメータを持ち、フィールトラックの感情を独自に解釈しながら演奏します。

*The entity that performs each note track. Performers have parameters such as emotional envelopes (ADSR), emotion algorithms, and performance habits (vibrato, tremolo, portamento), interpreting the Feel Track's emotions in their own way.*

### 指揮者 / Conductor

フィールトラックを解釈し、演者へ伝達する役割。カリスマ・解釈傾向・伝達遅延などのパラメータを持ちます。

*The entity that interprets the Feel Track and conveys it to performers. Has parameters such as charisma, interpretation tendency, and transmission latency.*

### 感情ベクトル / Emotion Vector

演者内部の感情状態はValence-Arousal平面上のベクトル（θ, strength）として表現されます。フィールトラックの複数感情はこのベクトルに統合され、ノートオンのタイミング・アタック・リリース・演奏癖に反映されます。

*The performer's internal emotional state is expressed as a vector (θ, strength) on the Valence-Arousal plane.*

### 非決定論的演奏 / Non-deterministic Performance

指揮者または演者が定義されている場合、同一の楽曲データであっても毎回異なる演奏結果が生成されます。演奏後のスナップショットを記録することで再現演奏が可能になります。

*When a conductor or performers are defined, different performance results are generated each time from the same musical data. Reproducible playback is possible by recording a post-performance snapshot.*

---

## ドキュメント構成 / Documentation

日本語ドキュメントが一次言語です。英語版は `docs/en/` に配置されます。

| ファイル | 内容 |
|---|---|
| `docs/ja/shin_psg_spec.md` | 仕様書 — 概念・演奏モード・パラメータ一覧 |
| `docs/ja/shin_psg_feel_engine_v0.2.md` | 感情エンジン テクニカルドキュメント — Stage 1〜6の処理フロー・演奏癖パラメータ |
| `docs/ja/shin_psg_architecture_v0.1.md` | アーキテクチャドキュメント — 抽象化・継承・音源別実装 |
| `docs/ja/shin_psg_roadmap_v0.1.md` | 実装ロードマップ — Phase 1〜8・マイルストン・技術スタック候補 |
| `docs/ja/shin_psg_glossary_v0.1.md` | 用語辞典 — 日英対訳・定義一覧 |
| `docs/ja/shin_psg_project_structure_v0.1.md` | プロジェクト構造ガイド — ディレクトリ構成・ビルドシステム・ブランチ戦略 |
| `docs/ja/shin_psg_retro_guide_v0.1.md` | レトロプラットフォーム開発ガイド — Z80移植方針・AILZ80ASM・機種別注意事項 |

---

## プロジェクト構成 / Project Structure

```
shin-psg/
├── src/           モダンプラットフォーム向け実装（C++17）
│   ├── core/      プラットフォーム非依存コア
│   └── platform/  OS・環境別実装（windows/macos/linux/android/ios/wasm/rpi-baremetal）
├── retro/         レトロプラットフォーム向け実装（Z80アセンブラ）
│   └── z80/       Z80共通コア＋機種別実装（x1/msx/pc88）
├── platform/      IDEプロジェクトファイル・CMakeツールチェーン定義
├── res/           リソースファイル（MML・演者定義等）
├── docs/          ドキュメント（ja/en/progress）
└── research/      検証用コード（マイルストン別）
```

詳細は [`docs/ja/shin_psg_project_structure_v0.1.md`](docs/ja/shin_psg_project_structure_v0.1.md) を参照。

---

## 実装ステータス / Implementation Status

現在は **Phase 1（基盤調査・技術選定）** に着手しています。

*Currently in **Phase 1 (Foundation Research and Technology Selection)**.*

---

## 対応音源 / Supported Sound Sources

- PSG（AY-3-8910 / YM2149 / SN76489）
- FM（OPM / OPN / OPNA / OPL系）
- PCM

---

## ライセンス / License

本リポジトリのドキュメントおよび仕様は [Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/) のもとで公開されます。

実装コードのライセンスはPhase 1の技術選定後に確定します。

*The documents and specifications in this repository are published under [Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/).*

*The license for implementation code will be finalized after the technology selection in Phase 1.*

---

## 作者 / Author

Junichi Jobi

---

*初版公開 / First published: 2026*
