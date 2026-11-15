#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <string_view>
#include <vector>

namespace pedal {
constexpr int kSlots = 8;
constexpr int kControls = 4;
constexpr int kMaxChannels = 2;

enum class Model : int {
    Empty = 0, VerdantDrive, GoldenArc, CopperSoul,
    OrangeVoltage, SteelFang, VelvetFury, SiliconStorm,
    CrystalLift, BritishSpark, VelvetSustain, SculptEQ,
    VocalWah, AzureChorus, OrbitPhase, CelestialFlange,
    WarmTremolo, TapeVoyager, CrystalRepeat, GoldenPlate,
    DreamFracture,
    AmberBreaker, VintageBloom, IronHammer, CrimsonRiot, BlackReactor,
    GermaniumSun, WoollyThunder, GoldlinePreamp, OpticalSilk,
    ResonantSweep, FunkEnvelope, AnalogMirage, HarmonicPulse,
    MemoryTrail, CathedralAir, SpringGarden, OctaveCrown,
    SubmarineVoice, TimeGarden, InfiniteDrift, Count
};
constexpr int kModelCount = static_cast<int>(Model::Count);

struct ModelInfo {
    Model model;
    std::string_view name, family, era;
    uint32_t colour;
    std::array<std::string_view, kControls> controls;
    std::array<float, kControls> defaults;
};
const std::array<ModelInfo, kModelCount>& catalog() noexcept;
const ModelInfo& info(Model model) noexcept;

enum class RoutingMode : int { Series = 0, Parallel = 1 };

struct SlotSettings {
    Model model = Model::Empty;
    bool enabled = false;
    std::array<float, kControls> controls { .5f, .5f, .5f, .5f };
    float mix = 1.f;
    float levelDb = 0.f;
    int branch = 0; // Parallel routing: 0 = lane A, 1 = lane B; travels with the pedal.
};
struct Settings {
    std::array<SlotSettings, kSlots> slots {};
    float inputDb = 0.0f;
    float outputDb = 0.0f;
    float globalMix = 1.0f;
    bool globalBypass = false;
    RoutingMode routing = RoutingMode::Series;
    float parallelBlend = .5f; // 0 = A only, 1 = B only, linear unity-preserving blend.
};

class Engine {
public:
    void prepare(double sampleRate, int channels);
    void reset() noexcept;
    // All memory allocated by prepare(). process() makes no allocations or locks.
    void process(float* const* audio, int channels, int numSamples,
                 const Settings& settings) noexcept;
    double sampleRate() const noexcept { return sr_; }
private:
    struct DelayLine {
        std::vector<float> data;
        int index = 0;
        float read(float samples, uint64_t age) const noexcept;
        void write(float value) noexcept;
    };
    struct Comb {
        std::vector<float> data;
        int index = 0;
        float filter = 0.0f;
    };
    struct Allpass {
        std::vector<float> data;
        int index = 0;
    };
    struct ChannelState {
        DelayLine delay;
        std::array<Comb, 4> comb;
        std::array<Allpass, 2> allpass;
        float lp1 = 0.f, lp2 = 0.f, lp3 = 0.f;
        float env = 0.f, feedback = 0.f, phase = 0.f;
        float phaser[4] {}, phaserFeedback = 0.f;
        float sub = 1.f, prevInput = 0.f;
        uint64_t age = 0;
        void resetScalars() noexcept;
    };
    struct SlotState {
        std::array<ChannelState, kMaxChannels> channel;
        Model lastModel = Model::Empty;
        float wet = 0.f;
        float smoothedMix = 1.f;
        float smoothedLevel = 1.f;
        std::array<float, kControls> smoothed { .5f, .5f, .5f, .5f };
    };
    std::array<SlotState, kSlots> states_;
    double sr_ = 48000.0;
    int preparedChannels_ = 2;
    float smoothedInputGain_ = 1.f;
    float smoothedOutputGain_ = 1.f;
    float smoothedMix_ = 1.f;
    float smoothedGlobalOn_ = 1.f;
    bool masterInitialized_ = false;
    float smoothedParallelBlend_ = .5f;
    RoutingMode lastRouting_ = RoutingMode::Series;
    std::array<int,kSlots> lastBranches_{};
    std::array<float,kMaxChannels> previousOutput_{};
    int routingFadeSamples_ = 0;
    int routingFadeLength_ = 1;
    float processOne(Model model, ChannelState& state, int ch,
                     float sample, const std::array<float, kControls>& controls) noexcept;
};
} // namespace pedal
