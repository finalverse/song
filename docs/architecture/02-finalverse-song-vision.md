# Finalverse Song vision

## Product definition

**Song** is Finalverse Inc.’s AI-native workspace for composing, arranging, performing, collaborating on, and publishing music. The desktop application is **Finalverse Song Studio**.

The long-term ambition is simple: make Song the operating system for music creation—where human taste, musical intelligence, professional notation, audio, and collaboration share one reversible project history.

The product should feel broader than a notation editor without weakening the notation engine that makes it professionally useful.

## Positioning

> From an idea to a living score.

Song is for composers, songwriters, singers, bands, educators, arrangers, producers, publishers, and creative agents. It should welcome a creator who starts with a sentence, a hummed melody, a chord loop, a lyric, a MIDI performance, an audio reference, or an existing score.

The durable differentiator is not one generative model. It is the project system that lets people move between intent, musical structure, sound, and publishable notation while keeping every decision editable and attributable.

## Product pillars

### Create from any starting point

Accept text, notation, MIDI, audio, lyrics, chords, and reference material. Convert ideas into structured score data rather than flattening them into an opaque audio result.

### Stay musically editable

AI output must remain notes, rhythms, harmony, lyrics, form, instrumentation, expression, and automation that a musician can inspect and change.

### Preserve authorship and control

Every AI operation is a proposal with a preview, rationale, scope, and undo path. Song should augment musicians, not silently replace their decisions.

### Sound and look professional

The inherited engraving and playback systems remain first-class. A project should produce a beautiful score, parts, tabs, MIDI, MusicXML, PDF, and audio preview from one source of truth.

### Collaborate around musical meaning

Comments, versions, suggestions, and future real-time editing should attach to measures, notes, lyrics, sections, and arrangements—not just pixels or timestamps.

## Core creation loop

```text
Intent
  -> understand the current song and selection
  -> propose one or more musical plans
  -> preview an explainable change
  -> apply as an undoable score transaction
  -> listen, compare, refine, or revert
  -> publish or collaborate
```

Examples:

- “Make the chorus feel wider without changing the melody.”
- “Create a playable fingerstyle guitar arrangement from these piano chords.”
- “Align this Mandarin translation to the existing melody and flag difficult syllables.”
- “Turn this voice memo into a lead sheet, then let me correct the chords.”
- “Prepare transposed parts and a rehearsal track for the band.”

## The SongProject model

The future product aggregate extends the existing notation project rather than replacing it:

```text
SongProject
  metadata and rights
  score and parts
  lyrics and language variants
  audio references and renders
  arrangement and performance settings
  AI action history
  comments, versions, and collaborators
  publishing targets
```

The first implementation should store AI history as sidecar project data inside the native project container only after the schema, privacy model, and migration rules are defined. Until then, prototypes should keep history in a local development store.

## AI-native does not mean AI-only

Song must remain excellent when all AI features are disabled. Manual notation, keyboard shortcuts, MIDI input, playback, import/export, plugins, printing, and accessibility are product foundations, not legacy mode.

AI features should satisfy five requirements:

1. provider-neutral;
2. selection-aware;
3. previewable;
4. undoable;
5. testable against musical and structural constraints.

## Initial product wedge

The first AI experience should be an Assistant panel that understands the open project and current selection. It should begin with analysis and bounded transformations rather than open-ended full-song generation.

Initial capabilities:

- summarize key, meter, tempo, form, instrumentation, range, and texture;
- explain a selected passage;
- suggest tempo, harmony, voicing, dynamics, or instrumentation changes;
- produce a structured plan and preview before editing;
- apply a narrow set of validated changes through the undo stack.

The first signature workflow after that should be bilingual lyric alignment and singability, especially Chinese/English adaptation. It is distinctive, measurable, and naturally combines language intelligence with the existing score model.

## Collaboration vision

Song should become “Figma for music,” but collaboration must follow project semantics:

- presence at project, part, measure, and selection level;
- comments anchored to stable engraving element IDs;
- suggestion branches that can be auditioned before merge;
- version history that records human and AI authorship;
- conflict handling at musical-operation granularity;
- shareable review links with audio and rendered notation.

## Trust, safety, and rights

- Never train on user content without explicit, informed consent.
- Make local-only and no-retention provider modes visible.
- Do not transmit a full score when a selected range is sufficient.
- Separate provider credentials from projects and logs.
- Record model/provider metadata and input scope for reproducibility.
- Add provenance and rights metadata for imported references and generated material.
- Detect and stop invalid, destructive, or unexpectedly broad score changes before commit.

## Brand architecture

- Product: **Song**
- Desktop application: **Finalverse Song Studio**
- Company: **Finalverse Inc.**
- Canonical product domain: `song.finalverse.com`
- Company product route: `finalverse.com/song`
- Repository: `github.com/finalverse/song`

“Finalverse” provides trust and ecosystem context; “Song” should remain the memorable product name in navigation, conversation, and future collaboration URLs.

## Success measures

Product quality should be measured by useful musical outcomes, not token volume or generation count:

- time from idea to editable first draft;
- acceptance and revision rate of AI suggestions;
- undo/revert rate by feature and model;
- structural validity of applied score patches;
- export success across native, MusicXML, MIDI, and PDF workflows;
- rehearsal or publishing completion rate;
- collaboration activity around meaningful score anchors;
- retention among musicians who use both manual and AI workflows.
