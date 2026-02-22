# Milestone 1-2 Sound Emulator Technical Verification Report

**Milestone:** Phase 1 / Milestone 1-2 Sound Emulator Technical Verification
**Date:** 2026-02-22
**Status:** Complete

---

## Overview

This report documents the build, runtime, and CPU load verification of the following libraries
under consideration for the Shin-PSG sound emulation layer, tested on
Linux (Ubuntu 22.04 / g++ 11.4).

| Target | Library | Version |
|---|---|---|
| PSG sound source (Candidate A) | ayumi | latest (git) |
| PSG sound source (Candidate B) | emu2149 | latest (git) |
| FM sound source | ymfm | latest (git) |
| Audio output | miniaudio | latest (header only) |

Verification source code and generated WAV files are located under
`research/milestone-1-2-sound-verification/`.

---

## Test Content

### PSG Verification (ayumi / emu2149)

- C major scale (C4–C5), 3-channel simultaneous output (melody + harmony + perfect 5th)
- 0.4 seconds per note, 3.2 seconds total
- 44100 Hz / 16-bit / stereo (ayumi), 44100 Hz / 16-bit / mono (emu2149)
- CPU time measured for WAV file write-out

### FM Verification (ymfm / YM2203 OPN)

- YM2203 (OPN): piano-style voice using Algorithm 4 (2-operator series FM)
- C major scale (C4–C5) with Key ON/OFF, 4.4 seconds total
- WAV output at the chip's internal rate obtained via `chip.sample_rate()` API
  (FIDELITY_MIN: 166400 Hz)
- CPU time measured for WAV file write-out

**Note:** The initial OPN test produced only a muffled low-frequency noise due to
an incorrect internal sample rate calculation. This has been fixed. See the
Bug Fix Record section for details.

### FM Verification (ymfm / YM2151 OPM)

- YM2151 (OPM): targets Sharp X1 + FM sound board (CZ-8BS1, etc.),
  X1turboZ (built-in), and X68000 series
- Algorithm 5 brass-style 4-operator voice
- C major scale (C4–C5) with Key ON/OFF, 4.4 seconds total
- OPM clock: 4.0 MHz, internal rate: 62500 Hz
- Implemented the proper KC register note encoding (C=14, C#=0, with skip gaps at 3, 7, 11, 15)

---

## Measurements

### Environment

| Item | Value |
|---|---|
| OS | Ubuntu 22.04 LTS (Linux VM) |
| Compiler | g++ 11.4.0 |
| Optimization | -O2 |

### PSG: ayumi

| Item | Value |
|---|---|
| Samples generated | 141,120 |
| Audio duration | 3.20 s |
| CPU time | 0.0442 s |
| CPU load ratio | **0.0138** (real-time limit = 1.0) |
| Sample rate | 44100 Hz |
| Result | ✅ Comfortable margin |

### PSG: emu2149

| Item | Value |
|---|---|
| Samples generated | 141,120 |
| Audio duration | 3.20 s |
| CPU time | 0.0212 s |
| CPU load ratio | **0.0066** (real-time limit = 1.0) |
| Sample rate | 44100 Hz |
| Result | ✅ Comfortable margin |

### FM: ymfm (YM2203 OPN)

| Item | Value |
|---|---|
| Samples generated | 732,160 |
| Audio duration | 4.40 s |
| CPU time | 0.1294 s |
| CPU load ratio | **0.0294** (real-time limit = 1.0) |
| Sample rate | 166400 Hz (FIDELITY_MIN internal rate) |
| Result | ✅ Comfortable margin |

### FM: ymfm (YM2151 OPM)

| Item | Value |
|---|---|
| Samples generated | 275,000 |
| Audio duration | 4.40 s |
| CPU time | 0.0714 s |
| CPU load ratio | **0.0162** (real-time limit = 1.0) |
| Sample rate | 62500 Hz (internal rate) |
| Result | ✅ Comfortable margin |

---

## Sound Quality Evaluation (Listening)

Subjective listening evaluation results for the WAV files generated under
`research/milestone-1-2-sound-verification/`.

### PSG: ayumi vs emu2149

| Item | ayumi | emu2149 |
|---|---|---|
| Tonal impression | Slightly grainy texture, strong PSG character | Cleaner, more straightforward PSG character |
| Output | Stereo (built-in FIR filter + DC filter) | Mono (simple implementation) |
| API | C API with stereo pan support | C API with direct int16 output |
| CPU load ratio | 0.0138 | 0.0066 (approx. 2× faster) |

Both libraries deliver recognizable AY-3-8910 tonal character; the sonic difference is subtle.
**Both are adopted** for use in different contexts (see Adoption Decision below).

### FM: OPN / OPM

- **OPN (YM2203):** After the bug fix, proper FM sound confirmed. Characteristic PC-88 chiptune quality.
- **OPM (YM2151):** X1/X1turboZ/X68000 OPM sound reproduced accurately.
  The 4-operator brass voice sounded as intended.

---

## Build Verification

### Linux (Ubuntu 22.04 / g++ 11.4)

| Library | Build | Run | WAV output |
|---|---|---|---|
| ayumi | ✅ | ✅ | ✅ output_ayumi.wav (552 KB) |
| emu2149 | ✅ | ✅ | ✅ output_emu2149.wav (276 KB) |
| ymfm (OPN) | ✅ | ✅ | ✅ output_ymfm_opn.wav (2.8 MB / 166400 Hz) |
| ymfm (OPM) | ✅ | ✅ | ✅ output_ymfm_opm.wav (1.1 MB / 62500 Hz) |
| miniaudio | ✅ (header-only, acquisition only) | — | — |

### Windows / macOS

Milestone 1-2 covers Linux verification only.
Windows and macOS verification should be performed separately in native environments.

---

## Bug Fix Record

### OPN Internal Sample Rate Miscalculation

**Symptom:** Generated WAV produced only muffled low-frequency noise.

**Root cause:**
```cpp
// Wrong: OPN_CLOCK / 144 = 27733 Hz incorrectly assumed as internal rate
const double ymfm_rate = (double)OPN_CLOCK / 144.0;
```

`ym2203::sample_rate()` with `OPN_FIDELITY_MIN` returns `input_clock / 24 = 166400 Hz`.
The miscalculated value of 27733 Hz was approximately 1/6 of the correct internal rate,
causing the pitch to drop drastically and the output to become noise-like.

**Fix:**
```cpp
// Correct: obtain the true internal rate via the chip.sample_rate() API
opn.chip.set_fidelity(ymfm::OPN_FIDELITY_MIN);
uint32_t SAMPLE_RATE = opn.chip.sample_rate(OPN_CLOCK); // = 166400 Hz
```

---

## CMake Integration

A CMake build definition is provided at
`research/milestone-1-2-sound-verification/CMakeLists.txt`.
On systems with cmake installed:

```bash
cd research/milestone-1-2-sound-verification
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Direct g++ compilation was used for verification in this VM environment
(cmake not installed). Use the CMake build path when integrating into `src/`.

---

## Adoption Decision

### PSG Sound Emulator: Both ayumi and emu2149 Adopted

| Library | Use case | Rationale |
|---|---|---|
| **ayumi** | Performer channels requiring stereo panning | Per-channel stereo pan API and built-in DC filter directly satisfy the Shin-PSG spec requirement |
| **emu2149** | Lightweight processing where panning is not required | Simple, fast, suited for batch processing and preview generation |

Which library to use is an Infrastructure-layer decision; the host (Application layer) makes
no distinction between them (complete abstraction). Both libraries are MIT-licensed.
Hardware comparison testing was not performed as no physical hardware is available.

### Other Confirmed Libraries

| Role | Library | Rationale |
|---|---|---|
| FM sound emulator | **ymfm** | Reaffirms Milestone 1-1 decision. OPN and OPM support verified. |
| Audio output | **miniaudio** | Header-only, zero external dependencies confirmed. Formal integration into the output layer scheduled for Milestone 2-1. |

---

## Next Steps

1. **Milestone 1-3: Language and Framework Selection**
   - Confirm core engine language, GUI layer, and live integration layer
2. **Architecture Design Phase (pre-implementation)**
   - Finalize domain decomposition under Onion Architecture (DDD)
   - Design all Domain layers (interfaces and model definitions) across all domains
   - Define ISoundSource / ISoundSourceDescriptor / C ABI Bridge specification
3. **Milestone 2-1 (future): Sound Driver Abstraction Layer Implementation**
   - Integrate adopted libraries as SoundSource Domain Infrastructure implementations

---

## Revision History

| Version | Notes |
|---|---|
| v0.2 | Added OPN bug fix record and OPM (YM2151) verification results. Updated PSG adoption decision to "both adopted." Added listening evaluation results. Updated next steps to reflect architecture design phase. |
| v0.1 | Initial version. Documented ayumi / emu2149 / ymfm build, execution, and CPU load measurement results on Linux. |
