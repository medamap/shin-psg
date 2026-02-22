# Shin-PSG Architecture Document v0.2

## Overview

This document is a supplement to the Shin-PSG Specification, and serves as an architecture reference
describing the design, definition, and extension methods for Performers, Conductors, and trait parameters.

While the Feel Engine Technical Document defines "how it works internally,"
this document defines "how to design and extend it."

---

## Abstraction Unique to Electronic Music

### Separation of Concept and Implementation

With acoustic instruments, "concept" and "implementation" cannot be separated.

```
With acoustic instruments:
  Violin vibrato → Oscillating the finger pressing the string (physical constraint)
  Flute vibrato  → Breath control (a different physical constraint)
  The implementation changes with the instrument, and the implementation is bound to the instrument
```

With electronic sound sources, concept and implementation can be completely separated.
This is the foundation of Shin-PSG's design and a unique strength of electronic music.

```
With electronic sound sources:
  The concept of "applying vibrato" is common
  PSG  → Achieved by rapidly rewriting the pitch register
  FM   → Achieved by LFO-based pitch modulation operator
  PCM  → Achieved by pitch shifting
  Implementation methods differ but the concept is identical
```

A Performer only needs to **hold abstract-level traits and emotional parameters**,
and which sound source it plays through is resolved at runtime.
Even if the same Performer plays through PSG or FM,
its character of "liking vibrato" does not change,
but the method of expression is delegated to the sound source implementation.

---

## Class Structure

### Abstract and Concrete Classes for Performance Traits

```
Abstract class: PerformanceTrait
  ├── Vibrato
  │     Parameters: Affinity, Delay, Depth Fluctuation, Rate Sensitivity
  ├── Tremolo
  │     Parameters: Affinity, Emotion Sensitivity, Threshold
  └── Portamento
        Parameters: Affinity, Curve Shape, Undershoot, Overshoot

Sound source-specific concrete classes (implement the abstract class):

  PSG implementation
    Vibrato    → High-speed pitch register rewriting
    Tremolo    → High-speed volume register rewriting
    Portamento → Stepwise pitch register scanning

  FM implementation
    Vibrato    → LFO → Pitch modulation operator
    Tremolo    → LFO → Volume modulation operator
    Portamento → Carrier frequency interpolation

  PCM implementation
    Vibrato    → Pitch shift processing
    Tremolo    → Volume automation
    Portamento → Pitch interpolation
```

Performer definition files are written at the abstract class level,
and assignment to a sound source is resolved at performance time.
This allows the same Performer file to be reused across multiple sound sources.

---

### Performer Inheritance Structure

Performers can be defined by inheriting from another Performer as a base
and overwriting only the differential parameters.

```
Base Performer
  A Performer with all parameters fully defined:
  Emotion Envelope, Emotion Algorithm, and Performance Traits

Derived Performer A
  Inherits from Base Performer
  Override: only Vibrato Affinity changed

Derived Performer B
  Inherits from Base Performer
  Override: only Anger ADSR changed

Derived Performer C
  Inherits further from Derived Performer A
  Override: Portamento Undershoot Amount changed
```

Inheritance chains can span multiple levels.
Undefined parameters use the value from the parent.
Similar in feel to FM voice copying,
this allows efficiently defining "variations of Performers with similar personalities."

**Example uses of inheritance:**

```
"Classical player" (Base Performer)
  → Restrained vibrato, no portamento, stable Emotion Envelope

"Classical player (Passionate)" (Derived)
  → Inherits base, only sets Anger and Joy Sustain higher

"Classical player (Late career)" (Derived)
  → Inherits base, only sets Sadness and Void Release longer
```

---

### Conductor Inheritance Structure

Conductors can also be defined through inheritance, similar to Performers.

```
Base Conductor
  Basic definition of Interpretation, Charisma, and Latency

Derived Conductor A
  Inherits from base, overrides only Charisma

Derived Conductor B
  Inherits from base, overrides only Interpretation
```

---

## Emotion Algorithm as Interface

The topology (Series, Parallel, Feedback) of the Emotion Algorithm
is defined as an interface, and Performers implement it.

```
Interface: EmotionAlgorithm
  Defines: the modulation relationship topology of 9 emotions
  Implementation: assigned per Performer

Implementation A: Series Algorithm
  Anxiety → Anger → Vector output
  (For a Performer expressing anger tinged with urgency)

Implementation B: Parallel Algorithm
  Sadness ──┐
             ├→ Vector addition
  Hope    ──┘
  (For a Performer holding opposing emotions simultaneously)

Implementation C: Feedback Algorithm
  Anger → [Self-Modulation] → Vector output
    ↑_____________________________|
  (For a Performer prone to emotional runaway — high Breakdown risk)
```

Emotion Algorithms can be defined, saved, and shared independently from Performer definitions.
A use case of distributing excellent Emotion Algorithms separately
and incorporating them into Performers is also anticipated.

---

## MML Priority

When a Performer's trait parameters and explicit MML notation conflict,
the following priority order applies:

```
Priority (High)
  Explicit MML notation
    ↓
  Performer trait parameters (varying with Emotion Vector)
    ↓
  Sound source default behavior
Priority (Low)
```

When vibrato is explicitly notated in MML, it takes precedence over the Performer's traits.
When no notation exists in MML, the Performer's trait parameters are automatically applied.
This enables choosing "leave it to the Performer" or "the composer specifies" on a per-part, per-note basis.

---

## Performer Definition File Structure

A Performer definition file has the following structure.
The specific file format is left to implementation,
but the following is defined as the logical structure:

```
Performer Definition File
  ├── Metadata
  │     Name, Profile, Version, Author, Experience
  │
  ├── Base (optional)
  │     Reference to base Performer file
  │
  ├── Emotion Envelope (9 emotions × ADSR)
  │     Can describe only the differential from the base
  │
  ├── Emotion Algorithm
  │     Topology definition or reference to an Algorithm file
  │
  ├── Emotional Sensitivity Parameters
  │     Optimism, Patience, Seriousness, Sensitivity, Limiter, Decay Rate, Tracking
  │
  ├── Performance Trait Parameters
  │     Parameters for each of Vibrato, Tremolo, and Portamento
  │
  └── Conductor/Leader Parameters
        Interpretation, Charisma, Latency, Trust
```

---

## Software Architecture Design Policy

This chapter records the software architecture decisions confirmed prior to the implementation phase.
**Implementation begins only after these designs are finalized across all domains.**

---

### Architecture Style: Onion Architecture (DDD)

Shin-PSG adopts the Onion Architecture pattern under DDD (Domain-Driven Design).

Each domain is composed of four layers. Dependencies always flow inward (from outer to inner).
Inner layers (Domain) have no knowledge of outer layers (Infrastructure).

```
DomainName/
  Presentation/   ← UI display and user input (empty for domains with no UI)
  Application/    ← API entry points and service startup
  Infrastructure/ ← Concrete implementations (adapters for external libraries, etc.)
  Domain/         ← Interfaces, models, enums, business logic (pure abstraction layer)
```

Dependency rules:

```
Presentation  → may reference Application and Domain only
Application   → may reference Domain only
Infrastructure → may reference Domain only
Domain        → references no other layer (completely independent)
```

---

### Domain Decomposition

The following is the provisional decomposition of domains comprising Shin-PSG.
All Domain-layer interfaces and model definitions must be finalized across every domain
before implementation begins.

```
shin-psg/
  SoundSource/     ← Sound source plugin abstraction, loading, and management
  SoundEngine/     ← Tick management, note scheduling, performance orchestration
  SoundOutput/     ← Audio device output bridge
  Sequencer/       ← Score/MML → note event conversion (future domain)
```

Domains do not reference each other directly.
Any cross-domain dependency goes through Domain-layer interfaces.

---

### Sound Source Complete Abstraction Policy

**Method names and interfaces must never include sound source type names
such as PSG, FM, PCM, or BEEP.**

Sound source type is an implementation detail internal to the plugin.
The host (SoundEngine, etc.) treats a sound source solely as
"something that can produce and stop sounds on channels."

```
// Wrong: sound source type leaks into the interface
note_on_psg(ch, pitch, volume)
note_on_fm(ch, pitch, voice_id)

// Correct: fully abstracted interface
note_on(ch, pitch, velocity)
note_off(ch)
generate(buffer, frames)
```

This design allows swapping from PSG to FM to PCM with zero changes to host code.

---

### Sound Source Plugin Architecture

Sound sources are implemented as **self-describing plugins**.
Loading a plugin makes all information required by that sound source available
through a descriptor.

```
ISoundSourceDescriptor {
    name()              → Sound source name (e.g. "YM2151 OPM")
    channel_count()     → Number of simultaneous channels (e.g. 8)
    capabilities()      → Supported feature flags (pan / pitch_bend / envelope, etc.)
    voice_schema()      → List of voice parameter definitions (name, type, value range)
    sample_rate_for(clock) → Internal sample rate for a given clock
}
```

Through `voice_schema()`, the host can handle voice definition I/O, configuration file management,
and UI generation generically, without knowing the meaning of individual voice parameters.
PSG voice parameters and FM 4-operator parameters have entirely different structures,
but the host need not be aware of this distinction.

Runtime interface provided by a plugin:

```
ISoundSource {
    descriptor()               → ISoundSourceDescriptor
    initialize(clock, sr)      → void
    load_voice(ch, IVoiceParam) → void
    note_on(ch, pitch, vel)    → void
    note_off(ch)               → void
    set_pan(ch, pan)           → void    ← valid only if capabilities includes pan
    generate(buffer, frames)   → void
}
```

**Relationship between plugins and the SoundSource domain:**

Each sound source implementation (ayumi, emu2149, OPN, OPM, etc.) is a separate
`ISoundSource` implementation residing in the Infrastructure layer of the SoundSource domain.
New electronic sound sources can be added by simply adding a plugin;
no changes to existing domain code are required.

---

### .so / .dll Plugins and the C ABI Bridge

To distribute plugins as independent shared libraries (`.so` / `.dll`),
**a thin C ABI bridge layer is placed just outside the Infrastructure layer**.

Because C++ vtables provide no guaranteed ABI compatibility across shared library boundaries,
only C-linkage functions are exported at the shared library boundary.

```c
// C ABI functions exported by a plugin .so / .dll
extern "C" {
    // Plugin creation and destruction
    void* spsg_plugin_create(uint32_t clock, uint32_t sample_rate);
    void  spsg_plugin_destroy(void* handle);

    // Descriptor access
    const char* spsg_get_name(void* handle);
    uint32_t    spsg_get_channel_count(void* handle);
    uint32_t    spsg_get_capabilities(void* handle);
    uint32_t    spsg_get_sample_rate(void* handle);

    // Runtime operations
    void  spsg_load_voice(void* handle, uint32_t ch, const void* voice_data, uint32_t size);
    void  spsg_note_on(void* handle, uint32_t ch, uint8_t pitch, uint8_t velocity);
    void  spsg_note_off(void* handle, uint32_t ch);
    void  spsg_set_pan(void* handle, uint32_t ch, float pan);
    void  spsg_generate(void* handle, int16_t* buffer, uint32_t frames);
}
```

The Application layer holds a C++ adapter that wraps these C functions and loads plugins
dynamically via `dlopen` / `LoadLibrary`. This achieves:

- Plugin internals can be freely written in any C++ style
- Shared library boundary ABI remains stable
- Swapping to a different plugin with the same function signatures is straightforward
- Any electronic sound source can be linked in as a plugin

---

### PSG Emulator Adoption Policy

Based on the results of Milestone 1-1 (license evaluation) and Milestone 1-2 (technical verification),
**both ayumi and emu2149 are adopted** as PSG emulators.

| Library | Use case | Rationale |
|---|---|---|
| ayumi | Performer channels requiring stereo panning | Per-channel stereo pan API and DC filter directly satisfy spec requirements |
| emu2149 | Lightweight processing where panning is not required | Simple and fast; suited for batch processing and preview generation |

Both libraries are MIT-licensed with no issues. Which library to use is an
Infrastructure-layer decision; the host makes no distinction (complete abstraction).

The FM emulator is ymfm (BSD-3-Clause). OPN (YM2203) and OPM (YM2151) support
were both confirmed in Milestone 1-2.

---

### Prerequisites for Moving to the Implementation Phase

Implementation begins only when all of the following are complete:

**1. Domain layer design complete for all domains**
- All interfaces, models, enums, and business logic defined
- Cross-domain dependencies and interface contracts confirmed

**2. Plugin specification finalized**
- Complete definition of `ISoundSourceDescriptor`
- Complete definition of the `ISoundSource` C ABI
- `VoiceParam` schema specification finalized
- First plugin implementation target decided

**3. Cross-domain integration design complete**
- SoundEngine ↔ SoundSource interaction flow
- SoundEngine ↔ SoundOutput interaction flow
- Tick management and note scheduling processing flow

Implementation follows these designs; the goal is for design to be complete
before a single line of code is written.

---

## Document Relationships

```
shin_psg_spec.md
  "What it can do"
  Concepts, performance modes, parameter list
       ↓ references
shin_psg_feel_engine.md
  "How it works internally"
  Stage 1–6 processing flow
  Emotion Envelope, Algorithm, Vector Conversion, Performance Traits
       ↓ references
shin_psg_architecture.md (this document)
  "How to design and extend it"
  Abstraction, inheritance, interfaces, sound source plugin architecture,
  Onion Architecture (DDD), domain decomposition, file structure
```

---

## Revision History

| Version | Notes |
|---|---|
| v0.2 | Added Software Architecture Design Policy section: Onion Architecture (DDD), domain decomposition, sound source complete abstraction, self-describing plugin architecture, C ABI Bridge, PSG emulator adoption policy (both ayumi and emu2149), and prerequisites for the implementation phase. |
| v0.1 | Initial version. Defined abstraction, inheritance, interfaces, sound source-specific implementation mapping, MML priority, and Performer definition file structure. |
