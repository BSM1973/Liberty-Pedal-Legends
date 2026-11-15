# PEDAL LEGENDS — Liberty Music Software

**Vintage Soul. Modern Power.**

Clean-room, original project created from the supplied PEDAL LEGENDS specification (1973–2026). No other PEDAL LEGENDS implementation is imported.

## Interface localisation and text rendering (v0.2.1)

- Correct JUCE Unicode rendering: every non-ASCII C++ UI literal uses explicit UTF-8 decoding; unsupported decorative glyphs are replaced by portable punctuation or vector icons. Accents (é, è, à, ñ) are preserved.
- Native language selector in the free top-header gap, never touching presets, SAVE, A/B, BYPASS or audio meters. Available: Français (default), English, Español. Translations cover the board, editor, encyclopedia, navigation, effect families, preset categories and file dialogs; proprietary pedal and preset names remain unchanged.
- Chosen language is stored with the AU/VST3 project state; loading an older project defaults to French. The language control is hidden behind modal views so it cannot overlap their contents.
- No DSP architecture or 100 factory presets altered by this patch.

## Current implementation

- Independent real-time DSP engine with exactly **8 serial audio slots**. Slot-by-slot bypass, model replacement, independent state/parameters, order swapping and master bypass.
- **40 original effect models** with independently coded signal processing, across drive, distortion, fuzz, boost, compression, EQ, wah, modulation, tremolo, delay, reverb, monophonic octave synthesis and experimental.
- **100 factory presets**, 10 categories of 10 each; actual saved 8-slot settings with differing chains, levels and controls.
- JUCE 8 AU/VST3 plugin source with host automation, project state recall, navigable browser, live search/category filters, peak meters, controls and editable graphical pedalboard.
- New responsive **midnight-blue and gold editor** built around the approved three-screen design: 2×4 premium pedalboard, large focused editor, and read-only French pedal encyclopedia covering all 40 models. Layered gradients, brushed-metal pedal enclosures, gold rotary controls, and clear typography are drawn natively in JUCE. The generated design references are visual targets, not interactive images pasted over controls.
- Historical notes with era, sonic family, general physical-instrument inspirations and sound-design advice. These are explicitly educational references, not circuit-accurate clones.
- Searchable **40-model library** and **100-preset browser**, direct preset loading, saved user files (`.plegends`) and A/B editor comparisons.
- Per-slot MIX and LEVEL automation now changes real DSP output. New global stereo-linked noise gate with user threshold is optional and off by default.
- Mono and stereo processing paths. Per-parameter smoothing and soft transitions on bypass/model selection. No allocation in the DSP process callback after `prepare()`.

## Build

Requirements: CMake 3.24+, C++17 compiler, network access to fetch JUCE 8.0.10 if not installed, macOS Xcode (AU+VST3) or Windows/MSVC (VST3).

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPEDAL_LEGENDS_BUILD_PLUGIN=ON
cmake --build build --config Release --parallel 3
ctest --test-dir build -C Release --output-on-failure
```

Offline portable DSP tests, no JUCE required:

```sh
cmake -S . -B build-dsp -DPEDAL_LEGENDS_BUILD_PLUGIN=OFF -DPEDAL_LEGENDS_BUILD_TESTS=ON
cmake --build build-dsp --parallel 4
ctest --test-dir build-dsp --output-on-failure
```

GitHub Actions workflow builds AU/VST3 macOS, VST3 Windows and portable DSP on Linux **after publishing this source to a repository**. The workflow is provided but has not been run on a GitHub runner in this delivery.

## Honest validation status

**Passed locally:** C++ portable DSP compilation, all models produce audible mathematical output, correct eight-slot serial ordering and independent bypass, mono/stereo test, 100 factory preset rendering and numerical stability regression.

**Not yet confirmed for this new UI version:** native JUCE build on macOS/Windows, plugin loading in DAWs, state recall and A/B behavior in DAWs, audible comparisons against hardware, CPU benchmarks, texture fidelity against reference renders, anti-aliased nonlinear oversampling, production-grade polyphonic pitch shifting. Monophonic octave effects are explicitly described as monophonic. No circuit-level hardware-accuracy claims.

The 120-model library is a future expansion target, not a claim of 120 completed algorithms. A/B state comparison and user preset save/load are now implemented in the editor. Per-pedal photorealistic texture artwork and further DSP validation remain ongoing.

## No third-party plugin hosting

Effects are implemented in-process, not VST/AU hosted in pedal slots. No third-party runtime audio engine.