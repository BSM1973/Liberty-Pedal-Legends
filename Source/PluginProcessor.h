#pragma once
#include <JuceHeader.h>
#include "PedalDSP.h"
#include "FactoryPresets.h"
#include "MidiLearn.h"
#include <array>
#include <atomic>
#include <algorithm>

class PedalLegendsAudioProcessor final : public juce::AudioProcessor,
                                         private juce::Timer {
public:
    PedalLegendsAudioProcessor();
    ~PedalLegendsAudioProcessor() override = default;
    void prepareToPlay(double sampleRate,int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "PEDAL LEGENDS"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.1; }
    int getNumPrograms() override { return pedal::kFactoryPresetCount; }
    int getCurrentProgram() override { return std::clamp(factoryPresetIndex.load(),0,pedal::kFactoryPresetCount-1); }
    void setCurrentProgram(int index) override { loadFactoryPreset(index); }
    const juce::String getProgramName(int index) override;
    void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    static juce::String parameterID(int slot,int component);
    pedal::Settings snapshot() const noexcept;
    void loadFactoryPreset(int index);
    void setModel(int slot,pedal::Model model);
    void resetSlot(int slot);
    void moveSlot(int from,int to);
    void changeSlotEnabled(int slot,bool value);
    void setControl(int slot,int control,float value);
    void setSlotMix(int slot,float value);
    void setRoutingMode(pedal::RoutingMode mode);
    void setRoutingBranch(int slot,int branch);
    void setParallelBlend(float blend);
    void setSlotLevel(int slot,float db);
    void setGlobal(int which,float value);
    void restoreSettings(const pedal::Settings& settings);
    void setGateEnabled(bool enabled);
    void setGateThreshold(float db);
    bool gateEnabled() const noexcept;
    int getUILanguage() const noexcept {return uiLanguage.load(std::memory_order_relaxed);}
    void setUILanguage(int value) noexcept {uiLanguage.store(std::clamp(value,0,2),std::memory_order_relaxed);}
    float gateThreshold() const noexcept;
    void setParameterValue(const juce::String& id,float realValue);
    bool isPresetModified() const noexcept { return presetModified.load(); }
    int selectedFactoryPreset() const noexcept { return factoryPresetIndex.load(); }
    float inputPeak() const noexcept { return inPeak.load(); }
    float outputPeak() const noexcept { return outPeak.load(); }
    // MIDI Learn is linked to stable slot/global parameter IDs, not to model names.
    void beginMidiLearn(int target) noexcept;
    void cancelMidiLearn() noexcept;
    void clearMidiLearn(int target) noexcept;
    int midiLearnTarget() const noexcept {return waitingMidiLearn.load(std::memory_order_acquire);}
    int midiControllerFor(int target) const noexcept;
private:
    void timerCallback() override;
    void receiveMidiControllers(const juce::MidiBuffer&) noexcept;
    static juce::String midiParameterID(int target);
    pedal::Engine engine;
    struct SlotRefs {
        std::atomic<float>* model=nullptr;
        std::atomic<float>* enabled=nullptr;
        std::atomic<float>* mix=nullptr;
        std::atomic<float>* level=nullptr;
        std::atomic<float>* branch=nullptr;
        std::array<std::atomic<float>*,pedal::kControls> controls{};
    };
    std::array<SlotRefs,pedal::kSlots> refs{};
    std::atomic<float>* inputGain=nullptr;
    std::atomic<float>* outputGain=nullptr;
    std::atomic<float>* mix=nullptr;
    std::atomic<float>* routingMode=nullptr;
    std::atomic<float>* parallelBlendParam=nullptr;
    std::atomic<float>* bypass=nullptr;
    std::atomic<float>* noiseGate=nullptr;
    std::atomic<float>* noiseGateThreshold=nullptr;
    std::array<float,2> gateEnvelope{{0.f,0.f}};
    float gateGain=1.f;
    std::atomic<int> uiLanguage{0}; // 0 Français, 1 English, 2 Español
    std::atomic<int> factoryPresetIndex{0};
    std::atomic<bool> presetModified{false};
    std::atomic<float> inPeak{0.f},outPeak{0.f};
    // Audio-thread producer / message-thread consumer. No UI callbacks, locks
    // or allocations in processBlock; parameter host notifications are made
    // on the message thread even while the editor window is closed.
    std::array<std::atomic<int>,pedal::midi::kTargetCount> midiCC{};
    std::array<std::atomic<int>,pedal::midi::kTargetCount> midiPending{};
    std::array<juce::RangedAudioParameter*,pedal::midi::kTargetCount> midiParameters{};
    std::atomic<int> waitingMidiLearn{-1};
    void applySettings(const pedal::Settings& s);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalLegendsAudioProcessor)
};
