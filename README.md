# PEDAL LEGENDS — Liberty Music Software

## v0.6.0 - Right-click MIDI Learn

- Right-click any working knob in the focused editor, the three miniature pedal knobs, the pedal footswitch, master/gate controls or the routing blend to select **Learn MIDI CC** or **Clear MIDI CC**. Routing A/B and lane branch buttons are also MIDI assignable.
- MIDI input is enabled for AU and VST3; select an active MIDI input and route MIDI to the audio-plugin track in your DAW.
- 72 stable assignable targets (eight per slot plus eight global controls), channel-aware CC 0-119; one assignment per target. MIDI CC 120-127 reserved for channel mode.
- Audio thread only writes lock-free atomic pending values. A processor-owned 30 Hz message-thread timer performs host parameter notifications even with the editor closed.
- MIDI assignments are stored with the project state and survive preset changes. Loading an older project without mapping data is safe.
- Main pedalboard remains exactly 1600×930 with no changes to button, knob, pedal or routing geometry.
- Map changes do not alter factory presets, audio algorithms, history pages or colour palettes.

## v0.5.0 — 400 factory presets (200 series + 200 parallel)

**Backward compatibility is preserved:** the original 200 factory presets remain byte-for-byte equivalent in name, index, routing, model settings and level parameters. Their stable positions are 0–99 (series) and 100–199 (parallel). Exactly 100 new serial patches are appended at indices 200–299, and 100 new parallel A/B patches at indices 300–399.

All 200 additions explore the 40 new DSP pedals introduced in v0.4, with 10 musical categories and purposefully selected five-to-seven-stage chains. Every new patch uses at least three of the added pedals; every new pedal appears in both new banks. The preset browser filters and headings now show 400 total / 200 series / 200 parallel in Français, English, and Español.

**No graphic geometry or pedal graphics have been changed:** the 1600 × 930 layout, 8 pedal slots, knob sizes, colours, full library of 80 history entries and series/parallel DSP modes remain unchanged. Test coverage verifies all factory presets render finite nonzero output; each added parallel preset routes both lanes A and B; 40 models are represented in each new bank. Final listening and visual QA inside a DAW are still necessary.

## v0.4.1 - Luxury colour families (visual-only)

80 original Liberty pedal enclosures now use **13 distinct family palettes**: olive green overdrive, forest-green distortion, burgundy fuzz, copper boost/preamp, rose-copper compression, olive-khaki EQ, honey/orange wah, yellow-ochre modulation, amber tremolo, caramel delay, warm-sand reverb, blue-violet pitch and petrol-blue experimental. Each model has a small deterministic finish variation; catalogue thumbnails use the same colour. Gold/cream typography remains unchanged and tested for >=4.5:1 contrast at the lightest gradient endpoint. No panel, pedal, knob, label or hitbox dimensions have changed. DSP, original model IDs, factory presets, series/parallel routing and all 80 history entries are untouched.

**Vintage Soul. Modern Power.**

## v0.4.0 — 80 original effect models

PEDAL LEGENDS is a working real-time guitar / bass multi-effects plugin, not an instrument or an AU/VST host. This expansion preserves the original 40 model IDs, the eight slot DSP architecture, series / true parallel A+B routing, the 100+100 factory preset banks, and the 1600 × 930 reference UI coordinate system.

### Expansion (model numbers 41–80)

- 5 overdrives: Neo Classic Push, Sapphire Valve, Mercury Nine, Fingerstyle Glow, Copper Contours
- 5 distortions: Legacy Orange, Cosmic Lead, Neo Fire Lead, Royal Harmony, Titanium Riff
- 5 fuzzes: Octave Halo, Sustain Cloud, Germanium Mist, Silicon Razor, Nebula Square
- 5 boosts/preamps: Regal Treble, Classical Push, Crystal Fingers, Legacy Presence, Velvet Preamp
- 5 compressors: Finger Pulse, Glass Limiter, Lead Sustain, Neo Attack, Royal Comp
- 5 wah/envelope effects: Expressive Sweep, Cosmic Wah, Vintage Crying, Funk Prism, Talk Boxish
- 5 modulations: Stereo Dimension, Phase Garden, Liquid Rotary, Aqua Vibrato, Golden Flange II
- 5 delays: Chrono Voyager, Dual Orbit, Counterpoint Eight, Velvet Slap, Reverse Bloom

Each new model has its own DSP case, controls, defaults, non-endorsement historical reference, tone description and suggested use. These are **original digital interpretations**. They are neither endorsed signature products nor circuit-verified hardware clones. Guitarist and hardware mentions are educational references only.

The encyclopedia browse range and library model count have been extended to 80, and searching a name, era, guitar artist, or historical pedal family can find the relevant model. The three languages from the previous release remain available for UI navigation; the new extended editorial notes are currently authored in French, pending editorial translations to English and Spanish.

### Validation

Portable C++ DSP test suite: verify 80 model IDs, finite audio, independent processing, bypass, mono and stereo, sample rates, parallel branches and 200 original presets. UI text/routing tests check preservation of the existing panel geometry and 80-card access. Real AU/VST3 compilation and DAW load testing must be confirmed by the GitHub Actions results and in a DAW.

### Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPEDAL_LEGENDS_BUILD_PLUGIN=ON
cmake --build build --config Release --parallel 3
ctest --test-dir build -C Release --output-on-failure
```

Portable DSP tests without JUCE:

```sh
cmake -S . -B build-dsp -DCMAKE_BUILD_TYPE=Release -DPEDAL_LEGENDS_BUILD_PLUGIN=OFF
cmake --build build-dsp --parallel 4
ctest --test-dir build-dsp --output-on-failure
```