#include "FactoryPresets.h"
#include <algorithm>
#include <cmath>

namespace pedal {
namespace {
using Row = std::array<std::string_view,10>;
constexpr std::array<std::string_view,10> categories{{
 "Vintage Blues","Classic Rock","Hard Rock","Heavy Metal","Modern Metal",
 "Funk and Clean","Jazz and Warm","Ambient and Cinematic",
 "Psychedelic and Experimental","Studio Essentials"
}};
constexpr std::array<Row,10> names{{
 {{"1975 Vintage Soul","Texas Copper Blues","Slow Train Ember","Delta Valve Glow","Midnight 1977","Warm Bottle Drive","Southern Crossroads","Olive Green Shuffle","Sunset Blues Line","Velvet Room Blues"}},
 {{"British Crunch Legacy","Classic Arena Lead","Copper Stack 78","Twin Guitar Hero","Stadium Golden Hour","Highway Valve","Bright Riff Machine","Seventies Power Chords","Vintage Anthem","Legacy Solo Voice"}},
 {{"Iron Bridge Rhythm","Crimson Stage Lead","Garage Thunder","Vintage Iron Solo","Riot Overdrive Chain","Power House Chorus","Steel Sunset","Rebel Guitar Wall","Raw Valve Assault","Hard Rock Majesty"}},
 {{"Eighties Heavy Machine","Metal Hammer Rhythm","Voltage Iron Crunch","Black Leather Lead","Heavy Stack Pulse","Crimson Iron Wall","Chug and Sustain","Old School Metal","Steel Crown Solo","Analog Metal Riff"}},
 {{"Modern Precision Rhythm","Reactor Tight Lead","Progressive Low End","Surgical Drop Tuning","Steel Modern Punch","Cinematic Modern Metal","Fast Attack Metal","Technical Guitar Wall","Dark Precision","Ultra Definition Lead"}},
 {{"Boutique Clean Glow","Funky Envelope Motion","Disco Copper Lines","Bright Rhythm Studio","Liquid Funk Bass","Velvet Clean Chorus","Organic Chank","Filtered Clean Groove","Electric Chic","Crystal Sparkle Funk"}},
 {{"Warm Jazz Studio","Vintage Hollowbody","Smooth Copper Night","Silky Combo Clean","After Hours Octaves","Warm Guitar Club","Quiet Studio Velvet","Soft Touch Tone","Vintage Jazz Ensemble","Nocturne Jazz Guitar"}},
 {{"Cathedral Guitar","Infinite Shimmer Trails","Ambient Copper Horizon","Cloud Nine Guitar","Slow Motion Cinema","Lunar Soundscape","Celestial Atmosphere","Dreamy Stereo Fields","Warm Drone Garden","Endless Evening"}},
 {{"Analog Psychedelia","Vintage Tape Dream","Dream Fracture Lab","Swirling Liquid Phase","Woolly Acid Clouds","Fuzz Space Odyssey","Pulsing Tape Orbit","Unusual Delay Garden","Sixties Freakout","Midnight Psychedelia"}},
 {{"Studio Lead Polish","Clean Recording Chain","Guitar Mix Finisher","Polished Funk Lines","Controlled Sustain","Stereo Room Detail","Warm Rhythm Comp","Wide Boutique Space","Transient Guitar Lift","All Purpose Guitar"}}
}};
// Each category combines a different set of algorithms. Individual patches use
// two independently chosen texture options plus category-specific core stages.
constexpr std::array<std::array<Model,8>,10> palettes{{
 {{Model::CrystalLift,Model::VerdantDrive,Model::AmberBreaker,Model::GermaniumSun,Model::VelvetSustain,Model::TapeVoyager,Model::SpringGarden,Model::OpticalSilk}},
 {{Model::GoldlinePreamp,Model::VintageBloom,Model::OrangeVoltage,Model::CrimsonRiot,Model::OrbitPhase,Model::MemoryTrail,Model::GoldenPlate,Model::CrystalLift}},
 {{Model::BritishSpark,Model::SteelFang,Model::IronHammer,Model::CrimsonRiot,Model::AzureChorus,Model::CrystalRepeat,Model::SpringGarden,Model::VelvetSustain}},
 {{Model::CrystalLift,Model::IronHammer,Model::OrangeVoltage,Model::BlackReactor,Model::SculptEQ,Model::MemoryTrail,Model::CathedralAir,Model::VelvetSustain}},
 {{Model::GoldlinePreamp,Model::BlackReactor,Model::SteelFang,Model::IronHammer,Model::SculptEQ,Model::CrystalRepeat,Model::CathedralAir,Model::OpticalSilk}},
 {{Model::OpticalSilk,Model::CrystalLift,Model::FunkEnvelope,Model::VocalWah,Model::AnalogMirage,Model::AzureChorus,Model::GoldenPlate,Model::SculptEQ}},
 {{Model::OpticalSilk,Model::GoldlinePreamp,Model::GoldenArc,Model::VintageBloom,Model::AnalogMirage,Model::SpringGarden,Model::GoldenPlate,Model::SculptEQ}},
 {{Model::OpticalSilk,Model::AnalogMirage,Model::TimeGarden,Model::MemoryTrail,Model::CathedralAir,Model::GoldenPlate,Model::InfiniteDrift,Model::CrystalRepeat}},
 {{Model::GermaniumSun,Model::WoollyThunder,Model::OrbitPhase,Model::HarmonicPulse,Model::DreamFracture,Model::TimeGarden,Model::InfiniteDrift,Model::SpringGarden}},
 {{Model::OpticalSilk,Model::GoldlinePreamp,Model::SculptEQ,Model::ResonantSweep,Model::AnalogMirage,Model::CrystalRepeat,Model::GoldenPlate,Model::CrystalLift}}
}};
float bound(float x) noexcept { return std::clamp(x,0.f,1.f); }
FactoryPreset make(int category,int variant) {
    FactoryPreset preset;
    preset.name=std::string(names[size_t(category)][size_t(variant)]);
    preset.category=std::string(categories[size_t(category)]);
    Settings& chain=preset.settings;
    chain.globalMix=1.f;
    chain.inputDb=(category==3||category==4) ? -4.f : (category==6 ? -1.f:0.f);
    chain.outputDb=(category==3||category==4) ? -2.f : 0.f;
    // Put five to seven distinct processors in real audio slots. Two banks of
    // rotating effect choices produce genuinely distinct serial chains.
    const auto& family=palettes[size_t(category)];
    const int count=5+(variant%3);
    for(int slot=0;slot<count;++slot) {
        int selected=slot;
        if(slot==1) selected=1+(variant%3);
        if(slot==2) selected=2+((variant/2)%3);
        if(slot==3) selected=3+((variant/3)%3);
        if(slot==count-1) selected=6+(variant%2);
        selected=std::min(7,selected);
        const Model model=family[size_t(selected)];
        SlotSettings& stage=chain.slots[size_t(slot)];
        stage.model=model;
        stage.enabled=true;
        stage.branch=slot>=4?1:0;
        stage.controls=info(model).defaults;
        const float rotation=float((variant*7+slot*5+category*3)%17)/16.f;
        const float delta=(rotation-.5f)*.33f;
        stage.controls[0]=bound(stage.controls[0]+delta);
        stage.controls[1]=bound(stage.controls[1]-delta*.8f);
        stage.controls[2]=bound(stage.controls[2]+delta*.4f);
        // Time and ambience levels stay conservative for guitar usability.
        if(model==Model::CathedralAir||model==Model::GoldenPlate||model==Model::SpringGarden)
            stage.controls[3]=bound(.22f+.25f*rotation);
        if(model==Model::MemoryTrail||model==Model::CrystalRepeat||model==Model::TapeVoyager)
            stage.controls[3]=bound(.22f+.18f*rotation);
    }
    return preset;
}
}
const std::array<std::string_view,kPresetCategoryCount>& presetCategories() noexcept { return categories; }
const std::array<FactoryPreset,kFactoryPresetCount>& factoryPresets() {
    static const auto presets=[] {
        std::array<FactoryPreset,kFactoryPresetCount> result;
        for(int cat=0;cat<10;++cat)
            for(int variant=0;variant<10;++variant)
                result[size_t(cat*10+variant)]=make(cat,variant);
        // A real, playable welcome chain matching the six populated slots in
        // the approved eight-slot pedalboard layout. The remaining 99 presets
        // are generated as before; the serial bank remains exactly 100.
        auto& welcome=result[0].settings;
        constexpr Model welcomeModels[]={Model::OpticalSilk,Model::VerdantDrive,
            Model::SculptEQ,Model::AzureChorus,Model::TapeVoyager,Model::GoldenPlate};
        for(int slot=0;slot<kSlots;++slot) {
            auto& stage=welcome.slots[size_t(slot)];
            if(slot<6) {
                stage.model=welcomeModels[slot];
                stage.enabled=slot!=3; // chorus initially bypassed like the reference
                stage.controls=info(stage.model).defaults;
            } else stage=SlotSettings{};
            stage.branch=slot>=4?1:0;
        }
        // New bank of 100 true parallel presets. The original 100 patches are
        // untouched, with exactly the same indices and processing parameters.
        static constexpr const char* variantNames[10]={
            "Dual Paths", "Split Focus", "Layered Echo", "Two Worlds", "Stereo Cloud",
            "Clean + Drive", "Branch Fusion", "Parallel Stage", "Twin Tones", "Studio Split"};
        for(int cat=0;cat<10;++cat) for(int variant=0;variant<10;++variant) {
            const int baseIndex=cat*10+variant;
            auto copy=result[size_t(baseIndex)];
            copy.name += " - ";
            copy.name += variantNames[variant];
            copy.settings.routing=RoutingMode::Parallel;
            copy.settings.parallelBlend=std::clamp(.25f+.06f*float(variant)+.025f*float(cat%3),.18f,.85f);
            int activeA=0,activeB=0;
            for(int slot=0;slot<kSlots;++slot) {
                auto& stage=copy.settings.slots[size_t(slot)];
                stage.branch=(slot+variant/2+cat)%2;
                // Different paths receive different character and effects mixes.
                if(stage.model!=Model::Empty && stage.enabled) {
                    if(stage.branch==0) ++activeA; else ++activeB;
                    stage.mix=stage.branch==0?.95f:.83f;
                    stage.levelDb=stage.branch==0?-1.2f:-2.4f;
                }
            }
            // Preserve two audible lanes even for unusually sparse future patches.
            if(activeA==0||activeB==0) {
                for(int slot=0;slot<kSlots;++slot) {
                    auto& stage=copy.settings.slots[size_t(slot)];
                    if(stage.enabled && stage.model!=Model::Empty) {stage.branch=activeA==0?0:1;break;}
                }
            }
            result[size_t(kSeriesPresetCount+baseIndex)]=std::move(copy);
        }
        return result;
    }();
    return presets;
}
}
