# Refactoring and delivery roadmap

## Delivery principles

- Protect the engraving engine and file compatibility.
- Keep each pull request reviewable and independently buildable.
- Separate brand, architecture, infrastructure, and feature changes.
- Add interfaces before provider implementations.
- Make AI edits bounded, previewable, and undoable before expanding capability.
- Preserve upstream attribution and make future upstream merges practical.

## Phase 0 — understand and establish guardrails

Deliverables:

- current architecture audit;
- product vision;
- AI integration plan;
- refactoring roadmap;
- compatibility boundary for naming and identifiers;
- baseline build instructions and dependency record.

Exit criteria:

- the architecture documents agree with current source boundaries;
- submodules are initialized at pinned revisions;
- a clean local configure/build can be reproduced once Qt is available;
- no secrets are present in Git remotes, source, generated documentation, or logs.

## Phase 1 — brand rebirth

### PR 1: identity foundation

- change the application display name to Finalverse Song Studio;
- set Finalverse Inc. organization metadata and product URLs;
- introduce the Song logo, app icon master, splash, welcome visual, color tokens, and brand guidelines;
- update core shell strings, About, first launch, CLI identity, README, generated PDF/SVG creator metadata, and package metadata;
- disable inherited MuseScore cloud/update/learning integrations until Finalverse-owned services exist;
- preserve legacy format identifiers, QML namespaces, route schemes, and command names.

Verification:

- configure and build on macOS;
- render and inspect vector assets;
- confirm app name, bundle ID, title bar, splash, About, and first-launch screens;
- run filename-only secret scans and review the staged diff.

### PR 2: packaging completion

- finish Linux AppStream/desktop naming and validation;
- finish Windows installer naming, GUID policy, and visual assets;
- define Apple signing/notarization identities owned by Finalverse;
- generate and verify `.icns` and `.ico` outputs from the checked-in vector master;
- decide whether to preserve `mscore` only as a CLI alias or introduce `song` as the primary command;
- add CI jobs for all three platforms.

### PR 3: inherited service separation

- inventory every upstream network endpoint;
- classify endpoints as required compatibility infrastructure, replaceable content, or disabled product service;
- introduce Finalverse configuration for updates, docs, privacy, account, collaboration, and publishing;
- add an offline mode and endpoint allow-list tests.

## Phase 2 — AI architecture foundation

### Milestone 2.1: optional AI module

- add `MUE_BUILD_AI_MODULE`;
- add provider-neutral interfaces, request lifecycle, cancellation, and capability routing;
- add platform secret storage abstraction and log redaction;
- ship a deterministic fake/local provider for tests.

### Milestone 2.2: score context

- define versioned score and selection snapshots;
- map stable engraving element IDs;
- add scope visualization and token/size budgeting;
- add golden fixtures for common score structures.

### Milestone 2.3: read-only Assistant

- add a dockable Assistant panel;
- analyze the current score or selection;
- stream results and show provider/privacy state;
- add keyboard and accessibility support;
- do not mutate scores in this milestone.

Exit criteria:

- AI can be disabled at compile time and runtime;
- no provider type leaks into feature controllers;
- no credentials or score content appear in logs;
- closing or changing projects cancels stale requests safely.

## Phase 3 — first reversible musical actions

- define the initial `ScorePatch` schema;
- implement parser, validator, preview, and executor;
- support one or two narrow actions such as tempo and dynamics;
- apply each accepted patch as a single named undo transaction;
- record minimal AI history and user outcome.

Exit criteria:

- invalid or stale patches cannot mutate the project;
- undo restores the prior semantic state;
- preview scope matches applied scope;
- existing notation and file-format tests remain green.

## Phase 4 — bilingual lyrics intelligence

- add lyric extraction and alignment models;
- support Chinese/English language and pronunciation metadata;
- present alternatives and singability warnings by phrase;
- apply lyric-only patches by default;
- export aligned lyrics correctly through native and MusicXML paths.

Exit criteria:

- golden alignment corpus passes agreed quality thresholds;
- melismas, ties, pickups, and mixed-language passages are handled;
- users can compare, accept, refine, and revert each phrase.

## Phase 5 — arrangement and composition

- expand `ScorePatch` operations for harmony, voicing, rhythm, form, and instrumentation;
- add auditionable suggestion branches;
- support constrained generation into a selected range or new part;
- add structural and playability validators by instrument;
- introduce user-owned style and reference controls with rights metadata.

Do not begin full-song generation until bounded patch execution is reliable and evaluated.

## Phase 6 — audio understanding

- add local/cloud transcription providers behind capability interfaces;
- ingest audio as a referenced asset with provenance;
- detect tempo, meter, sections, melody, chords, and instruments;
- show confidence and let users correct analysis before score creation;
- create an editable score through normal project/import pathways.

## Phase 7 — collaboration and publishing

- define stable project, element, comment, version, and collaborator IDs;
- build versioned sync operations on the same musical patch model;
- add comments and suggestion branches before real-time co-editing;
- add Finalverse account, access control, project sharing, and publishing services;
- preserve offline-first desktop creation.

## Refactoring backlog

High value, low disruption:

- centralize product identity strings and URLs in generated configuration;
- introduce a checked-in asset generation script for icons and splash derivatives;
- separate legacy MuseScore service terminology from native file-format terminology;
- add a `Finalverse.Song` namespace only for new modules;
- add architecture tests that prevent AI modules from depending on engraving internals.

Higher risk; defer until justified:

- renaming `MuseScoreStudio` CMake targets;
- renaming C++ namespaces;
- changing QML module URIs;
- changing the `musescore://` route scheme;
- replacing native file extensions, MIME types, or UTIs;
- rewriting the project container or engraving engine.

## Release gates

Every public Song build should require:

- clean builds on supported platforms;
- no critical regression in notation, import/export, playback, or printing;
- secret and endpoint scans;
- license and upstream attribution review;
- verified app/package identity;
- privacy review for every networked feature;
- deterministic rollback or feature flag for new AI capabilities;
- published known limitations and compatibility notes.
