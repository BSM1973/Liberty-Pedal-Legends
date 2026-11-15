#pragma once
#include "PedalDSP.h"
#include <array>
#include <string>
#include <string_view>

namespace pedal {
constexpr int kSeriesPresetCount = 100;
constexpr int kParallelPresetCount = 100;
constexpr int kFactoryPresetCount = kSeriesPresetCount + kParallelPresetCount;
constexpr int kPresetCategoryCount = 10;
const std::array<std::string_view, kPresetCategoryCount>& presetCategories() noexcept;
struct FactoryPreset {
    std::string name;
    std::string category;
    Settings settings;
};
// Owned for the lifetime of the process; no allocations in audio processing.
const std::array<FactoryPreset, kFactoryPresetCount>& factoryPresets();
} // namespace pedal
