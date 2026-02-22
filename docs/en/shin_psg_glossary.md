# Shin-PSG Glossary v0.3

This glossary systematically defines terms used in the Shin-PSG project.
Japanese is the primary language; English names are listed alongside.
Its purpose is to ensure consistency of terminology across translations, documentation, and skill processing.

---

## Revision History

| Version | Notes |
|---|---|
| v0.3 | Added Section 15: Software Architecture Terms. New entries: Onion Architecture, DDD, ISoundSource, ISoundSourceDescriptor, VoiceSchema, C ABI Bridge, Self-describing Plugin, and each domain name. |
| v0.2 | Added retro and embedded development terms (Tick, CTC, VDP, Bare Metal, etc.) and license terms. Added AILZ80ASM to external tools. |
| v0.1 | Initial version. Comprehensively collected terms from the specification, Feel Engine, architecture, and roadmap documents. |

---

## Table of Contents

1. [Core Concepts](#1-core-concepts)
2. [Performance Entities](#2-performance-entities)
3. [Performance Modes](#3-performance-modes)
4. [9 Emotional Parameters](#4-9-emotional-parameters)
5. [Performer Parameters](#5-performer-parameters)
6. [Conductor and Leader Parameters](#6-conductor-and-leader-parameters)
7. [Performance Trait Parameters](#7-performance-trait-parameters)
8. [Feel Engine Processing Stages](#8-feel-engine-processing-stages)
9. [Emotion Algorithm](#9-emotion-algorithm)
10. [Sound Sources and Chips](#10-sound-sources-and-chips)
11. [Libraries and Tools](#11-libraries-and-tools)
12. [Project-Specific Terms](#12-project-specific-terms)
13. [Retro and Embedded Development Terms](#13-retro-and-embedded-development-terms)
14. [License Terms](#14-license-terms)
15. [Software Architecture Terms](#15-software-architecture-terms)

---

## 1. Core Concepts

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| シン・PSG | Shin-PSG | A concept of electronic music that defines performance method itself, not the type of sound source. Inherits and evolves the lineage of Japanese electronic music. | spec, README |
| シン・BEEP | Shin-BEEP | An alternate name for Shin-PSG; used when viewed as originating from BEEP sound. | spec, README |
| シン・FM | Shin-FM | An alternate name for Shin-PSG; used when centered on FM sound sources. | spec, README |
| シン・PCM | Shin-PCM | An alternate name for Shin-PSG; used when centered on PCM sound sources. | spec, README |
| 音符トラック | Note Track | A track describing note data for each part on a time axis. Created using MML or piano roll format. | spec |
| フィールトラック | Feel Track | An emotional parameter track defined per part of a composition. 9 emotional types are described as curves on a time axis. Represents the composer's emotional intent. | spec, feel_engine |
| 感情ベクトル | Emotion Vector | The Performer's internal emotional state expressed as a vector (θ, strength) on the Valence-Arousal plane. | spec, feel_engine |
| 非決定論的演奏 | Non-deterministic Performance | The property by which different performance results are generated each time from the same musical data when a Conductor or Performer is defined. | spec |
| 決定論的演奏 | Deterministic Performance | Normal playback without Conductor or Performers defined. The same data always produces the same performance result. | spec |
| スナップショット | Snapshot | A file recording all parameters and emotion logs after a performance. Reproducible playback (Deterministic replay) from a snapshot is possible. | spec, roadmap |
| 発狂状態 | Breakdown State / Breakdown | The state entered when a Performer's internal emotional parameters exceed the Limiter. Performance becomes uncontrollable. | spec |
| フリーズ型崩壊 | Freeze Breakdown | A form of Breakdown State. A collapse where performance stops completely. | spec |
| 暴走 | Runaway | A form of Breakdown State. A collapse where notes unrelated to the original track are sounded. | spec |
| Valence-Arousal平面 | Valence-Arousal Plane | A psychological coordinate system expressing emotions on two axes: "pleasant/unpleasant (Valence)" and "arousal level (Arousal)." The core of the Feel Engine. | feel_engine |
| θ（シータ） | θ (Theta) | The angle of the vector on the Valence-Arousal plane. Represents the direction and quality of the emotion. | feel_engine |
| strength（強度） | strength | The magnitude of the vector on the Valence-Arousal plane. Represents how strongly the emotion influences performance. The closer to 0, the less the influence. | feel_engine |
| 経験値 | Experience / EXP | Growth data obtained by Performers and Conductors after a performance. Recorded in a separate file and loaded as cache for the next performance. | feel_engine |

---

## 2. Performance Entities

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| 演者 | Performer | The entity that performs each Note Track. Holds Emotion Envelope, Emotion Algorithm, and Performance Trait parameters; independently interprets the Feel Track's emotions and performs. | spec, architecture |
| 指揮者 | Conductor | Interprets the Feel Track and conveys emotional information to each Performer. Has Charisma, Interpretation, and Latency parameters. Does not function without Performers present. | spec |
| リーダー演者 | Leader Performer / Leader | A Performer that functions as a Conductor substitute. Performs itself while conveying its emotional state to other Performers. Intended for assignment to the tempo/groove anchor part, such as drums in a band. | spec |
| 基底演者 | Base Performer | A Performer that serves as the starting point for inheritance. A Performer with all parameters fully defined. | architecture |
| 派生演者 | Derived Performer | A Performer defined by inheriting from a Base Performer or another Performer and overwriting only the differential parameters. | architecture |
| 基底指揮者 | Base Conductor | A Conductor that serves as the starting point for inheritance. | architecture |
| 派生指揮者 | Derived Conductor | A Conductor defined by inheriting from a Base Conductor and overwriting differential parameters. | architecture |

---

## 3. Performance Modes

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| モード1（通常演奏） | Mode 1 (Normal Performance) | Both Conductor and Performers are undefined. Feel Track is disabled. Deterministic normal music playback. | spec |
| モード2（演者のみ） | Mode 2 (Performer Only) | Only Performers defined; no Conductor. Each Performer independently interprets the Feel Track. Emotional states of Performers are independent. | spec |
| モード3（標準構成） | Mode 3 (Standard Configuration) | Conductor present, Performers present. The standard configuration where the Conductor interprets the Feel Track and conveys it to each Performer. | spec |
| モード4（指揮者のみ） | Mode 4 (Conductor Only) | Conductor present, no Performers. Effectively equivalent to Mode 1. | spec |
| モード5（バンド構成） | Mode 5 (Band Configuration) | Leader Performer present, no Conductor. A band format where the Leader functions as a Conductor substitute. | spec |
| プラクティス | Practice | A performance mode type. Practice and test performances. No audience. Emotions enter in a restrained manner. | roadmap |
| ステージング | Staging | A performance mode type. Concerts and live performances. The emotional intensity of Performers changes with audience count. | roadmap |
| レコーディング | Recording | A performance mode type. Studio recording. No audience; emphasis on snapshot recording. | roadmap |

---

## 4. 9 Emotional Parameters

Emotional parameters available in the Feel Track. Each value is normalized to 0.0–1.0.

| Japanese | English | Valence | Arousal | Notes |
|---|---|---|---|---|
| 希望 | Hope | Pleasant (+) | Mid | |
| 怒り | Anger | Unpleasant (−) | High | |
| 悲しみ | Sadness | Unpleasant (−) | Low | |
| 喜び | Joy | Pleasant (+) | High | |
| 楽しみ | Fun | Pleasant (+) | Mid–High | |
| 嫉妬 | Envy | Unpleasant (−) | High | Special effect: works to unbalance volume relative to other parts |
| 貪欲 | Greed | Unpleasant (−) | High | |
| 虚無 | Void | Unpleasant (−) | Low | |
| 不安 | Anxiety | Unpleasant (−) | Mid–High | |

---

## 5. Performer Parameters

### 5-1. Emotional Sensitivity Parameters

| Japanese | English | Definition |
|---|---|---|
| 楽観度 | Optimism | Sensitivity to Hope, Joy, and Fun. |
| 忍耐度 | Patience | Tolerance for Anger and Anxiety. The lower this is, the more prone to overreaction. |
| 真面目度 | Seriousness | Sensitivity to Sadness, Void, and Envy. |

### 5-2. Emotional Dynamics Parameters

| Japanese | English | Definition |
|---|---|---|
| 感度 | Sensitivity | How quickly and strongly the Performer reacts to emotional changes. |
| リミッター | Limiter | The upper and lower bounds of emotional parameter fluctuation. Exceeding this causes a transition to Breakdown State. |
| 感情減衰速度 | Decay Rate | The speed at which received emotions converge. |
| 追従性 | Tracking | The ability to follow sudden change curves in the Feel Track. |

### 5-3. Technical Parameters

| Japanese | English | Definition |
|---|---|---|
| 熟練度 | Skill | Overall performance precision. The lower this is, the more directly and naturally emotions show in performance. The higher it is, the faster the decay rate even when emotions run high, making it easier to maintain performance precision. |

### 5-4. Emotion Envelope Parameters (9 emotions × ADSR)

36 envelope parameters exist per Performer (9 emotions × 4 parameters).

| Japanese | English | Definition |
|---|---|---|
| アタック | Attack | The time for the Performer's internal emotional value to follow when the Feel Track value rises. Shorter = reacts immediately (short-tempered). |
| ディケイ | Decay | The time for the emotional value to settle down to the Sustain level after reaching its peak. |
| サスティン | Sustain | The stable level (0.0–1.0) while the emotional trigger persists. |
| リリース | Release | The time for the Performer's internal emotional value to return to zero after the Feel Track emotional value falls. |

---

## 6. Conductor and Leader Parameters

Parameters held by the Conductor. All Performers also hold these parameters (for Leader functionality).

| Japanese | English | Definition |
|---|---|---|
| 解釈傾向 | Interpretation | The tendency for how the entity interprets the Feel Track's emotions. Defines whether to exaggerate or suppress. |
| カリスマ | Charisma | The intensity of Feel transmission to Performers. The higher this is, the stronger the influence on Performers; the lower it is, the more Performers follow their own parameters. |
| 伝達遅延 | Latency | The time delay before Feel Track changes are conveyed to Performers. |
| 信頼感 | Trust | The degree of trust in the Leader Performer. The higher this is, the more readily the Performer accepts the Leader's emotional state; the lower it is, the stronger the tendency toward independent interpretation. |

---

## 7. Performance Trait Parameters

Playing habits held by Performers. Automatically added to performance if not explicitly specified in MML.
When explicitly specified in MML, the MML specification takes precedence (override).

### 7-1. Vibrato Traits

| Japanese | English | Definition |
|---|---|---|
| ビブラート好き度 | Vibrato Affinity | How much the Performer tends to apply vibrato. 0 = almost never, 1.0 = applies actively. |
| 開始タイミング | Vibrato Delay | The time after note-on before vibrato begins. |
| 深さ揺れ | Vibrato Depth Fluctuation | The degree to which vibrato depth fluctuates in conjunction with emotion. |
| 速度変化感度 | Vibrato Rate Sensitivity | How much vibrato speed changes in response to emotional changes. |

### 7-2. Tremolo Traits

| Japanese | English | Definition |
|---|---|---|
| トレモロ好き度 | Tremolo Affinity | How much the Performer tends to apply tremolo. |
| 感情連動強度 | Tremolo Emotion Sensitivity | How much tremolo increases or decreases in response to emotional changes. |
| 閾値 | Tremolo Threshold | The strength value above which tremolo begins to occur. |

### 7-3. Portamento Traits

| Japanese | English | Definition |
|---|---|---|
| ポルタメント好き度 | Portamento Affinity | How much the Performer tends to apply portamento. |
| カーブ形状 | Portamento Curve | The basic curve shape of pitch movement: ease-in, ease-out, S-curve, linear, etc. |
| アンダーシュート量 | Undershoot Amount | How much pitch drops before ascending. 0 = none. A natural habit of string players and vocalists. |
| アンダーシュートカーブ | Undershoot Curve | The curve shape of the transition from the dip back to the ascending motion. |
| オーバーシュート量 | Overshoot Amount | How much pitch overshoots after arriving at the target. 0 = none. |
| オーバーシュートカーブ | Overshoot Curve | The curve shape of the transition from the overshoot back. |

---

## 8. Feel Engine Processing Stages

The processing flow by which emotional data from the Feel Track is converted into performance expression.

| Stage | Name | English | Summary |
|---|---|---|---|
| Stage 1 | フィールトラック入力 | Feel Track Input | Input as 9 independent emotion curves. No effect on performance occurs at this stage. |
| Stage 2 | 感情エンベロープ処理 | Emotion Envelope Processing | Temporally shapes each emotion's value using the Performer's Emotion Envelope (ADSR). |
| Stage 3 | 感情アルゴリズム処理 | Emotion Algorithm Processing | Synthesizes emotions according to the modulation relationships (Emotion Algorithm) between them. |
| Stage 4 | Valence-Arousalベクトル変換 | Valence-Arousal Vector Conversion | Converts processed emotional data into a 2D vector of θ and strength. |
| Stage 5 | 解釈フィルタ | Interpretation Filter | Further transforms the vector through the interpretation filters of the Conductor, Leader, and Performer. |
| Stage 6 | 演奏表現への反映 | Performance Expression Mapping | Reflects the final vector in note-on timing, Attack, Release, and Performance Traits. |
| Stage 6-B | 演奏癖パラメータ | Performance Trait Parameters | Adds Vibrato, Tremolo, and Portamento habits in conjunction with the Emotion Vector and applies them to performance. |

---

## 9. Emotion Algorithm

The topology definition of modulation relationships between emotions. Equivalent to the algorithm concept in FM sound sources.

| Japanese | English | Definition |
|---|---|---|
| キャリア感情 | Carrier Emotion | An emotion that directly contributes to the Valence-Arousal vector. |
| モジュレータ感情 | Modulator Emotion | An emotion that modulates the conversion characteristics of the Carrier Emotion. |
| 直列接続 | Series Connection | A topology where the Modulator Emotion modulates the Carrier Emotion before reaching the final output. Effects tend to be amplified, producing complex emotional texture. |
| 並列接続 | Parallel Connection | A topology where multiple emotions independently contribute to the vector and are vector-summed. When opposing emotions are connected in parallel, strength decreases. |
| フィードバック接続 | Feedback Connection | A loop topology where an emotion self-modulates. Emotions tend to self-reinforce and can be the shortest path to Breakdown State. |
| 感情アルゴリズム | Emotion Algorithm | The entire topology of modulation relationships among 9 emotions. Defined per Performer. |

---

## 10. Sound Sources and Chips

| Japanese | English / Model | Category | Representative Devices |
|---|---|---|---|
| PSG（プログラマブルサウンドジェネレータ） | PSG / Programmable Sound Generator | PSG | MSX, ZX Spectrum, etc. |
| AY-3-8910 | AY-3-8910 | PSG | MSX, etc. |
| YM2149 | YM2149 | PSG | Sharp X68000, etc. (AY-3-8910 compatible) |
| SN76489 | SN76489 | PSG | Sega devices |
| FM音源 | FM Sound Generator | FM | Various |
| OPM | OPM / YM2151 | FM | Sharp X68000 |
| OPN | OPN / YM2203 | FM | PC-88 |
| OPNA | OPNA / YM2608 | FM | PC-98 |
| OPL系 | OPL / YMF262 (OPL3) | FM | PC sound cards, etc. |
| PCM音源 | PCM Sound Source | PCM | Various |

---

## 11. Libraries and Tools

### Sound Emulator Libraries

| Name | Supported Chips | License | Purpose |
|---|---|---|---|
| ymfm | OPM/OPN/OPL series | BSD-3-Clause | FM sound emulator (first choice) |
| libADLMIDI | OPL3 (YMF262) | LGPL2.1+/MIT mixed | MIDI integration; virtual multi-chip |
| libOPNMIDI | OPN2 (YM2612) / OPNA (YM2608) | LGPL2.1+/MIT mixed | Sister library of libADLMIDI |
| Nuked-OPM | OPM (YM2151) | LGPL2.1+ | X68000 series; high accuracy |
| ayumi | AY-3-8910 / YM2149 | MIT | PSG sound emulator (first choice) |
| emu2149 | YM2149 / PSG | MIT | Lightweight PSG emulator |
| MAME internal core | SN76489, etc. | BSD/GPL mixed | Requires license review due to GPL mix |

### Audio Output Libraries

| Name | License | Notes |
|---|---|---|
| miniaudio | MIT/Unlicense | Header-only; no dependencies; lightest weight (first choice) |
| PortAudio | MIT | Low latency; professional grade |
| SDL2 Audio | zlib | Cross-platform; proven track record |

### External Tools and Services

| Name | Purpose |
|---|---|
| OBS Studio | Video/audio capture and streaming (obs-websocket port 4455 built-in) |
| YouTube Live Streaming API | Retrieve comments and Super Chats |
| Streamlabs Socket API | Unified event retrieval from YouTube/Twitch, etc. |
| Social Stream Ninja | Chat aggregation across multiple platforms |
| AILZ80ASM | Z80 assembler (C#/.NET 8, MIT, self-contained binary). Assembles for X1/MSX/PC-88. Output formats: MZT/CMT (X1), ROM/CAS (MSX) |

---

## 12. Project-Specific Terms

| Japanese | English | Definition |
|---|---|---|
| MML（ミュージックマクロ言語） | MML / Music Macro Language | The notation language for musical data. Used for describing Note Tracks and Feel Tracks. |
| PML（パーソナルマクロ言語） | PML / Personal Macro Language | The notation language for Performer and Conductor parameters. Describes Emotion Envelopes, Emotion Algorithms, Performance Trait parameters, and inheritance relationships. |
| シン・エンジン | Shin Engine | The core engine that processes performance binaries converted from MML in real time. |
| シン・コンパイラ | Shin Compiler | The compiler from MML to Shin Engine binaries. |
| PMLコンパイラ | PML Compiler | The compiler from PML to Performer/Conductor binaries. |
| シン・アカデミー | Shin Academy | Implementation of Performer and Conductor training, experience, and export features. Has both local and social platform versions. |
| フィールフィードバック | Feel Feedback | A feature where audience reactions (comments, Super Chats, etc.) during live streaming influence the Feel Track. |
| オーディエンス | Audience | Viewers of live streams and concerts. Audience count influences the emotional intensity of Performers. |
| 演奏癖 | Performance Trait | Playing habits held by Performers. Three types: Vibrato, Tremolo, and Portamento. |
| 感情エンベロープ | Emotion Envelope | An ADSR-structured envelope that shapes the temporal change characteristics of each emotion. Equivalent to the volume envelope in FM sound sources, but the control target is emotional intensity values. |
| 継承 | Inheritance | In the definition of Performers and Conductors, a mechanism for defining a new Performer or Conductor by using an existing one as a base and overwriting only the differential parameters. |
| サウンドドライバ抽象レイヤー | Sound Driver Abstraction Layer | A common interface that does not depend on sound source type. Performers only need to hold abstract-level traits and emotional parameters; the concrete implementation for the sound source is resolved at runtime. |
| PerformanceTrait | PerformanceTrait | The abstract class for Performance Traits. Vibrato, Tremolo, and Portamento inherit from this. |
| EmotionAlgorithm | EmotionAlgorithm | The interface for the Emotion Algorithm. An implementation is assigned per Performer. |

---

## 13. Retro and Embedded Development Terms

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| チック | Tick | The minimum time unit in an interrupt-driven music driver. Counted by 1 each time an interrupt fires. Tempo depends on the Tick Rate. | retro_guide |
| チックレート | Tick Rate | The number of ticks per second. Differs per machine (X1: freely set via CTC; MSX: fixed 60Hz via VDP V-SYNC). The MML compiler converts wait values to match the tick rate. | retro_guide |
| チック抽象化 | Tick Abstraction | A design pattern where machine-specific code provides a `TICKS_PER_SEC` constant so that shared code can operate without knowing the tick rate. | retro_guide |
| CTC | CTC / Counter Timer Circuit | A counter/timer chip peripheral for Z80. Used on the Sharp X1 to fire interrupts at any arbitrary period. High flexibility for tempo management. | retro_guide |
| VDP | VDP / Video Display Processor | The video processor in MSX. Fires interrupts at V-SYNC (NTSC: 60Hz; PAL: 50Hz). MSX music drivers piggyback on this interrupt. | retro_guide |
| ベアメタル | Bare Metal | An execution environment that directly controls hardware without an OS. The RPi (Raspberry Pi) bare-metal target falls into this category. Implemented in C/C++. | project_structure |
| バンク切り替え | Bank Switching | A technique for dynamically switching memory regions to exceed the Z80's 64KB address space limitation. | retro_guide |

---

## 14. License Terms

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| GPL汚染 | GPL Contamination | The problem where linking GPL-licensed code makes the conditions of the GPL (obligation to disclose source code) apply to the entire project. Must be avoided in commercial and closed-source development. | milestone-1-1-license-evaluation |
| LGPL | LGPL / GNU Lesser General Public License | Less restrictive than GPL. Static linking may pose GPL Contamination risk, but dynamic linking is generally acceptable in most cases. | milestone-1-1-license-evaluation |
| スタティックリンク | Static Linking | A linking method where a library is directly embedded in the binary. Statically linking an LGPL library carries a contamination risk. | milestone-1-1-license-evaluation |
| ダイナミックリンク | Dynamic Linking | A linking method where a library is loaded from an external DLL/SO at runtime. For LGPL libraries, this method often avoids contamination. | milestone-1-1-license-evaluation |
| 帰属表示 | Attribution Notice | The copyright notice obligation required by BSD licenses, etc. Typically recorded in a NOTICE file or displayed in an About screen. | milestone-1-1-license-evaluation |
| BSD-3-Clause | BSD 3-Clause License | A permissive license with three conditions: "retain copyright notice," "no warranty disclaimer," and "no unauthorized use of author name." No GPL Contamination. Adopted by ymfm. | milestone-1-1-license-evaluation |
| Unlicense / MIT-0 | Unlicense / MIT No Attribution | Effectively equivalent to public domain. No attribution required. Adopted by miniaudio. | milestone-1-1-license-evaluation |

---

## 15. Software Architecture Terms

| Japanese | English | Definition | Reference Documents |
|---|---|---|---|
| オニオンアーキテクチャ | Onion Architecture | A DDD architecture style. The Domain sits at the center, surrounded by concentric layers: Application, Infrastructure, and Presentation. Dependencies always flow inward; inner layers have no knowledge of outer layers. | architecture |
| DDD / ドメイン駆動設計 | DDD / Domain-Driven Design | A software design methodology that places business logic at the center and structures the system around a domain model. Shin-PSG adopts this in combination with Onion Architecture. | architecture |
| Domain 層 | Domain Layer | The innermost layer of Onion Architecture. Holds interfaces, models, enums, and business logic as a pure abstraction layer with no dependencies on external libraries or other layers. | architecture |
| Application 層 | Application Layer | The layer holding API entry points and service startup logic. May only depend on the Domain layer. | architecture |
| Infrastructure 層 | Infrastructure Layer | The layer holding concrete implementations (adapters for external libraries, etc.). May only depend on the Domain layer. | architecture |
| Presentation 層 | Presentation Layer | The layer handling UI display and user input. Empty for domains that have no UI. | architecture |
| SoundSource ドメイン | SoundSource Domain | The domain responsible for sound source plugin abstraction, loading, and management. Centers on ISoundSource and ISoundSourceDescriptor. | architecture |
| SoundEngine ドメイン | SoundEngine Domain | The domain responsible for tick management, note scheduling, and performance orchestration. | architecture |
| SoundOutput ドメイン | SoundOutput Domain | The domain responsible for the audio device output bridge, wrapping audio output libraries such as miniaudio. | architecture |
| Sequencer ドメイン | Sequencer Domain | A future domain responsible for converting scores/MML into note events. | architecture |
| ISoundSource | ISoundSource | The runtime interface for a sound source plugin. Defines fully abstracted methods such as note_on, note_off, and generate. Method names contain no sound source type identifiers (PSG, FM, PCM, etc.). | architecture |
| ISoundSourceDescriptor | ISoundSourceDescriptor | The metadata interface for a sound source plugin. Provides name, channel_count, capabilities, voice_schema, and sample_rate_for as a self-describing descriptor. | architecture |
| 自己記述型プラグイン | Self-describing Plugin | A plugin design where the plugin provides a descriptor upon loading, enabling the host to handle it generically without knowledge of the sound source's internal specification. | architecture |
| VoiceSchema / voice_schema | VoiceSchema | A list of voice parameter definitions declared by a plugin, including parameter names, types, and value ranges. Enables the host to handle voice parameters from structurally different sound sources (PSG, FM, etc.) in a uniform way. | architecture |
| capabilities | Capabilities | A set of feature flags indicating what a sound source plugin supports (pan, pitch_bend, envelope, etc.). Provided via ISoundSourceDescriptor. | architecture |
| C ABIブリッジ | C ABI Bridge | A bridge layer that defines a plugin's public interface as C-linkage (extern "C") functions, since C++ vtables provide no guaranteed ABI compatibility across shared library boundaries. Enables stable .so/.dll ABI. | architecture |
