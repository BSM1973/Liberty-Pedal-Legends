#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

class PedalLegendsAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer {
public:
    explicit PedalLegendsAudioProcessorEditor(PedalLegendsAudioProcessor&);
    ~PedalLegendsAudioProcessorEditor() override=default;
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
private:
    PedalLegendsAudioProcessor& processor;
    int selectedSlot=0;
    juce::Rectangle<int> cards[pedal::kSlots];
    juce::Label logo,tagline,presetName,selectedName,slotStatus;
    juce::TextButton browserButton{"PRESETS"},previousButton{"<"},nextButton{">"},
                     bypassButton{"GLOBAL BYPASS"},leftButton{"MOVE LEFT"},
                     rightButton{"MOVE RIGHT"},resetButton{"RESET PEDAL"};
    juce::ComboBox modelSelector;
    juce::ToggleButton enabledButton{"PEDAL ON"};
    std::array<juce::Slider,pedal::kControls> knobs;
    std::array<juce::Label,pedal::kControls> knobLabels;
    juce::Slider input,output,mix;
    juce::Label inLabel,outLabel,mixLabel;
    void timerCallback() override;
    void selectSlot(int slot);
    void showBrowser();
    void setButtonStyle(juce::TextButton&);
    static juce::Colour colourFor(pedal::Model);
    static juce::Colour contrast(juce::Colour);
    void drawPedal(juce::Graphics&,juce::Rectangle<int>,int,const pedal::SlotSettings&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalLegendsAudioProcessorEditor)
};
