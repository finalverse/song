# Finalverse Song

![Finalverse Song](branding/logo/song-lockup.svg)

**From an idea to a living score.**

Finalverse Song is an open-source, AI-native music creation platform for composing, arranging, playing, collaborating on, and publishing music. The desktop application is **Finalverse Song Studio**.

Song is built on the MuseScore Studio notation and engraving engine. The project preserves professional score editing, playback, parts, tablature, printing, MusicXML, MIDI, Guitar Pro, and existing MuseScore-compatible files while building a provider-neutral music intelligence layer around them.

## Product direction

Song is being designed as a human + AI music workspace:

- start from notation, MIDI, audio, lyrics, chords, or a written idea;
- understand the current score and musical selection;
- preview explainable AI suggestions before applying them;
- keep every AI edit structured, editable, and undoable;
- move from composition to parts, tabs, audio preview, PDF, MusicXML, and publishing;
- collaborate around measures, notes, lyrics, arrangements, and versions.

The notation engine is a foundation, not a rewrite target. New intelligence belongs above the score model and must use the existing command, undo, rendering, playback, and import/export pathways.

## Current phase

The project is in its architecture and brand-foundation phase. The first design documents are in [`docs/architecture`](docs/architecture):

- current MuseScore architecture audit;
- Finalverse Song product vision;
- provider-neutral AI integration plan;
- incremental refactoring and delivery roadmap.

Visual identity source files and usage guidance live in [`branding`](branding).

## Compatibility

Song intends to remain compatible with:

- `.mscz`, `.mscx`, and existing MuseScore Studio projects;
- MusicXML and compressed MusicXML;
- MIDI and karaoke MIDI;
- Guitar Pro, MEI, MNX, Capella, and other formats supported by the inherited import/export modules;
- the existing MuseScore plugin API while a future Song Plugin SDK is designed.

Internal `MuseScore.*` QML names, `musescore://` routes, native format identifiers, and historical product references remain in the code where changing them would harm compatibility or attribution.

## Building

The desktop application uses C++20, Qt 6, QML, CMake, and Ninja on macOS.

Initialize the pinned framework dependencies:

```bash
git submodule update --init --recursive
```

Make Qt available, then configure and build:

```bash
export PATH="/opt/homebrew/opt/qt/bin:$PATH"
cmake -P build.cmake configure -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake -P build.cmake build -DCMAKE_BUILD_TYPE=Debug -G Ninja
```

For release, testing, platform dependencies, and unit-test workflows, see the inherited build scripts and the upstream [MuseScore Studio developer documentation](https://github.com/musescore/MuseScore/wiki/Set-up-developer-environment).

## Project links

- Product: [song.finalverse.com](https://song.finalverse.com/)
- Company route: [finalverse.com/song](https://finalverse.com/song)
- Repository: [github.com/finalverse/song](https://github.com/finalverse/song)

## Contributing

Keep changes small, documented, testable, and compatible with the notation engine. New AI features must be modular, provider-neutral, privacy-aware, cancellable, previewable, and reversible.

Do not commit provider credentials, access tokens, private endpoints, or user music. Never embed credentials in Git remote URLs or diagnostic output.

## License and upstream attribution

Finalverse Song Studio is based on MuseScore Studio and is licensed under the GNU General Public License version 3. See [`LICENSE.txt`](LICENSE.txt).

Copyright for inherited MuseScore Studio code remains with MuseScore Limited and its contributors. New Finalverse contributions are copyright Finalverse Inc. and contributors unless otherwise stated. Existing source headers and third-party notices must be preserved.
