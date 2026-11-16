#pragma once
#include <cstdint>

// Stable, independent MIDI CC addresses for 8 slots plus global controls.
// 8 controls per slot: enabled, mix, level, branch, and four DSP controls.
// Never route a CC to the model selector (changing algorithms mid-note).
namespace pedal { namespace midi {
constexpr int kPerSlot=8;
constexpr int kSlots=8;
constexpr int kGlobals=8;
constexpr int kTargetCount=kSlots*kPerSlot+kGlobals;
constexpr int kNoController=-1;
enum SlotTarget : int { Enabled=0, Mix=1, Level=2, Branch=3,
                        Control1=4, Control2=5, Control3=6, Control4=7 };
enum GlobalTarget : int { Input=0, Output=1, WetDry=2, Bypass=3,
                          NoiseGate=4, GateThreshold=5, ParallelMode=6, ParallelBlend=7 };
constexpr int slot(int slotIndex,int control) noexcept {
    return slotIndex>=0&&slotIndex<kSlots&&control>=0&&control<kPerSlot
        ? slotIndex*kPerSlot+control:-1;
}
constexpr int global(int control) noexcept {
    return control>=0&&control<kGlobals?kSlots*kPerSlot+control:-1;
}
constexpr int encode(int channel,int controller) noexcept {
    // Reserve channel mode 120-127, which should never become effect automation.
    return channel>=1&&channel<=16&&controller>=0&&controller<120
        ?(channel-1)*128+controller:kNoController;
}
constexpr int channel(int code) noexcept {return code>=0?code/128+1:0;}
constexpr int controller(int code) noexcept {return code>=0?code%128:-1;}
} } // namespace pedal::midi
