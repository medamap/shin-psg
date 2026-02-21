# Shin-PSG Retro Platform Development Guide v0.1

This document defines the design policy, directory structure, build procedures, and notes
for implementing Shin-PSG on Z80-based retro platforms (Sharp X1, MSX, PC-88, etc.).

---

## Revision History

| Version | Notes |
|---|---|
| v0.1 | Initial version. Defined Z80 shared core policy, machine-specific separation, interrupt differences, Feel Engine porting strategy, and build procedures. |

---

## Target Platforms

| Machine | CPU | Interrupt Source | Timer Resolution | Notes |
|---|---|---|---|---|
| Sharp X1 | Z80A (4MHz) | CTC (Counter Timer Circuit) | Freely configurable | High flexibility for tempo management |
| MSX | Z80A (3.58MHz) | VDP (V-SYNC interrupt) | Fixed 1/60 sec (NTSC) | 1/50 sec in PAL environments |
| PC-88 | μPD780C (Z80-compatible, 4/8MHz) | Pending detailed investigation | Pending investigation | |

---

## Design Policy

### Concept of the Z80 Shared Core

Since X1, MSX, and PC-88 all use the Z80 architecture,
the sound driver algorithm body can be implemented as shared source code.

The separation principle is identical to the relationship between `src/core/` and `src/platform/` in modern platforms.

```
retro/z80/core/    ← Z80 shared code (algorithm body)
  driver/          ← Sound driver algorithms
  feel/            ← Simplified Feel Engine implementation
  mml/             ← MML binary decode processing

retro/z80/x1/platform/   ← X1-specific (interrupts, registers)
retro/z80/msx/platform/  ← MSX-specific (interrupts, registers)
retro/z80/pc88/platform/ ← PC-88-specific
```

Shared code is referenced from each machine's entry point via AILZ80ASM's `INCLUDE` directive.
Core code contains no direct hardware register access;
all hardware-dependent processing is separated into the `platform/` directories.

---

## Interrupts and Tempo Management

### The Core Problem

Z80 music drivers operate interrupt-driven.
Each time an interrupt fires, a "tick" is counted by 1,
and the wait counters per channel are decremented to process note events.

Since tempo depends on the tick rate, when tick rates differ between machines,
the same MML binary plays back at different tempos on different machines.

### Machine-Specific Interrupt Sources and Characteristics

**Sharp X1 — CTC Interrupt**

```
Using the X1's CTC (Z80 CTC: Counter/Timer Circuit),
interrupts can be fired at any arbitrary period.

Example: When CTC is set to 300Hz
  Tempo 120 BPM = 2 beats/sec = 1 beat = 150 ticks
  Minimum time unit = 1/300 sec ≈ 3.3ms
  Sufficiently fine time resolution is obtained
```

**MSX — VDP V-SYNC Interrupt**

```
Piggybacks on the interrupt fired by the MSX's VDP (Video Display Processor) at V-SYNC.
Frequency is fixed at 60Hz in NTSC environments (50Hz in PAL environments).

Example: At 60Hz
  Tempo 120 BPM = 2 beats/sec = 1 beat = 30 ticks
  Minimum time unit = 1/60 sec ≈ 16.7ms
  Tempo resolution is lower than X1, but practically sufficient as a music driver
```

### Solution: Tick Abstraction

The shared code is designed so that it does not need to know the tick rate.
Machine-specific initialization code provides the tick rate as a constant,
and the MML binary compiler converts wait values to match each machine's tick rate at compile time.

```asm
; Shared code concept (pseudocode)
; Processing that does not depend on tick rate
TICK_HANDLER:
    ; Decrement wait counters for all channels
    ; Process note events for channels where wait = 0
    ; No need to know the tick rate

; Constant provided by the machine side (X1 example)
TICKS_PER_SEC   EQU 300     ; Rate set by CTC

; MSX example
TICKS_PER_SEC   EQU 60      ; VDP V-SYNC fixed
```

The MML compiler generates a binary with wait values embedded to match the tick rate,
based on the target specification (`--target x1` / `--target msx`, etc.).

---

## Build Tool: AILZ80ASM

### Overview

- Repository: https://github.com/AILight/AILZ80ASM
- Language: C# (.NET 8)
- Executable: Self-contained binary (Windows/macOS/Linux compatible; .NET not required)
- License: MIT
- Configuration file: `AILZ80ASM.json` (input/output and option settings)

### Installation

Download the binary for the corresponding platform from the GitHub Releases page
and place it in a directory on your PATH.

### Basic Usage

```bash
# Basic compile
AILZ80ASM input.asm

# With options
AILZ80ASM --output-format MZT --output main.mzt main.asm

# Using configuration file (place AILZ80ASM.json in project root)
AILZ80ASM main.asm
```

### AILZ80ASM.json Placement

Configuration files are placed in each machine's directory.
They are not shared because output formats differ per machine.

```
retro/z80/x1/AILZ80ASM.json   ← MZT/CMT output settings
retro/z80/msx/AILZ80ASM.json  ← ROM/CAS output settings
retro/z80/pc88/AILZ80ASM.json ← Pending investigation
```

### Invocation from CMake (Optional)

To build retro targets together with the modern build,
they can be invoked from the root CMakeLists.txt as follows (not required):

```cmake
add_custom_target(retro-x1
    COMMAND make -C ${CMAKE_SOURCE_DIR}/retro/z80/x1
    COMMENT "Building Sharp X1 target with AILZ80ASM"
)

add_custom_target(retro-msx
    COMMAND make -C ${CMAKE_SOURCE_DIR}/retro/z80/msx
    COMMENT "Building MSX target with AILZ80ASM"
)
```

For normal retro development, run `make` in each machine's directory.

---

## Z80 Porting Strategy for the Feel Engine

### Basic Policy

The Feel Engine for Shin-PSG (Stages 1–6) will be designed for Z80 porting
after the full-set implementation on modern platforms is completed in Phases 2–3.

"First implement the full set, measure the actual load on Z80, then trim down" is the principle.
Take care not to trim so much that the interesting qualities of the Feel Engine are lost.

### Constraints on Z80

| Constraint | Details |
|---|---|
| No FPU | Floating-point arithmetic for θ and strength must be replaced with fixed-point (8-bit/16-bit) |
| Memory constraints | Z80 address space is 64KB (bank switching may be available on some machines) |
| Processing speed | Multiply-accumulate operations are implemented in software and thus slow. Lookup tables are effective. |

### Simplification Candidates (to be decided after measurement)

The following are future optimization candidates and are not finalized at this point.

- Reducing bit width of emotional parameters (float → 8-bit fixed-point)
- Converting `atan2` to a lookup table
- Approximating the emotion envelope's ADSR with a simpler curve
- Reducing update frequency to per-tick (once every few ticks instead of every tick)
- Consolidating 9 emotions into a smaller subset (subset emotion definition)

### Z80 Feel Engine Implementation Order

1. Wait for the full-set version (modern platform) to be completed
2. Implement minimal emotional processing on Z80 (fixed-point ADSR + simplified vector conversion)
3. Measure load on actual hardware or emulator
4. Simplify incrementally until within acceptable range

---

## Machine-Specific Notes

### Sharp X1

- Timer resolution can be freely chosen via CTC settings
- Memory map and I/O port addresses need to be confirmed
- PSG is AY-3-8910 compatible (port addresses require confirmation)

### MSX

- Uses the VDP V-SYNC interrupt (INT)
- Note that PAL environments run at 50Hz (consider NTSC/PAL detection logic)
- PSG is AY-3-8910 (accessed via I/O port)
- Structure differs depending on whether it runs on MSX-BASIC / MSX-DOS or is implemented as a ROM cartridge

### PC-88

- Details are under investigation. This document will be updated as information becomes available.

---

## Development Flow

```
[1] Implement full set on modern platform (Phases 2–3)
    ↓
[2] Design study for Z80 porting (after Phase 3 completion)
    ↓
[3] Implement shared Z80 code in retro/z80/core/ (algorithms only)
    ↓
[4] Implement machine-specific parts in retro/z80/x1/ or retro/z80/msx/
    ↓
[5] Verify operation on emulator
    ↓
[6] Verify operation on actual hardware
```
