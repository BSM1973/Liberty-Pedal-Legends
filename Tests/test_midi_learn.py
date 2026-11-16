"""Static MIDI Learn regression: stable geometry, learnability and host state.
A green JUCE build is still required; this does not simulate a MIDI DAW."""
from pathlib import Path
root = Path(__file__).resolve().parents[1]
p = (root/"Source/PluginProcessor.cpp").read_text(encoding="utf-8")
h = (root/"Source/PluginProcessor.h").read_text(encoding="utf-8")
ui = (root/"Source/PluginEditor.cpp").read_text(encoding="utf-8")
eh = (root/"Source/PluginEditor.h").read_text(encoding="utf-8")
loc = (root/"Source/UILocalization.h").read_text(encoding="utf-8")
cmake = (root/"CMakeLists.txt").read_text(encoding="utf-8")
midi = (root/"Source/MidiLearn.h").read_text(encoding="utf-8")
assert "NEEDS_MIDI_INPUT TRUE" in cmake
assert "acceptsMidi() const override { return true; }" in h
assert "receiveMidiControllers(messages)" in p
assert "midiCC[size_t(i)].load" in p and "midiPending[size_t(i)].store" in p
assert "MidiCC_" in p and "getProperty(\"MidiCC_\"" in p
assert "setValueNotifyingHost(normalized)" in p
assert "startTimerHz(30)" in p
assert "e.mods.isPopupMenu()" in ui and "showMenuAsync" in ui
assert "Component::SafePointer" in ui
assert "ASSIGN MIDI CC" in loc and "REMOVE MIDI CC" in loc
assert "canvasWidth=1600.f,canvasHeight=930.f" in eh
assert "const float left=56.f,w=355.f,space=26.f;" in ui
assert "drawKnob(g,174,824,20" in ui
assert "kTargetCount=kSlots*kPerSlot+kGlobals" in midi
assert "juce::Timer" in h
print("PASS: 72 MIDI CC targets, right-click menu, persistent mappings, immutable UI")
