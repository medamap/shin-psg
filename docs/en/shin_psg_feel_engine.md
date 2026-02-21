# Shin-PSG Feel Engine Technical Document v0.2

## Overview

This document is a supplement to the Shin-PSG Specification, and serves as a technical reference
defining the internal processing by which emotional data from the Feel Track is converted into performance expression.

The Feel Engine processing consists of the following stages:

```
[Stage 1] Feel Track Input (9 emotions, independent curves)
    ↓
[Stage 2] Emotion Envelope Processing (temporal shaping of each emotion)
    ↓
[Stage 3] Emotion Algorithm Processing (modulation and synthesis between emotions)
    ↓
[Stage 4] Valence-Arousal Vector Conversion (θ, strength)
    ↓
[Stage 5] Interpretation Filter (Conductor, Leader, Performer)
    ↓
[Stage 6] Performance Expression Mapping
```

---

## Stage 1: Feel Track Input

The Feel Track is the emotional blueprint defined by the composer,
with 9 emotional parameters described as independent curves on a time axis.
Each emotion has a normalized value from 0.0 to 1.0 and can be defined independently.

| Emotion | English Name | Valence | Arousal |
|---|---|---|---|
| 希望 | Hope | + | Mid |
| 怒り | Anger | − | High |
| 悲しみ | Sadness | − | Low |
| 喜び | Joy | + | High |
| 楽しみ | Fun | + | Mid–High |
| 嫉妬 | Envy | − | High |
| 貪欲 | Greed | − | High |
| 虚無 | Void | − | Low |
| 不安 | Anxiety | − | Mid–High |

The Feel Track represents the composer's intent; at this stage no effect on performance occurs.
Multiple emotions having high values simultaneously is permitted by design,
and their interpretation and conversion is delegated to subsequent processing stages.

---

## Stage 2: Emotion Envelope Processing

### Overview

The value of each emotion received from the Feel Track is temporally shaped
by the Emotion Envelope configured for each Performer.
This expresses Performer-specific emotional characteristics such as "slow to warm up" or "holds onto feelings for a long time."

The Emotion Envelope has an ADSR structure, analogous to the envelope in an FM sound source.
However, the target of control is emotional intensity values, not volume.

### Emotion Envelope Parameters (configured per Performer, per emotion)

| Parameter | Description |
|---|---|
| Attack | The time for the Performer's internal emotional value to follow when the Feel Track value rises. Shorter = reacts immediately (short-tempered). Longer = emotions build gradually. |
| Decay | The time for the emotional value to settle down to the Sustain level after reaching its peak. Shorter = cools down quickly. Longer = lingers with emotion. |
| Sustain | The stable level (0.0–1.0) while the emotional trigger persists. Higher = emotion continues to influence performance. Lower = reacts momentarily then subsides. |
| Release | The time for the Performer's internal emotional value to return to zero after the Feel Track emotional value falls. Shorter = forgets quickly. Longer = holds onto the emotion for a long time. |

### Design Notes

9 emotions × 4 parameters (ADSR) results in 36 envelope parameters per Performer.
The combination of these determines the emotional "character" of the Performer, so
designing a Performer that expresses the intended emotional performance requires
careful adjustment based on a deep understanding of the relationship between emotions and performance parameters.
Like operator settings in FM sound sources, this is an advanced parameter set.

Even with the same Performer, the behavior varies significantly depending on how the Feel Track of the piece is written.
A natural concept of "compatibility" — where "this Performer suits this kind of piece" — emerges organically.

---

## Stage 3: Emotion Algorithm Processing

### Overview

Just as the operator connection topology (algorithm) in FM sound sources determines the timbre,
in Shin-PSG the modulation relationships between emotions (Emotion Algorithm)
determine how the final Valence-Arousal vector is derived.

### Carrier Emotion and Modulator Emotion

| Role | Definition |
|---|---|
| Carrier Emotion | An emotion that directly contributes to the Valence-Arousal vector |
| Modulator Emotion | An emotion that modulates the conversion characteristics of the Carrier Emotion |

**Important: The Carrier/Modulator relationship is determined dynamically.**

In FM, Carrier and Modulator are fixed by the algorithm,
but in Shin-PSG the relationship between Carrier and Modulator changes dynamically at each moment
based on the emotional intensity in the Feel Track.
The emotion with the highest intensity becomes the Carrier,
and the next strongest emotion acts as the Modulator — this is the basic behavior.
This is a concept not found in FM, and is a unique extension of Shin-PSG.

### Emotion Algorithm Topology

An Emotion Algorithm topology is defined per Performer.
The three basic topologies are as follows.

**Series Connection**
```
Modulator Emotion → Carrier Emotion → Valence-Arousal Vector
```
The Modulator modulates the Carrier's conversion characteristics before reaching the final output.
Effects tend to be amplified, producing a complex emotional texture.
Example: Anxiety modulating Anger creates "anger tinged with urgency" or "frightened anger,"
  where θ points in the anger direction but strength fluctuates unstably.

**Parallel Connection**
```
Emotion A ──┐
             ├→ Vector Addition → Valence-Arousal Vector
Emotion B ──┘
```
Multiple emotions independently contribute to the vector and are ultimately vector-summed.
A simple synthesis, but when opposing emotions are connected in parallel simultaneously,
the vectors cancel each other and strength decreases.
Emotionally turbulent but difficult to express state.

**Feedback Connection**
```
Emotion A → [Self-Modulation] → Valence-Arousal Vector
  ↑________________|
```
A loop structure where an emotion modulates itself.
Just as feedback in FM sound sources increases harmonics,
emotions self-reinforce and the risk of Breakdown increases.
This can become the shortest path to Breakdown State.
It has high expressive power but requires careful handling.

### Modulation Effect Examples

| Carrier | Modulator | Resulting Emotional Texture |
|---|---|---|
| Anger | Anxiety | Anger tinged with urgency. Strength fluctuates unstably. |
| Sadness | Envy | Aggressiveness seeping through sadness. Moments where Arousal suddenly spikes amid low Arousal. |
| Joy | Void | Empty joy. Tension never quite builds. A positive-side vector but strength is suppressed. |
| Hope | Anxiety | Unstable expectation. θ points to the positive side but strength keeps fluctuating. |
| Greed | Envy | Aggressive desire. High Arousal, negative side, strength tends to increase. |

---

## Stage 4: Valence-Arousal Vector Conversion

### Overview

The emotional data processed through Stage 3 is converted into
a 2D vector (θ, strength) on the Valence-Arousal plane.

```
Valence axis (horizontal): Pleasant (+) ← → Unpleasant (−)
Arousal axis (vertical):   High arousal (+) ↑ ↓ Low arousal (−)
```

### Basic Placement of 9 Emotions

```
              High Arousal
                  ↑
    Anger  Anxiety  Greed  Envy
Unpleasant ←───────────────────→ Pleasant
         Sadness  Void       Hope Joy Fun
                  ↓
              Low Arousal
```

### Definition of θ and strength

| Parameter | Definition |
|---|---|
| θ (Theta) | The angle of the vector on the Valence-Arousal plane. Represents the "direction/quality" of the emotion. |
| strength | The magnitude of the vector. Represents how strongly the emotion influences performance. The closer to 0, the smaller the influence on performance. |

When opposing emotions (e.g., Joy and Void) coexist in the Feel Track,
the vectors cancel each other and strength decreases.
This naturally expresses the state of "emotionally turbulent but difficult to express."

---

## Stage 5: Interpretation Filter

### Overview

The Valence-Arousal vector generated in Stage 4 is further transformed
through the interpretation filters of the Conductor, Leader, and Performer.
The same Feel Track generates completely different vectors depending on who interprets it.

See also: "Performance Modes" section in the Shin-PSG Specification.

### Interpretation Filter Mixing Ratio (in Leader mode)

```
Final Vector = Leader's Vector × Trust
             + Performer's own Vector × (1.0 − Trust)
```

A Performer with Trust = 1.0 fully follows the Leader's interpretation,
while a Performer with Trust = 0.0 performs entirely on its own interpretation.

---

## Stage 6: Performance Expression Mapping

### Overview

The final Valence-Arousal vector (θ, strength) is reflected in performance
as changes to note-on timing, Attack, and Release.

### Performance Characteristic Changes by θ

**High Arousal, Unpleasant side (Anger, Anxiety, Greed, Envy direction)**
- Note-on moves forward (timing rushes ahead)
- Attack becomes stronger and faster
- Release becomes shorter (abrupt cutoff)
- Tempo fluctuates faster
- Volume increases

**Low Arousal, Unpleasant side (Sadness, Void direction)**
- Note-on is pulled back
- Attack becomes weaker and slower
- Release becomes longer (lingering tail)
- Tempo fluctuates slower
- Volume decreases

**Pleasant side (Hope, Joy, Fun direction)**
- Note-on timing stabilizes
- Attack takes on a natural strength
- Release extends moderately
- Tempo stabilizes
- Volume becomes mid–high

**Special effect of Envy**
- Works to unbalance volume relative to other parts
- Behavior that tries to push its own part forward
- More likely to interfere with other Performers when performing in an ensemble

### Strength Controls Effect Intensity

The higher strength is, the stronger each performance effect becomes.
The closer strength is to 0, the less influence on performance, approaching normal playback.

---

## Stage 6-B: Performance Trait Parameters

### Overview

In addition to basic performance expression such as note-on, Attack, and Release,
Shin-PSG Performers have "habits" in their playing style — Vibrato, Tremolo, and Portamento.
These are defined as Performer-specific parameters and change in conjunction with the Emotion Vector (θ, strength).

**Important design principle: No MML notation required.**

Vibrato, Tremolo, and Portamento are automatically added to performance according to the Performer's trait parameters,
even without explicit notation in MML.
When explicitly notated in MML, the MML specification takes precedence (overrides the Performer's traits).
This means the design allows the choice between "leaving it to the Performer" or "the composer specifying it."

---

### Vibrato Trait Parameters

| Parameter | Description |
|---|---|
| Vibrato Affinity | How much the Performer tends to apply vibrato. 0 = almost never, 1.0 = applies actively. |
| Vibrato Delay | The time after note-on before vibrato begins. Shorter = vibrating from the start, longer = stabilizes before beginning to vibrate. |
| Vibrato Depth Fluctuation | Whether vibrato depth is constant or fluctuates with emotion. Higher = vibrato deepens as emotions intensify. |
| Vibrato Rate Sensitivity | How much vibrato speed changes in response to emotional changes. For example, vibrato speeds up in the Anger direction or slows in the Sadness direction. |

**Examples of emotion coupling:**
```
Anger direction (High Arousal, Unpleasant)  → Vibrato becomes faster and deeper
Sadness direction (Low Arousal, Unpleasant) → Vibrato becomes slower and deeper
Void direction                               → Vibrato thins out or disappears
Anxiety direction                            → Vibrato depth fluctuates irregularly
```

---

### Tremolo Trait Parameters

| Parameter | Description |
|---|---|
| Tremolo Affinity | How much the Performer tends to apply tremolo. |
| Tremolo Emotion Sensitivity | How much tremolo increases or decreases in response to emotional changes. For example, tremolo involuntarily mixes in when Anxiety or Anger rises. |
| Tremolo Threshold | The strength value above which tremolo begins to occur. Lower = even small emotional changes trigger tremolo. |

**Examples of emotion coupling:**
```
Anxiety direction → Tremolo tends to mix in involuntarily
Anger direction   → Tremolo becomes stronger and faster
Void direction    → Tremolo disappears; becomes listlessly flat
```

---

### Portamento Trait Parameters

Portamento is added as a Performer's trait even when not specified in MML.
When Portamento is specified in MML, the traits are layered on top of that specification.

#### Basic Curve

| Parameter | Description |
|---|---|
| Portamento Affinity | How much the Performer tends to apply portamento. |
| Portamento Curve | The basic curve shape of pitch movement: ease-in (slow start, fast arrival), ease-out (fast start, slow arrival), S-curve, linear, etc. |

#### Undershoot (reaching up from below)

Behavior where pitch momentarily dips below the target before ascending.
A natural habit unconsciously performed by string players and vocalists.

| Parameter | Description |
|---|---|
| Undershoot Amount | How much pitch drops before ascending. 0 = none. Changes with emotion. |
| Undershoot Curve | The curve shape of the transition from the dip back to the ascending motion. |

```
Normal portamento  :  ────/────
With undershoot    :  ──╲/────
                        (momentarily dips before ascending)
```

#### Overshoot (going past and returning)

Behavior where pitch momentarily exceeds the target before returning.

| Parameter | Description |
|---|---|
| Overshoot Amount | How much pitch overshoots after arrival. 0 = none. Changes with emotion. |
| Overshoot Curve | The curve shape of the transition from the overshoot back. |

```
Normal portamento  :  ────/──
With overshoot     :  ────/╲─
                          (slightly overshoots then returns)
```

#### Examples of emotion coupling

```
Anger direction
  → Overshoot increases (rushes ahead and overshoots)
  → No undershoot (no room to probe from below)
  → Curve is ease-in (arrives quickly)

Sadness direction
  → Undershoot increases (probing upward from below)
  → Curve is ease-out (arrives slowly)
  → Overshoot is small

Anxiety direction
  → Both undershoot and overshoot appear (unstable pitch)
  → Curve fluctuates irregularly

Hope/Joy direction
  → Undershoot at a natural amount (functions as musical expression)
  → Curve is S-curve (smooth)
```

---

## Training and Experience

### Overview

After a performance, Performers, Leaders, and Conductors gain experience from that performance.
Experience is recorded in a separate file and loaded as cache for the next performance.

### Training Environment

The training environment is unrestricted. Both of the following are valid:

- **Local definition**: Train by directly setting and adjusting parameters
- **Social platform**: Train on the platform, then export and use in performances

### Files and Identity

Performers, Leaders, and Conductors have names and profiles,
and can be nurtured through accumulated experience into unique individuals in the world.
The trained files can be exported, shared, and distributed.

### Design Philosophy

The parameter system is designed for advanced users, similar to FM sound sources.
Designing a Performer that expresses the intended emotional performance requires
deep understanding of the combination of Emotion Envelope, Emotion Algorithm, and Interpretation tendency.
With the premise that "creating a Performer that expresses emotions exactly as intended is difficult,"
the joy of trial-and-error and nurturing is part of the cultural value of this specification.

---

## Revision History

| Version | Notes |
|---|---|
| v0.1 | Initial version. Defined Emotion Envelope, Emotion Algorithm, Valence-Arousal vector conversion, and performance expression mapping. |
| v0.2 | Added Stage 6-B. Defined Performance Trait Parameters (Vibrato, Tremolo, Portamento). Described Undershoot, Overshoot, curve shapes, and emotion coupling. Clarified design principle of MML override priority. |
