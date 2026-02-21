# Shin-PSG Implementation Roadmap v0.1

## Overall Structure

```
[Phase 1] Foundation Research and Technology Selection
    ↓
[Phase 2] Sound Engine Layer
    ↓
[Phase 3] Shin Core Engine (Minimal Configuration)
    ↓
[Phase 4] MML and PML Language Design and Implementation
    ↓
[Phase 5] Shin Academy (Performer and Conductor Training)
    ↓
[Phase 6] Staging and Audience Support
    ↓
[Phase 7] Social Live Streaming Integration
    ↓
[Phase 8] Social Platform
```

---

## Platform and Language Policy

### Supported Platforms

Windows, macOS, and Linux are supported equally across all three platforms.

### Recommended Languages and Stack (Candidates)

| Layer | Candidate | Reason |
|---|---|---|
| Core engine | C++ (C++17 or later) | Sound emulators are predominantly C++; advantageous for real-time processing |
| Scripting and bindings | Rust or Python bindings | Ease of cross-platform builds |
| GUI and tools | Qt6 or Tauri (Rust + WebView) | Cross-platform GUI |
| Live integration layer | Python or Node.js | Rich libraries for YouTube API / OBS WebSocket integration |
| Build system | CMake | De facto standard for cross-platform C++ |

Note: Languages and libraries will be finalized after technical verification in Phase 1.

---

## Sound Engine Candidate Libraries (Researched)

### FM Sound Sources

| Library | Supported Chips | License | Notes |
|---|---|---|---|
| **ymfm** | OPM/OPN/OPL series | BSD-3-Clause | Derived from MAME project; C++14; high accuracy |
| **libADLMIDI** | OPL3 (YMF262) | LGPL2.1+/MIT mixed | MIDI integration; virtual multi-chip support |
| **libOPNMIDI** | OPN2 (YM2612) / OPNA (YM2608) | LGPL2.1+/MIT mixed | Sister library of libADLMIDI |
| **Nuked-OPM** | OPM (YM2151) | LGPL2.1+ | Supports X68000 series; high accuracy |

### PSG Sound Sources

| Library | Supported Chips | License | Notes |
|---|---|---|---|
| **ayumi** | AY-3-8910 / YM2149 | MIT | Standard PSG for MSX and ZX Spectrum |
| **emu2149** | YM2149 / PSG | MIT | Lightweight; easy to integrate |
| **MAME internal core** | SN76489, etc. | BSD/GPL mixed | Requires license scrutiny |

### Audio Output

| Library | License | Notes |
|---|---|---|
| **SDL2 Audio** | zlib | Cross-platform; proven track record |
| **PortAudio** | MIT | Low latency; professional grade |
| **miniaudio** | MIT/Unlicense | Header-only; no dependencies; lightest weight |

**First choices: ymfm (FM) + ayumi (PSG) + miniaudio (output)**
Reason: All use MIT or BSD-equivalent licenses; strong affinity with C++.

---

## Phase 1: Foundation Research and Technology Selection

**Goal:** Confirm technology stack and verify minimal operation

### Milestone 1-1: License Review
- Review commercial use and modification conditions for ymfm / libADLMIDI / libOPNMIDI
- Confirm licenses for ayumi / emu2149
- Investigate possibility of isolating GPL-mixed components
- Deliverable: License evaluation report

### Milestone 1-2: Sound Emulator Technical Verification
- Verify PSG / FM / PCM sound output with ymfm + miniaudio
- Confirm builds and operation on Windows, macOS, and Linux
- Measure latency and CPU load
- Deliverable: Technical verification report and sample code

### Milestone 1-3: Language and Framework Selection
- Confirm core engine language (C++ or Rust)
- Select GUI layer (Qt6 or Tauri)
- Select live integration layer (Python or Node.js)
- Confirm build system (CMake)
- Deliverable: Technology selection document

### Milestone 1-4: Project Structure Design
- Repository structure and module separation policy
- CI/CD environment design (GitHub Actions, etc.)
- Deliverable: Project structure document

---

## Phase 2: Sound Engine Layer

**Goal:** Implement sound emulator integration layer for PSG / FM / PCM

### Milestone 2-1: Sound Driver Abstraction Layer Design
- Design a common interface independent of sound source type
- Define abstract classes for Performance Traits (Vibrato, Tremolo, Portamento)
- Conform to the abstraction design in the architecture document

### Milestone 2-2: PSG Driver Implementation
- Integrate AY-3-8910 / YM2149 emulator
- Note-on/off, volume, and pitch control
- Vibrato: Implemented via high-speed pitch register rewriting
- Tremolo: Implemented via high-speed volume register rewriting
- Portamento: Implemented via stepwise pitch register scanning

### Milestone 2-3: FM Sound Driver Implementation
- Integrate OPM (YM2151) / OPN (YM2203) / OPNA (YM2608)
- Operator, algorithm, and envelope control
- Vibrato: Via LFO pitch modulation
- Tremolo: Via LFO volume modulation
- Portamento: Carrier frequency interpolation

### Milestone 2-4: PCM Driver Implementation
- Sample playback, pitch shifting, volume automation
- Implement Vibrato, Tremolo, and Portamento

### Milestone 2-5: Performance Status Retrieval API Design
- Retrieve current state of each channel (note-on, pitch, volume)
- Interface for emotion log and Feel log snapshot
- Real-time performance monitoring callback design

---

## Phase 3: Shin Core Engine (Minimal Configuration)

**Goal:** Minimal implementation capable of normal playback without Conductor or Performers

### Milestone 3-1: Shin Engine Basic Design
- Define the interface between Shin Engine and sound drivers
- Design performance data (binary) format
- Design Feel Track data structure

### Milestone 3-2: Normal Performance Implementation (No Performers or Conductor)
- Load and play Note Tracks
- Achieve Deterministic Performance
- Simultaneous control of multiple sound sources

### Milestone 3-3: Feel Engine Basic Implementation
- Implement Stages 1–4 (Feel Track Input through Valence-Arousal Vector Conversion)
- Implement Emotion Envelope (ADSR)
- Implement Emotion Algorithm (Series, Parallel, Feedback)

### Milestone 3-4: Basic Performer Implementation
- Define and load Performer parameters
- Reflect Feel Vector in performance expression:
  - Note-on timing variation
  - Attack and Release changes
  - Dynamic changes to Vibrato, Tremolo, and Portamento traits
- Implement Breakdown State

### Milestone 3-5: Performance Log and Snapshot Implementation
- Record emotion log and Feel log after performance
- Write to snapshot file
- Reproduce performance from snapshot (Deterministic playback)
- Deliverable: "Shin Engine Minimal Working Version"

---

## Phase 4: MML and PML Language Design and Implementation

**Goal:** Formalize the notation for musical data (MML) and Performer/Conductor definitions (PML), and implement the processing systems

### Milestone 4-1: MML Notation Design
- Survey existing MML formats (PPMCK / FMP / PMD / MUCOM88, etc.)
- Design Shin-PSG extended MML notation:
  - Feel Track notation method
  - Override notation for Performance Traits (explicit MML notation for Vibrato, Tremolo, Portamento)
  - Sound source specification and Performer assignment notation
- Deliverable: MML notation specification

### Milestone 4-2: PML Notation Design (Personal Macro Language)
- Design notation for Performer and Conductor parameter description:
  - Emotion Envelope (9 emotions × ADSR)
  - Emotion Algorithm topology notation
  - Performance Trait parameters
  - Inheritance and differential notation format
- Notation for profile, name, and metadata
- Deliverable: PML notation specification

### Milestone 4-3: Shin Compiler Implementation
- Compile MML → Shin Engine binary
- Convert Feel Track envelope curves
- Error checking and debug output
- Deliverable: Shin Compiler v1.0

### Milestone 4-4: Shin Academy Compiler Implementation
- Compile PML → Performer/Conductor binary
- Resolve inheritance chains
- Parameter validation and warning output
- Deliverable: PML Compiler v1.0

---

## Phase 5: Shin Academy (Performer and Conductor Training)

**Goal:** Implement Performer and Conductor training, experience, and export features

### Milestone 5-1: Experience System Design
- Design post-performance experience calculation logic
  - Reference Feel Vector variation, presence/absence of Breakdown State, etc.
- Design experience file format
- Design parameter variation through growth

### Milestone 5-2: Local Training Implementation
- Record and load experience
- Growth processing per performance
- Practice and Staging mode switching

### Milestone 5-3: Export and Import Implementation
- Export Performer and Conductor files
- Import and validate files from others

### Milestone 5-4: Shin Academy GUI (Local Version)
- Performer and Conductor list and detail views
- Parameter editing UI
- Performance history and growth log visualization
- Deliverable: Shin Academy Local Version

---

## Phase 6: Staging and Audience Support

**Goal:** Implement performance-event and live mode, and realize emotional influence based on audience count

### About Staging

Shin-PSG performance environments are distinguished into the following 3 modes:

| Mode | Description |
|---|---|
| Practice | Practice and test performances. No audience. Emotions enter in a restrained manner. |
| Staging | Concerts and live performances. The emotional intensity of Performers changes with audience count. |
| Recording | Studio recording. No audience; emphasis on snapshot recording. |

### Milestone 6-1: Staging Mode Design
- Switching logic for Practice, Staging, and Recording
- Design emotional parameter amplification coefficient for Staging mode
  - Coefficient variation based on audience count
  - Expressing "the bigger the stage, the more emotional energy flows in"

### Milestone 6-2: Audience Feedback Design
- Receive and normalize audience count
- Design Feel Feedback (audience reactions influencing the Feel Track)
- Map reaction types to emotional influence:
  ```
  Normal comments         → Small influence on Hope and Fun
  Super Chat              → Strong influence on Joy and Greed (proportional to amount)
  Mass comment influx     → Influences Anxiety or Fun (depends on Performer)
  No reaction (silence)   → Influences Void and Anxiety
  ```

### Milestone 6-3: Local Staging Implementation
- Verify operation with dummy audience count
- Confirm real-time Feel Feedback reflection

---

## Phase 7: Social Live Streaming Integration

**Goal:** Realize integration between OBS, YouTube, and other streaming platforms with Shin Engine

### Required External Software and Services

| Software/Service | Purpose | Notes |
|---|---|---|
| **OBS Studio 28+** | Video/audio capture and streaming | obs-websocket built-in (port 4455) |
| **YouTube Live Streaming API** | Retrieve comments and Super Chats | Requires API key from Google Cloud Console |
| **Streamlabs Socket API** | Unified event retrieval from YouTube/Twitch, etc. | Can centrally manage multiple platforms |
| **Social Stream Ninja** | Chat aggregation across multiple platforms | OBS overlay support; free |

### Milestone 7-1: OBS Integration Implementation
- Connect to obs-websocket (WebSocket port 4455)
- Link performance state to OBS scene switching triggers
  - Production integration such as Breakdown State → scene change
- Route audio output to OBS (consider Virtual Audio Cable, etc.)

### Milestone 7-2: YouTube Live API Integration Implementation
- Implement authentication for YouTube Data API v3 / Live Streaming API
- Poll retrieval of liveChatMessages
- Retrieve superChatEvents and map amount → emotional intensity
- Retrieve viewer count and reflect in audience count

### Milestone 7-3: Multi-Platform Streaming Support
- Research support for Twitch, Niconico Live, TikTok Live, etc.
- Consider unified retrieval via Streamlabs Socket API
- Convert comments and reactions from each platform → Feel Feedback

### Milestone 7-4: Feel Feedback Real-Time Implementation
- Send comment and Super Chat events to Shin Engine in real time
- Convert event type and intensity → emotional feedback values
- Visualize Performer's real-time emotion log (output to streaming overlay)

### Milestone 7-5: Integration Test and Streaming Rehearsal
- Verify full-stack operation on Windows / macOS / Linux
- Streaming test using actual YouTube Live
- Verify latency and stability

---

## Phase 8: Social Platform

**Goal:** Build a platform for sharing and training Performers and Conductors

### Design Policy

The social platform is built as a layer independent of the Shin-PSG core specification.
The following two platform formats are under consideration:

| Format | Description | Merits | Demerits |
|---|---|---|---|
| Centralized | Manages Performers and Conductors on a single server | Simple implementation; unified user experience | Risk of server loss |
| Federated (Mastodon style) | Each user holds a server instance with mutual connections | Distributed; high fault tolerance | Complex implementation |

### Milestone 8-1: Platform Format Confirmation
- Organize tradeoffs between centralized and federated formats
- Consider policy of starting with centralized format at prototype scale

### Milestone 8-2: Performer and Conductor Sharing Features
- Upload, download, and search
- Profile and achievement display
- Version management (snapshots)

### Milestone 8-3: Community Features
- Comments and ratings on Performers and Conductors
- Publishing and sharing performance logs
- Rankings and discovery of popular Performers

---

## Overall Roadmap Schedule (Estimate)

```
Phase 1  Foundation Research and Technology Selection     1–2 months
Phase 2  Sound Engine Layer                               2–3 months
Phase 3  Shin Core Engine Minimal Configuration           3–4 months
Phase 4  MML and PML Language Design and Implementation   2–3 months
Phase 5  Shin Academy                                     2–3 months
Phase 6  Staging Support                                  1–2 months
Phase 7  Social Live Streaming Integration                2–3 months
Phase 8  Social Platform                                  3–6+ months
```

Phases 1–3 are the highest priority. The goal is to release a minimal working version
capable of "writing in MML and playing back" as early as possible.
Phases 6 and beyond can proceed with design in parallel after Phase 3 is complete.

---

## Revision History

| Version | Notes |
|---|---|
| v0.1 | Initial version. Defined structure for Phases 1–8, milestones, technology stack candidates, and external software survey. |
