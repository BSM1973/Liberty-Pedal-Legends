#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>
#include <functional>
#include <memory>
#include <vector>

// The three screens share a single responsive coordinate system and one
// interactive skin. No effect audio processing is performed by the editor.
class PedalLegendsAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer {
public:
    explicit PedalLegendsAudioProcessorEditor(PedalLegendsAudioProcessor&);
    ~PedalLegendsAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    bool keyPressed(const juce::KeyPress&) override;
private:
    enum class Screen { Board, Editor, History, Presets, Library };
    struct HotSpot {
        juce::Rectangle<float> bounds;
        std::function<void()> click;
        std::function<void(float)> drag;
        float value=0.f;
    };
    PedalLegendsAudioProcessor& processor;
    Screen screen=Screen::Board, returnScreen=Screen::Board;
    int selectedSlot=0, historyModel=1, historyTab=0, browserCategory=-1, libraryCategory=-1;
    int browserOffset=0, libraryOffset=0;
    float scale=1.f, xOffset=0.f, yOffset=0.f;
    std::vector<HotSpot> hotspots;
    std::function<void(float)> dragCallback;
    float dragStart=0.f, dragY=0.f;
    juce::TextEditor search;
    juce::ComboBox languageSelector;
    int currentLanguage=0;
    juce::String localized(const juce::String&) const;
    void languageChanged();
    std::unique_ptr<juce::FileChooser> fileChooser;
    pedal::Settings snapshotA{},snapshotB{};
    bool viewingB=false;
    bool gateA=false,gateB=false;
    float thresholdA=-66.f,thresholdB=-66.f;
    pedal::SlotSettings clipboardSlot{};
    bool hasClipboard=false;
    juce::String lastSavedPreset;
    static constexpr float canvasWidth=1600.f,canvasHeight=930.f;
    void timerCallback() override;
    void updateSearch();
    void setScreen(Screen next);
    void selectSlot(int slot);
    void openHistory(int slot);
    void toggleAB();
    void savePreset();
    void openUserPreset();
    void showModel(int model);
    juce::Point<float> logicalPoint(juce::Point<int>) const;
    void handleClick(juce::Point<float>);
    void button(juce::Graphics&,juce::Rectangle<float>,const juce::String&,
                std::function<void()>,bool filled=false,bool highlighted=false,float size=13.f);
    void hit(juce::Rectangle<float>,std::function<void()>);
    void drag(juce::Rectangle<float>,float,std::function<void(float)>);
    void drawLabel(juce::Graphics&,const juce::String&,juce::Rectangle<float>,float,
                   juce::Colour,bool serif=false,juce::Justification=juce::Justification::centredLeft,bool bold=false);
    void drawKnob(juce::Graphics&,float,float,float,float,const juce::String&,
                  const juce::String&,std::function<void(float)>,bool darkBackground=false);
    void drawBackplate(juce::Graphics&);
    void drawBoard(juce::Graphics&);
    void drawPedal(juce::Graphics&,juce::Rectangle<float>,pedal::Model,const pedal::SlotSettings&,bool large=false);
    void drawEditor(juce::Graphics&);
    void drawHistory(juce::Graphics&);
    void drawPresets(juce::Graphics&);
    void drawLibrary(juce::Graphics&);
    void drawOverlay(juce::Graphics&,juce::Rectangle<float>,const juce::String&,bool drawClose=true);
    void drawMeter(juce::Graphics&,juce::Rectangle<float>,float,const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalLegendsAudioProcessorEditor)
};
