# Shin-PSG Specification v0.3

## Overview

"Shin-PSG" is a concept that defines the performance method of electronic music, inheriting and evolving the lineage of Japanese electronic music.
Its essence lies in defining performance method itself, not the type of sound source.

In accordance with the definition of electronic music, the following sound sources are encompassed:

- PSG (Programmable Sound Generator)
- FM sound source
- PCM sound source
- And other electronic sound sources equivalent to the above

Performance data is created using MML (Music Macro Language), piano rolls, or other common electronic music production methods.

---

## Origin of the Name

### Why "Shin-PSG"?

Japanese electronic music evolved using PSG (Programmable Sound Generator) as its foundation.
The music culture on platforms such as MSX, Sharp X1, PC-88, and Famicom developed its own unique evolution starting from PSG, and the naming of this concept is based on that historical background.

"Shin" carries multiple overlapping meanings:

- **新 (New)** — Electronic music as a new definition and concept
- **真 (True)** — The will to inherit the essence of Japanese electronic music
- **進 (Progressive)** — Electronic music evolved to keep pace with the times
- **深 (Deep)** — Deepening of performance expression

### On Derivative Names

This specification uses "Shin-PSG" as the standard name, but the following names may also be used freely.
They all refer to the same specification and concept:

- **Shin-BEEP** — When viewed as originating from BEEP sound (a perfectly valid perspective)
- **Shin-FM** — When centered on FM sound sources
- **Shin-PCM** — When centered on PCM sound sources

Changing the name based on sound source type is a natural choice in accordance with the spirit of this specification.

---

## Relationship with Chiptune

"Chiptune" is a concept originating from Western demoscene culture and game music culture, a wonderful music genre with its own unique history and cultural value.

Japanese electronic music, on the other hand, has a different origin and a different evolutionary history.
Both use sound sources based on electronic chips (ICs), but the equation "uses electronic chips = chiptune" does not hold.
Since the origins, cultural backgrounds, and developmental histories differ, both should be distinguished as independent concepts.

Shin-PSG is rooted in the lineage of Japanese electronic music.
It does not deny chiptune, but is defined as something clearly distinct from it.

---

## Data Structure

### Note Track

A track describing the note data for each part on a time axis, identical to conventional electronic music.
Created using MML or piano roll format.

### Feel Track

A new concept in Shin-PSG.
An emotional parameter track defined for each part (instrument part) of the composition.
Arranged on a time axis like the Note Track, each emotional parameter's value is described as a freely varying curve (envelope curve).

The Feel Track represents the emotional expression intended by the composer.
During performance, it is interpreted and transformed through the Conductor and Performers,
so the values in the Feel Track are not reflected in the performance as-is.

Specifiable emotional parameters (each parameter specified numerically):

| Parameter | English Name |
|---|---|
| 希望 | Hope |
| 怒り | Anger |
| 悲しみ | Sadness |
| 喜び | Joy |
| 楽しみ | Fun |
| 嫉妬 | Envy |
| 貪欲 | Greed |
| 虚無 | Void |
| 不安 | Anxiety |

---

## Performance Engine Structure

### Conductor

Interprets the Feel Track during performance and conveys it to each Performer.

Conductor parameters:

| Parameter | Description |
|---|---|
| Interpretation | The tendency for how the Conductor interprets the emotions in the Feel Track. Defines whether to exaggerate or suppress them. |
| Charisma | The intensity of Feel transmission to Performers. The higher this is, the stronger the influence on Performers; the lower it is, the more Performers follow their own parameters. |
| Latency | The time delay before changes in the Feel Track are conveyed to Performers. |

When a Conductor exists without Performers (Performers undefined), the Conductor alone produces no effect on performance expression.
The Conductor's function becomes effective only when Performers are present.

---

### Performer

The entity that performs each Note Track. Similar in concept to operator definitions in FM sound sources,
a Performer is assigned to each track.

A Performer receives the Feel Track's emotional parameters in real time via the Conductor or Leader Performer,
and its internal emotional parameters vary moment by moment.
This variation passes through filters of the Conductor and the Performer itself,
so it does not necessarily match the Feel Track values, and in some cases, overreaction may occur.

#### Emotional Sensitivity Parameters

A group of parameters that define how receptive the Performer is to each Feel.

| Parameter | Description |
|---|---|
| Optimism | Sensitivity to Hope, Joy, and Fun |
| Patience | Tolerance for Anger and Anxiety. The lower this is, the more prone to overreaction. |
| Seriousness | Sensitivity to Sadness, Void, and Envy |

#### Emotional Dynamics Parameters

A group of parameters that define how the Performer receives and varies emotions.

| Parameter | Description |
|---|---|
| Sensitivity | How quickly and strongly the Performer reacts to emotional changes |
| Limiter | The upper and lower bounds of emotional parameter fluctuation |
| Decay Rate | The speed at which received emotions converge |
| Tracking | How well the Performer can follow sudden change curves in the Feel Track |

#### Technical Parameters

| Parameter | Description |
|---|---|
| Skill | Overall performance precision. The lower this is, the more directly and naturally emotions show in performance. The higher it is, the faster the decay rate even when emotions run high, making it easier to maintain performance precision. |

#### Conductor and Leader-Related Parameters

Parameters used when the Performer itself functions as a Conductor substitute (Leader) or when independently interpreting in the absence of a Conductor.
All Performers hold these parameters.

| Parameter | Description |
|---|---|
| Interpretation | The tendency for emotional interpretation when acting independently or without a Conductor |
| Charisma | The transmission intensity when influencing other Performers as a Leader Performer |
| Latency | The time delay before the Leader Performer's influence reaches other Performers |
| Trust | The degree of trust in the Leader Performer. The higher this is, the more readily the Performer accepts the Leader's emotional state; the lower it is, the more the Performer tends toward independent interpretation. |

---

## Performance Modes

The behavior during performance changes as follows depending on whether a Conductor, Leader Performer, and Performers are defined.

### Mode 1: Normal Performance (Conductor and Performers both undefined)

The Feel Track is disabled. Deterministic normal music playback is performed.

### Mode 2: Performers Only (No Conductor, No Leader)

Since no Conductor exists, each Performer independently interprets the Feel Track.
Following its own interpretation tendency parameter, each Performer directly references the Feel Track
and reflects it in its own internal emotional parameters.
The emotional states of Performers are independent of each other; no unified interpretation is performed.

### Mode 3: Conductor Present, Performers Present (Standard Configuration)

The Conductor interprets the Feel Track and, through Charisma, Interpretation, and Latency,
influences the internal emotional parameters of each Performer.
Performers further filter through their own parameters and reflect the result in performance expression.

### Mode 4: Conductor Present, No Performers

Even when only a Conductor exists, no effect on performance expression occurs.
This is effectively equivalent to Mode 1 (Normal Performance).

### Mode 5: Leader Performer Present (No Conductor, Band Configuration)

By designating a specific Performer as a "Leader," it can function as a Conductor substitute.
This is intended for assigning the central part of tempo and groove in a band — such as percussion (drums).

**Leader Performer behavior:**

- The Leader itself independently interprets the Feel Track and performs (Performer function)
- Simultaneously, it conveys its emotional state to other Performers using its own Interpretation, Charisma, and Latency parameters (Conductor substitute function)

**Non-Leader Performer behavior:**

- Receives transmissions from the Leader through its own Trust parameter
- The higher Trust is, the more the Performer follows the Leader's emotional state; the lower it is, the more the Performer's own independent interpretation takes precedence
- When multiple Performers with low Trust exist, ensemble cohesion is lost and performance may become individualistic or chaotic

---

## Performance Flow (Mode 3: Standard Configuration)

```
Feel Track (emotional curves representing the composer's intent, described on a time axis)
    ↓
Conductor's interpretation and conversion
(filtered through Interpretation, Charisma, and Latency)
    ↓
Real-time reflection in each Performer's internal emotional parameters
(further filtered by the Performer's Sensitivity, Optimism, Patience, Seriousness,
 Limiter, Decay Rate, and Tracking)
    ↓
Effect on performance expression
  ├── Note-on timing variation
  ├── Attack (note onset intensity) changes
  └── Release (note tail/decay) changes
    ↓
Sound source output (PSG / FM / PCM)
```

---

## Breakdown State

When a Performer's internal emotional parameters exceed the Limiter, it enters the "Breakdown State."

**Effects on performance in Breakdown State:**

- Note-on timing deviation expands beyond control
- Attack intensity takes erratically extreme values (excessive or absent)
- In the worst case, notes unrelated to the original track are sounded (Runaway)
- Or performance stops completely (Freeze Breakdown)

**Recovery from Breakdown State:**

- The Conductor can intervene by exercising Charisma to bring the Performer back
- If the Leader Performer is stable, Performers with high Trust in the Leader tend to recover more easily as the Leader's stable state propagates
- The stable performance state of other Performers propagating can pull the Performer back (mutual ensemble influence)
- If the Conductor or Leader's abilities are low, or many Performers have low Trust, the Breakdown may be uncontrollable and performance may collapse entirely

---

## Characteristics of Shin-PSG

- When a Conductor or Performer is defined, different performance results are generated each time from the same musical data (Non-deterministic Performance)
- The combination of Conductor, Leader Performer, and Performer definitions changes the individuality, interpretation, and ensemble cohesion of the performance
- When both Conductor and Performer are undefined, normal Deterministic Performance occurs (the Feel Track is disabled)
