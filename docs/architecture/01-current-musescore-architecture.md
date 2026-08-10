# Current architecture audit

Status: Phase 0 baseline, 2026-08-11
Repository: `github.com/finalverse/song`
Audited revision: `7e1de972a1`

## Executive summary

Song inherits a mature, modular C++20 and Qt 6 application. The engraving domain is the core asset and should remain the source of truth for score structure, layout, editing, playback events, and interchange. The safest path to an AI-native product is to add orchestration above the `project` and `notation` interfaces, express AI results as validated, undoable score operations, and leave the engraving engine and legacy formats compatible.

The codebase has three important architectural layers:

1. `muse/` supplies the reusable application framework: dependency injection, actions, commands, audio, networking, extensions, UI primitives, settings, and platform services.
2. `src/engraving/` owns the score domain and rendering engine.
3. The application modules in `src/` adapt that engine into editing, playback, file I/O, and Qt/QML product surfaces.

## Build and module topology

- The root `CMakeLists.txt` configures a C++20 Qt 6 application and exposes feature flags for most product modules.
- `src/CMakeLists.txt` adds modules in dependency order and substitutes stubs when an optional module is disabled.
- `src/app/appfactory.cpp` assembles framework and product modules. Modules publish global or per-project interfaces through the Muse IoC system.
- QML modules use URIs such as `MuseScore.AppShell`, `MuseScore.Project`, and `MuseScore.Playback`. Those are internal compatibility identifiers, not user-facing branding.
- `muse/` and `muse_deps/` are pinned Git submodules. They must remain reproducible and should not be casually fork-edited from the parent repository.

The main module flow is:

```text
Qt application
  -> AppFactory / module lifecycle
     -> AppShell + QML scenes
        -> Project / Notation interfaces
           -> Engraving domain and renderer
           -> Playback event model -> Muse audio engine
           -> Import/export registries
```

## Score data model

`mu::engraving::Score` in `src/engraving/dom/score.h` is the central editable score graph. It owns or addresses staves, parts, measures, segments, notes, text, lyrics, harmony, spanners, layout state, selection state, and editing transactions.

`mu::engraving::MasterScore` extends `Score` with project-wide state:

- excerpts and linked parts;
- tempo, time-signature, repeat, MIDI, and playback mappings;
- the undo stack and transaction manager;
- automation data;
- file information and score change notifications.

`EngravingProject` owns the lifetime of the `MasterScore`, loads and writes `mscz`, and runs post-load setup and corruption checks. The file format version is separate from the desktop product version; that distinction must be preserved during the rebrand.

## Editing and notation facade

The `notation` module is the stable application-facing facade over raw engraving objects.

- `INotation` exposes painting, interaction, MIDI input, undo, style, elements, accessibility, parts, and change notifications.
- `IMasterNotation` owns the master notation, excerpts, playback, and automation interfaces.
- `Notation` composes focused implementations such as `NotationInteraction`, `NotationUndoStack`, `NotationStyle`, `NotationParts`, and `NotationPainting` around an engraving `Score`.
- User edits are expected to pass through command and undo infrastructure. Direct mutation of engraving objects from a new AI feature would bypass validation, UI refresh, and reversibility.

This interface boundary is the correct integration point for future `ScorePatch` operations.

## Rendering pipeline

The renderer is deliberately separate from QML.

1. Score mutations mark layout ranges or the full score dirty.
2. `IScoreRenderer::layoutScore()` dispatches to the score layout pipeline.
3. Layout passes in `src/engraving/rendering/score/` resolve measures, systems, horizontal spacing, page layout, lyrics, beams, slurs, dynamics, and other notation-specific geometry.
4. `NotationPainting` adapts renderer output for the interactive canvas, PDF, print, PNG, and other consumers.
5. The notation scene draws interaction overlays without changing the printable score.

AI features should request notation operations, then allow the existing invalidation and layout pipeline to render the result. They should never generate positioned glyphs as a replacement for score semantics.

## Project and file I/O

`INotationProject` is the application-level aggregate for path, cloud metadata, load/save state, score metadata, `IMasterNotation`, and audio settings.

`NotationProject` distinguishes native score files from imported formats:

- Native `mscz/mscx/mscs` content is read through `EngravingProject` and compatibility migrations.
- Other formats are resolved by suffix through `INotationReadersRegister`.
- Writers are resolved through `INotationWritersRegister`.
- MusicXML, MIDI, MEI, Guitar Pro, MNX, Capella, and other formats are separate modules that register readers and writers during module initialization.

Compatibility implications:

- Keep `mscz`, `mscx`, `mscs`, their MIME types, and their existing UTI identifiers.
- Keep MusicXML and MIDI behavior stable.
- Preserve import/export module boundaries; an AI-generated score should enter the same semantic model as a human-authored score.
- Keep legacy product names where they identify an actual file format, migration source, API namespace, or third-party service.

## Playback pipeline

`NotationPlayback` converts score semantics into Muse Playback Events and maintains tempo, repeats, loop boundaries, chord-symbol playback, metronome data, and per-track events.

`PlaybackController` maps engraving instrument tracks to audio engine tracks, loads sound profiles, manages playback state, and coordinates mixer parameters and effects through the Muse audio framework.

The useful AI boundary is therefore semantic and non-destructive: suggestions may propose tempo, instrumentation, dynamics, articulations, automation, or sound-profile changes, but playback remains owned by the established notation and audio modules.

## UI architecture

- Product screens are Qt Quick/QML modules backed by C++ models.
- `appshell` owns the home, notation, publish, welcome, about, menu, toolbar, and splash experiences.
- `project`, `playback`, `palette`, `propertiespanel`, `instrumentsscene`, and `notationscene` expose their own QML modules.
- Internal page routing currently uses `musescore://` URIs.
- Cross-module actions and commands use registered action codes, UI contexts, and per-project IoC contexts.

For the first rebrand, user-visible labels and assets can change safely. Renaming QML URIs, route schemes, C++ namespaces, target names, or action codes would create a high-risk compatibility migration with little user value.

## Extension and plugin system

The Muse framework supports:

- API v2 extensions described by `manifest.json` and run as JavaScript or QML forms;
- legacy QML plugins through a controlled API v1 engine;
- extension actions contributed to menus and toolbars;
- execution points around project open, create, and save flows.

The existing extension system is a strong base for a future Song Plugin SDK, but it is not yet an adequate trust boundary for arbitrary model providers. AI provider adapters should begin as native modules with explicit capabilities, secret storage, cancellation, and network policy. A later SDK can expose a restricted provider interface.

## Brand and compatibility boundary

Safe to change now:

- application display and machine-readable names;
- Finalverse organization metadata and product URLs;
- splash, app icon, about dialog, welcome screen, menu labels, CLI banner;
- package metadata and generated PDF/SVG creator strings;
- README and product documentation.

Keep stable in the first phase:

- `mscore` command and native file extensions;
- `org.musescore.*` UTIs and MIME types that identify MuseScore-compatible documents;
- `MuseScore.*` QML imports and the `musescore://` internal route scheme;
- C++ namespaces and public plugin API names;
- engraving serialization and file-format version numbers;
- the `MuseScore Basic` sound profile and other upstream/third-party proper names.

## Baseline build result

Both submodules were initialized successfully. Qt 6.11.1 was installed through Homebrew, and a clean Debug configuration plus the complete 2,862-target Ninja build succeeded on macOS. The built application also completed a command-line runtime smoke test (`--long-version`) with exit status 0 and reported the Finalverse Song Studio identity.

The build emitted only upstream or environment warnings: the repository's AppleClang version guidance, optional Vulkan headers, and Homebrew Qt's newer macOS deployment target. The targeted MusicXML suite passed 355 of 357 tests. The two remaining inferred-credit fixtures differ only by tiny text-height measurements under Qt 6.11; all other MusicXML fixtures pass, so the environment-specific golden files should not be rewritten.

The validated build commands are:

```bash
export PATH="/opt/homebrew/opt/qt/bin:$PATH"
cmake -P build.cmake configure -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake -P build.cmake build -DCMAKE_BUILD_TYPE=Debug -G Ninja
```

## Architectural risks to manage

- AI edits that bypass the undo stack could corrupt score state or make changes irreversible.
- A broad text replacement of “MuseScore” would break compatibility identifiers and misattribute upstream work.
- Existing cloud, update, learning, and publishing surfaces point to MuseScore-operated services. They must be disabled or replaced before Song is distributed.
- Provider credentials must never enter score files, logs, crash reports, Git history, or extension manifests.
- Collaboration and AI history need stable identifiers for score elements; they should build on existing engraving element IDs rather than positional guesses.
