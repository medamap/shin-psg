# Milestone 1-1 完了記録

**マイルストン：** Phase 1 / Milestone 1-1 ライセンス精査
**完了日：** 2026-02-21
**担当ブランチ：** milestone/1-1-license-review

---

## 意思決定サマリー

### 採用確定

- **ymfm**（BSD-3-Clause）: FM音源エミュレータ。全プラットフォームで制限なし使用可。
- **ayumi**（MIT）: PSG音源エミュレータ第一候補。emu2149との比較はMilestone 1-2へ。
- **miniaudio**（Unlicense/MIT-0）: オーディオ出力。帰属表示すら不要で最も自由。

### 不採用確定（理由）

- **libADLMIDI** / **libOPNMIDI**: GPL v2+ / GPL v3+ コンポーネントを含み汚染リスクが高い。ymfmで機能代替可能なため除外。
- **Nuked-OPM**: LGPL-2.1。ymfmがOPM(YM2151)に対応しているため不要。

### 見送り・保留

- **emu2149**（MIT）: ライセンス上は問題なし。ayumiとの技術比較をMilestone 1-2で実施して最終判断。

---

## 調査結果の要点

GPL混在ライブラリ（libADLMIDI/libOPNMIDI）を除外した結果、
採用候補のすべてがBSD/MIT/Unilcense系に統一でき、当初想定より良好な状況だった。

ymfmがOPM/OPN/OPN2/OPNA/OPL系を一括でカバーしているため、
FM音源エミュレータは単一ライブラリで済む。これはライセンス管理の観点でも好都合。

---

## 成果物

- `docs/ja/milestone-1-1-license-evaluation_v0.1.md` — ライセンス評価レポート（詳細版）
