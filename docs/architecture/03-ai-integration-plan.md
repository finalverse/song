# AI integration plan

## Architectural goal

Add music intelligence without coupling the score engine to a model vendor or allowing model output to mutate engraving objects directly.

The proposed `src/ai/` module should translate between the existing notation/project interfaces and provider-neutral AI requests. It should be optional at build time and usable with local or cloud providers.

## Proposed module boundaries

```text
Song UI
  IAssistantController
    ContextBuilder
      ScoreSnapshotAdapter
      SelectionSnapshotAdapter
    IAiService
      IModelRouter
        IAiProvider (local or cloud)
    ResponseParser
      ScorePatchValidator
      ScorePatchPreview
      ScorePatchExecutor -> notation commands + undo stack
    IAiHistoryStore
```

Suggested source layout:

```text
src/ai/
  CMakeLists.txt
  aimodule.{h,cpp}
  iai*.h
  internal/context/
  internal/providers/
  internal/router/
  internal/scorepatch/
  internal/history/
  qml/Finalverse/Song/Ai/
  tests/
```

The QML URI can use the new `Finalverse.Song.Ai` namespace because it is a new API. Existing `MuseScore.*` QML imports should not be renamed as part of this work.

## Provider abstraction

`IAiProvider` should expose capabilities, not vendor-specific endpoints:

```cpp
struct AiProviderCapabilities {
    bool text;
    bool structuredOutput;
    bool audioInput;
    bool streaming;
    bool localOnly;
};

class IAiProvider {
public:
    virtual std::string id() const = 0;
    virtual AiProviderCapabilities capabilities() const = 0;
    virtual AiRequestHandle submit(const AiRequest&) = 0;
    virtual void cancel(const AiRequestId&) = 0;
};
```

Provider adapters may support OpenAI, Anthropic, Gemini, Grok, DeepSeek, Ollama, llama.cpp, whisper.cpp, or later music/audio models. No feature controller should include a provider SDK directly.

The router selects a provider by capability, user preference, privacy mode, context size, latency budget, and availability. A feature may declare that it requires structured output or local-only execution.

## Score context

Do not send raw native files by default. Build a bounded, versioned context representation from the current project and selection.

The first snapshot schema should include:

- project and part identity;
- selection start/end in musical time;
- key, meter, tempo, form markers, and transposition context;
- parts, instruments, ranges, and staff types;
- notes, rests, ties, tuplets, harmony, lyrics, dynamics, articulations, and expressions in scope;
- stable element IDs where available;
- a compact MusicXML or purpose-built JSON representation only when needed.

Snapshots must be immutable, serializable for tests, and explicit about omitted data.

## ScorePatch intermediate representation

Model output should be parsed into a small musical operation language. Early operations might include:

- set tempo or tempo text;
- add, replace, or transpose harmony symbols;
- add or replace lyrics in a bounded range;
- set dynamics or articulations;
- change instrument assignment;
- insert or replace notes in one voice over a bounded range;
- add a rehearsal mark or section label.

Every patch carries:

- project revision and selection scope;
- operation list;
- human-readable summary and rationale;
- preconditions;
- expected affected elements and measures;
- provider/model metadata;
- optional confidence and alternatives.

The validator rejects unknown operations, stale project revisions, out-of-range targets, invalid durations, unsupported tuplets, illegal voices, destructive scope expansion, and mutations outside the requested selection.

## Preview and execution

The execution sequence is:

1. capture the project revision and selection;
2. request analysis or a patch;
3. validate syntax and musical invariants;
4. apply the patch to a temporary or cloned score context when practical;
5. render a visual diff and auditionable preview;
6. ask the user to apply;
7. execute through notation commands inside one named undo transaction;
8. record the result, affected IDs, and before/after summary;
9. roll back the entire transaction on any failure.

The executor must never hold raw `Score*` pointers across asynchronous provider calls. Resolve the active project and validate its revision again at apply time.

## AI history

History is an audit and recovery feature, not a chat transcript dump.

Store:

- user intent;
- selected scope;
- normalized request metadata;
- provider/model identifier and policy mode;
- validated patch;
- apply result and undo command identity;
- user acceptance, rejection, refinement, or revert;
- timestamps and authorship.

Do not store provider credentials, raw authorization headers, hidden prompts, or unrelated project content. Give users a way to delete AI history independently of the score.

## Privacy and credential handling

- Use platform credential storage through a dedicated secret interface.
- Redact known credential formats from logs and diagnostics.
- Never place tokens in URLs, command lines, Git remotes, settings exported with a project, or crash metadata.
- Show the exact scope sent to a cloud provider.
- Prefer selection-only context and allow a local-only workspace policy.
- Cancel and discard responses when the project closes or the request becomes stale.

## First vertical slice: Assistant analysis

The first implementation should be read-only:

1. add the optional `ai` module and provider interfaces;
2. add a local deterministic analysis provider for tests;
3. build a selection snapshot from `INotation`;
4. display key, meter, tempo, instrumentation, range, density, and lyric observations in an Assistant panel;
5. stream or render provider commentary without score mutation;
6. add cancellation, error states, privacy scope, and telemetry-free local logging.

The second slice adds one bounded edit—such as applying a tempo suggestion—as a full preview/validate/undo path. This proves the architecture before composition or arrangement generation is attempted.

## Lyrics alignment track

The bilingual lyric feature should use a dedicated pipeline rather than a generic chat response:

- extract melody notes, ties, melismas, phrase boundaries, stresses, and existing syllables;
- tokenize source and target languages with language-specific pronunciation data;
- score candidates for syllable count, vowel duration, stress, tone contour, rhyme, meaning, and singability;
- return aligned alternatives with per-phrase warnings;
- apply only lyric operations, never note changes, unless the user explicitly requests melody adaptation.

Golden tests should cover Mandarin/English, Cantonese/English, contractions, pickups, melismas, tied notes, punctuation, and mixed-language lines.

## Testing and evaluation

- Unit-test snapshots, parsers, validators, redaction, routing, and history serialization.
- Use golden score fixtures for every executable patch type.
- Run existing engraving, notation, project, playback, MusicXML, and MIDI tests after integration.
- Fuzz structured model output and reject malformed or oversized patches.
- Add invariant tests: one patch equals one undo step; undo restores a byte-equivalent semantic snapshot where expected.
- Evaluate usefulness separately from structural validity. A patch may be valid but musically poor.

## Non-goals for the first implementation

- full-song autonomous generation;
- training models inside the desktop application;
- real-time multi-user synchronization;
- renaming the engraving domain;
- exposing private provider credentials to plugins;
- embedding vendor-specific response objects in the project format.
