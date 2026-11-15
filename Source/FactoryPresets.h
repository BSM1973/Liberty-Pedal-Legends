#pragma once
#include "PedalDSP.h"
#include <array>
#include <string>
#include <string_view>

namespace pedal {
// Immutable indices of the original 200 factory presets, for saved DAW sessions.
constexpr int kOriginalSeriesPresetCount = 100;
constexpr int kOriginalParallelPresetCount = 100;
constexpr int kOriginalPresetCount = kOriginalSeriesPresetCount + kOriginalParallelPresetCount;
constexpr int kAddedSeriesPresetCount = 100;
constexpr int kAddedParallelPresetCount = 100;
constexpr int kSeriesPresetCount = kOriginalSeriesPresetCount + kAddedSeriesPresetCount;
constexpr int kParallelPresetCount = kOriginalParallelPresetCount + kAddedParallelPresetCount;
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
