# PEDAL LEGENDS — Liberty Music Software

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