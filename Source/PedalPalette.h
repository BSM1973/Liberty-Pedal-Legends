#pragma once
#include "PedalDSP.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

// Original Liberty per-family colour identity. Visuals only: DSP and
// factory preset IDs are intentionally unaffected. All 80 models map here.
namespace pedal {
struct PedalFamilyColour {
    std::string_view family;
    std::uint32_t rgb;
};
inline constexpr std::array<PedalFamilyColour,13> kPedalFamilyColours{{
    {"Overdrive",0x31502Du},
    {"Distortion",0x114632u},
    {"Fuzz",0x60333Cu},
    {"Boost / Preamp",0x613E27u},
    {"Compressor",0x60443Cu},
    {"EQ / Filter",0x4A522Cu},
    {"Wah / Envelope",0x653A16u},
    {"Modulation",0x5E4B08u},
    {"Tremolo",0x6B4610u},
    {"Delay / Echo",0x5C3D2Du},
    {"Reverb",0x544C40u},
    {"Pitch / Octave",0x49416Bu},
    {"Experimental",0x20535Bu}
}};

inline constexpr std::uint32_t familyPedalRGB(std::string_view family) noexcept {
    for(const auto& entry:kPedalFamilyColours)
        if(entry.family==family)return entry.rgb;
    return 0x283E55u; // dark navy fallback for unknown families
}
inline constexpr std::uint32_t scalePedalRGB(std::uint32_t colour,int percent) noexcept {
    auto component=[percent](std::uint32_t value) constexpr -> std::uint32_t {
        return std::min(255u,(value*std::uint32_t(percent)+50u)/100u);
    };
    const auto r=component((colour>>16u)&255u);
    const auto g=component((colour>>8u)&255u);
    const auto b=component(colour&255u);
    return (r<<16u)|(g<<8u)|b;
}
// Five subtle finish variations within each family, reproducible for every
// preset, host and frame. No allocations, globals or change of model IDs.
inline std::uint32_t pedalEnclosureARGB(Model model) noexcept {
    if(model==Model::Empty)return 0xff21364bu;
    const int id=std::clamp(int(model),1,kModelCount-1);
    constexpr std::array<int,5> finishes{{92,96,100,104,107}};
    const int finish=finishes[(std::uint32_t(id)*17u+std::uint32_t(id)/5u)%finishes.size()];
    return 0xff000000u|scalePedalRGB(familyPedalRGB(info(Model(id)).family),finish);
}
// Precise gradient brightness makes contrast deterministic across OSes.
// Gold (#F3D59A) and cream (#F3E7CB) labels remain above WCAG 4.5:1
// at the LIGHTEST endpoint, without moving, hiding or recolouring text.
inline std::uint32_t pedalEnclosureTopARGB(Model model) noexcept {
    return 0xff000000u|scalePedalRGB(pedalEnclosureARGB(model)&0x00ffffffu,110);
}
inline std::uint32_t pedalEnclosureBottomARGB(Model model) noexcept {
    return 0xff000000u|scalePedalRGB(pedalEnclosureARGB(model)&0x00ffffffu,78);
}
} // namespace pedal
