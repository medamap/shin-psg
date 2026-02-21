# Milestone 1-1 License Evaluation Report v0.1

**Milestone:** Phase 1 / Milestone 1-1 License Review
**Investigation Date:** 2026-02-21
**Status:** Completed

---

## Investigation Overview

Seven sound emulator libraries and audio output libraries under consideration for use in Shin-PSG implementation
were reviewed from the following perspectives:

- Commercial use eligibility
- Modification and redistribution conditions
- Conditions for static linking
- GPL Contamination risk
- Attribution Notice (copyright notice) requirements

---

## Evaluation Summary

| Library | License | Commercial | Modification | Static Link | GPL Contamination Risk | Conclusion |
|---|---|---|---|---|---|---|
| ymfm | BSD-3-Clause | ✅ | ✅ | ✅ | None | ✅ **Recommended** |
| ayumi | MIT | ✅ | ✅ | ✅ | None | ✅ **Recommended** |
| emu2149 | MIT | ✅ | ✅ | ✅ | None | ✅ Eligible (compare in tech verification) |
| miniaudio | Unlicense / MIT-0 | ✅ | ✅ | ✅ | None | ✅✅ **Top priority** |
| Nuked-OPM | LGPL-2.1 | ✅ | △ | △ | Low | ⚠️ Replaced by ymfm |
| libADLMIDI | LGPL/GPL/MIT mixed | △ | △ | ⚠️ | High | ❌ **Not recommended** |
| libOPNMIDI | LGPL/GPL/MIT mixed | △ | △ | ⚠️ | High | ❌ **Not recommended** |

---

## Individual Evaluations

### ymfm

| Item | Details |
|---|---|
| License | BSD-3-Clause |
| Copyright | Aaron Giles (2021) |
| Commercial use | ✅ Unrestricted |
| Modification | ✅ Free; copyright notice must be retained |
| Redistribution | ✅ Free; license text and copyright notice must be included |
| Static linking | ✅ No restrictions |
| Attribution Notice | Required (copyright notice in source code and binaries) |
| GPL Contamination | None |

**Supported chips:** OPM(YM2151) / OPN(YM2203) / OPN2(YM2612) / OPNA(YM2608) / OPL series

**Assessment:** Can be used without restriction in any use case, including commercial and static linking.
Derived from the MAME project with proven accuracy and track record.
Adopted as the first choice for FM sound emulation.

---

### ayumi

| Item | Details |
|---|---|
| License | MIT |
| Copyright | true-grue |
| Commercial use | ✅ Unrestricted |
| Modification | ✅ Free; copyright notice must be retained |
| Redistribution | ✅ Free; copyright notice must be retained |
| Static linking | ✅ No restrictions |
| Attribution Notice | Required (include copyright notice) |
| GPL Contamination | None |

**Supported chips:** AY-3-8910 / YM2149

**Assessment:** MIT license with extremely few restrictions.
Adopted as the first choice for PSG sound emulation.

---

### emu2149

| Item | Details |
|---|---|
| License | MIT |
| Copyright | digital-sound-antiques |
| Commercial use | ✅ Unrestricted |
| Modification | ✅ Free |
| Redistribution | ✅ Free |
| Static linking | ✅ No restrictions |
| Attribution Notice | Required (copyright notice) |
| GPL Contamination | None |

**Supported chips:** YM2149 / PSG (AY-3-8910 compatible)

**Assessment:** No license issues. Eligible for adoption, same as ayumi under MIT.
The comparison of implementation, accuracy, and performance between ayumi and emu2149 will be decided in Milestone 1-2 (technical verification).
Which one to adopt depends on the results.

---

### miniaudio

| Item | Details |
|---|---|
| License | Public Domain (Unlicense) **or** MIT No Attribution (MIT-0) (user's choice) |
| Copyright | David Reid (2025) |
| Commercial use | ✅✅ Completely free |
| Modification | ✅✅ Completely free |
| Redistribution | ✅✅ Completely free |
| Static linking | ✅✅ No restrictions |
| Attribution Notice | Not required (MIT-0 requires no copyright notice; Unlicense is public domain) |
| GPL Contamination | None |

**Assessment:** The most permissive license with effectively zero restrictions.
Header-only, no dependencies, cross-platform compatible.
Adopted as the top-priority choice for audio output library.
The Unlicense (public domain) is recommended as the license selection.

---

### Nuked-OPM

| Item | Details |
|---|---|
| License | LGPL-2.1 |
| Copyright | nukeykt |
| Commercial use | ✅ Possible (subject to LGPL conditions) |
| Modification | △ Modified versions must be released under LGPL-2.1 |
| Redistribution | △ License notice and disclosure of changes required |
| Static linking | △ See LGPL static linking conditions below |
| GPL Contamination | Low (LGPL itself does not contaminate the application body) |

**Supported chips:** OPM (YM2151)

**LGPL static linking notes:**
When statically linking an LGPL library, disclosure of the application body's source code is not required,
but the user must be able to replace the library and relink (provision of object files or documentation of relinking procedures).
With dynamic linking (DLL / .so), only attribution is required.

**Assessment:** Since ymfm supports OPM (YM2151), Nuked-OPM is **unnecessary**.
By substituting with ymfm, all LGPL constraints are completely avoided. Adoption is skipped.

---

### libADLMIDI

| Item | Details |
|---|---|
| License | LGPL 2.1+ / GPL v2+ / GPL v3+ / MIT **mixed** |
| Copyright | Vitaly Novichkov (Wohlstand) and others |
| Commercial use | △ Source code disclosure required when using GPL components |
| Static linking | ⚠️ When including GPL parts, the entire application is subject to GPL Contamination |
| GPL Contamination risk | **High** |

**Per-component licenses:**

| Component | License | Risk |
|---|---|---|
| Nuked OPL3 emulator | LGPL v2.1+ | Low |
| DosBox OPL3 emulator | GPL v2+ | ⚠️ High |
| Chip interfaces | LGPL v2.1+ | Low |
| File Reader class | MIT | None |
| MIDI Sequencer | MIT | None |
| WOPL reader/writer | MIT | None |
| Other parts | GPL v3+ | ⚠️ High |

**Assessment:** Contains DosBox OPL3 (GPL v2+) and other parts (GPL v3+),
creating a high GPL Contamination risk and imposing significant constraints for commercial and closed-source development.
Since OPL-series FM emulation can be replaced by ymfm, **not adopted**.

---

### libOPNMIDI

| Item | Details |
|---|---|
| License | LGPL 2.1+ / GPL v2+ / MIT **mixed** |
| Copyright | Vitaly Novichkov (Wohlstand) and others |
| Commercial use | △ Source code disclosure required when using GPL components |
| Static linking | ⚠️ When including GPL parts, the entire application is subject to GPL Contamination |
| GPL Contamination risk | **High** |

**Per-component licenses:**

| Component | License | Risk |
|---|---|---|
| Nuked OPN2 emulator | LGPL 2.1+ | Low |
| GENS 2.10 emulator | LGPL 2.1+ | Low |
| MAME YM2612 emulator | GPL v2+ | ⚠️ High |
| Genesis Plus GX emulator | GPL | ⚠️ High |
| MAME YM2608 emulator | GPL | ⚠️ High |
| Neko Project II Kai OPNA | MIT-compatible | None |
| File Reader class / MIDI Sequencer | MIT | None |

**Assessment:** Contains multiple GPL components including MAME emulator cores (YM2612, YM2608).
Since OPN2 (YM2612) / OPNA (YM2608) emulation can be replaced by ymfm, **not adopted**.

---

## Important Notes on Licensing

### Dynamic Linking vs. Static Linking for LGPL

| Link Method | Conditions |
|---|---|
| Dynamic linking (DLL / .so / .dylib) | Only library license attribution is required. Application source code disclosure not required. |
| Static linking (.a / .lib) | User must be able to replace the library and relink (provide object files or document relinking procedure). Application source code disclosure is not required. |

### What is GPL Contamination?

When GPL-licensed code is linked (whether statically or dynamically) and distributed,
the entire application becomes subject to GPL conditions (obligation to disclose and distribute source code).
This is called "GPL Contamination."

For commercial and closed-source development, incorporating GPL code
may fundamentally conflict with the business model, so it should be avoided in principle.

---

## Adoption Decisions

### Adopted Libraries (Confirmed)

| Role | Library | License | Decision Reason |
|---|---|---|---|
| FM sound emulator | **ymfm** | BSD-3-Clause | High accuracy; proven track record; no GPL Contamination |
| PSG sound emulator | **ayumi** | MIT | First choice for PSG; few restrictions |
| Audio output | **miniaudio** | Unlicense / MIT-0 | No restrictions; header-only |

### To Be Compared in Technical Verification (Decided in Milestone 1-2)

| Role | Candidate A | Candidate B | Decision Criteria |
|---|---|---|---|
| PSG sound emulator | ayumi | emu2149 | Accuracy, performance, API usability |

### Not Adopted (with reasons)

| Library | Reason for Non-Adoption |
|---|---|
| libADLMIDI | High GPL Contamination risk; replaceable with ymfm |
| libOPNMIDI | High GPL Contamination risk; replaceable with ymfm |
| Nuked-OPM | ymfm supports OPM; avoided to eliminate LGPL conditions |

---

## Attribution Notice Implementation Policy

Copyright notices for adopted libraries will be recorded in the following locations in the application:

- `NOTICE` file (placed at root; required for distribution)
- Header comments in corresponding files or modules within source code
- "License Information" screen in the GUI version, when implemented

Copyright notices required (confirmed adoptions as of this writing):

```
ymfm - Copyright (c) 2021 Aaron Giles
BSD-3-Clause License

ayumi - Copyright (c) true-grue
MIT License

miniaudio - Copyright (c) 2025 David Reid
Unlicense / MIT No Attribution License
```

---

## Next Steps

Following the conclusions of this report, proceed to the following actions:

1. **Milestone 1-2 (Sound Emulator Technical Verification)**
   - Verify sound output with ymfm + miniaudio (PSG / FM)
   - Comparative verification of ayumi vs. emu2149
2. **Milestone 1-3 (Language and Framework Selection)**
   - Confirm core engine language, GUI layer, and live integration layer
3. **Create NOTICE file** (create in parallel with `src/` before implementation begins)
