# Shin-PSG Architecture Document v0.1

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
  Abstraction, inheritance, interfaces, sound source-specific implementations, file structure
```

---

## Revision History

| Version | Notes |
|---|---|
| v0.1 | Initial version. Defined abstraction, inheritance, interfaces, sound source-specific implementation mapping, MML priority, and Performer definition file structure. |
