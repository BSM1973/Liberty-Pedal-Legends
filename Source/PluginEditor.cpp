#include "PluginEditor.h"
#include <cmath>
#include <vector>
#include <functional>
#include <algorithm>

namespace {
const juce::Colour champagne{0xffc3b7a3},ivory{0xffe7dfc9},olive{0xff374438},
                   copper{0xffb47748},graphite{0xff282824},amber{0xffe5a744};

class BrowserPanel final:public juce::Component,private juce::ListBoxModel {
public:
    explicit BrowserPanel(PedalLegendsAudioProcessor& p):processor(p) {
        title.setText("FACTORY PRESETS — 100",juce::dontSendNotification);
        title.setColour(juce::Label::textColourId,ivory);
        title.setFont(juce::Font(juce::FontOptions(18.f,juce::Font::bold)));
        addAndMakeVisible(title);
        search.setTextToShowWhenEmpty("Search presets...",champagne.withAlpha(.6f));
        search.setColour(juce::TextEditor::backgroundColourId,graphite);
        search.setColour(juce::TextEditor::textColourId,ivory);
        search.setColour(juce::TextEditor::outlineColourId,copper);
        search.onTextChange=[this]{updateFilter();};
        addAndMakeVisible(search);
        category.addItem("All categories",1);
        for(int i=0;i<pedal::kPresetCategoryCount;++i)
            category.addItem(juce::String(pedal::presetCategories()[size_t(i)].data()),i+2);
        category.setSelectedId(1,juce::dontSendNotification);
        category.onChange=[this]{updateFilter();};
        addAndMakeVisible(category);
        list.setModel(this);
        list.setRowHeight(38);
        list.setColour(juce::ListBox::backgroundColourId,olive.darker(.18f));
        list.setColour(juce::ListBox::outlineColourId,copper);
        addAndMakeVisible(list);
        updateFilter();
    }
    ~BrowserPanel() override {list.setModel(nullptr);}
    void paint(juce::Graphics& g) override {g.fillAll(olive);}
    void resized() override {
        auto b=getLocalBounds().reduced(14);
        title.setBounds(b.removeFromTop(36));
        b.removeFromTop(7);
        search.setBounds(b.removeFromTop(36));
        b.removeFromTop(7);
        category.setBounds(b.removeFromTop(34));
        b.removeFromTop(9);
        list.setBounds(b);
    }
private:
    PedalLegendsAudioProcessor& processor;
    juce::Label title;
    juce::TextEditor search;
    juce::ComboBox category;
    juce::ListBox list;
    std::vector<int> indices;
    int getNumRows() override {return int(indices.size());}
    void updateFilter() {
        indices.clear();
        const auto term=search.getText().trim().toLowerCase();
        const int cat=category.getSelectedId()-2;
        for(int i=0;i<pedal::kFactoryPresetCount;++i) {
            const auto& p=pedal::factoryPresets()[size_t(i)];
            if(cat>=0&&p.category!=std::string(pedal::presetCategories()[size_t(cat)]))continue;
            if(!term.isEmpty()&&!juce::String(p.name).containsIgnoreCase(term))continue;
            indices.push_back(i);
        }
        list.updateContent();
        list.repaint();
    }
    void paintListBoxItem(int row,juce::Graphics& g,int w,int h,bool selected) override {
        if(row<0||row>=int(indices.size()))return;
        const int actual=indices[size_t(row)];
        const auto& p=pedal::factoryPresets()[size_t(actual)];
        if(selected||actual==processor.selectedFactoryPreset())
            {g.setColour(copper.withAlpha(selected?.36f:.16f));g.fillRect(0,0,w,h);}
        g.setColour(champagne);
        g.setFont(12.f);
        g.drawText(juce::String(actual+1).paddedLeft('0',3),12,0,38,h,juce::Justification::centredLeft);
        g.setColour(ivory);
        g.setFont(14.f);
        g.drawText(juce::String(p.name),58,0,w-65,h,juce::Justification::centredLeft,true);
    }
    void selectedRowsChanged(int row) override {
        if(row>=0&&row<int(indices.size()))processor.loadFactoryPreset(indices[size_t(row)]);
        list.repaint();
    }
};

void setupLabel(juce::Label& label,juce::Colour colour,float fontSize,juce::Justification just=juce::Justification::centredLeft) {
    label.setColour(juce::Label::textColourId,colour);
    label.setFont(juce::Font(juce::FontOptions(fontSize)));
    label.setJustificationType(just);
}
void setupKnob(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,62,22);
    s.setRange(0.,1.,.001);
    s.setColour(juce::Slider::rotarySliderFillColourId,copper);
    s.setColour(juce::Slider::rotarySliderOutlineColourId,graphite.brighter(.4f));
    s.setColour(juce::Slider::thumbColourId,ivory);
    s.setColour(juce::Slider::textBoxTextColourId,ivory);
    s.setColour(juce::Slider::textBoxBackgroundColourId,graphite);
    s.setColour(juce::Slider::textBoxOutlineColourId,olive);
}
} // namespace

PedalLegendsAudioProcessorEditor::PedalLegendsAudioProcessorEditor(PedalLegendsAudioProcessor& p)
:juce::AudioProcessorEditor(&p),processor(p) {
    setOpaque(true);
    setResizable(true,true);
    setResizeLimits(1080,760,1920,1200);
    setSize(1340,930);
    logo.setText("LIBERTY  |  PEDAL LEGENDS",juce::dontSendNotification);
    setupLabel(logo,olive,21.f);
    logo.setFont(juce::Font(juce::FontOptions(21.f,juce::Font::bold)));
    tagline.setText("VINTAGE SOUL. MODERN POWER.",juce::dontSendNotification);
    setupLabel(tagline,olive.withAlpha(.7f),11.f);
    setupLabel(presetName,olive,17.f);
    presetName.setFont(juce::Font(juce::FontOptions(17.f,juce::Font::bold)));
    setupLabel(selectedName,ivory,21.f);
    selectedName.setFont(juce::Font(juce::FontOptions(21.f,juce::Font::bold)));
    setupLabel(slotStatus,champagne,13.f);
    for(auto* c: {&logo,&tagline,&presetName,&selectedName,&slotStatus})addAndMakeVisible(*c);
    for(auto* b:{&browserButton,&previousButton,&nextButton,&bypassButton,&leftButton,&rightButton,&resetButton}) {
        setButtonStyle(*b);
        addAndMakeVisible(*b);
    }
    previousButton.onClick=[this]{processor.loadFactoryPreset((processor.selectedFactoryPreset()+99)%100);};
    nextButton.onClick=[this]{processor.loadFactoryPreset((processor.selectedFactoryPreset()+1)%100);};
    browserButton.onClick=[this]{showBrowser();};
    bypassButton.onClick=[this]{processor.setGlobal(3,bypassButton.getToggleState()?0.f:1.f);};
    leftButton.onClick=[this]{if(selectedSlot>0){processor.moveSlot(selectedSlot,selectedSlot-1);selectSlot(selectedSlot-1);}};
    rightButton.onClick=[this]{if(selectedSlot<7){processor.moveSlot(selectedSlot,selectedSlot+1);selectSlot(selectedSlot+1);}};
    resetButton.onClick=[this]{processor.resetSlot(selectedSlot);selectSlot(selectedSlot);};
    modelSelector.addItem("EMPTY",1);
    constexpr const char* families[]={"Overdrive","Distortion","Fuzz","Boost / Preamp", "Compressor",
        "EQ / Filter","Wah / Envelope","Modulation","Tremolo","Delay / Echo","Reverb",
        "Pitch / Octave","Experimental"};
    for(const auto* family:families) {
        modelSelector.addSectionHeading(juce::String(family).toUpperCase());
        for(int model=1;model<pedal::kModelCount;++model)
            if(pedal::info(pedal::Model(model)).family==family)
                modelSelector.addItem(juce::String(pedal::info(pedal::Model(model)).name.data()),model+1);
    }
    modelSelector.onChange=[this]{processor.setModel(selectedSlot,pedal::Model(modelSelector.getSelectedId()-1));selectSlot(selectedSlot);};
    addAndMakeVisible(modelSelector);
    enabledButton.setColour(juce::ToggleButton::textColourId,ivory);
    enabledButton.onClick=[this]{processor.changeSlotEnabled(selectedSlot,enabledButton.getToggleState());};
    addAndMakeVisible(enabledButton);
    for(int k=0;k<pedal::kControls;++k) {
        setupKnob(knobs[size_t(k)]);
        const int control=k;
        knobs[size_t(k)].onValueChange=[this,control]{
            processor.setControl(selectedSlot,control,float(knobs[size_t(control)].getValue()));
        };
        addAndMakeVisible(knobs[size_t(k)]);
        setupLabel(knobLabels[size_t(k)],ivory,12.f,juce::Justification::centred);
        addAndMakeVisible(knobLabels[size_t(k)]);
    }
    for(auto* k:{&input,&output,&mix}){setupKnob(*k);addAndMakeVisible(*k);}
    input.setRange(-24,24,.1);output.setRange(-24,24,.1);
    input.onValueChange=[this]{processor.setGlobal(0,float(input.getValue()));};
    output.onValueChange=[this]{processor.setGlobal(1,float(output.getValue()));};
    mix.onValueChange=[this]{processor.setGlobal(2,float(mix.getValue()));};
    for(auto* label:{&inLabel,&outLabel,&mixLabel}){setupLabel(*label,ivory,12.f,juce::Justification::centred);addAndMakeVisible(*label);}
    inLabel.setText("INPUT dB",juce::dontSendNotification);
    outLabel.setText("OUTPUT dB",juce::dontSendNotification);
    mixLabel.setText("GLOBAL MIX",juce::dontSendNotification);
    selectSlot(0);
    startTimerHz(18);
}
void PedalLegendsAudioProcessorEditor::setButtonStyle(juce::TextButton& b) {
    b.setColour(juce::TextButton::buttonColourId,olive);
    b.setColour(juce::TextButton::buttonOnColourId,copper);
    b.setColour(juce::TextButton::textColourOffId,ivory);
    b.setColour(juce::TextButton::textColourOnId,graphite);
}
juce::Colour PedalLegendsAudioProcessorEditor::colourFor(pedal::Model model) {
    return juce::Colour(0xff000000|pedal::info(model).colour);
}
juce::Colour PedalLegendsAudioProcessorEditor::contrast(juce::Colour c) {
    const float brightness=.2126f*c.getFloatRed()+.7152f*c.getFloatGreen()+.0722f*c.getFloatBlue();
    return brightness>.58f?graphite:ivory;
}
void PedalLegendsAudioProcessorEditor::drawPedal(juce::Graphics& g,juce::Rectangle<int> rect,int index,const pedal::SlotSettings& settings) {
    const bool active=settings.enabled&&settings.model!=pedal::Model::Empty;
    const bool selected=index==selectedSlot;
    auto bounds=rect.toFloat();
    g.setColour(graphite.withAlpha(.23f));
    g.fillRoundedRectangle(bounds.translated(2.f,5.f),13.f);
    g.setGradientFill(juce::ColourGradient(champagne.brighter(.15f),bounds.getTopLeft(),
                                           champagne.darker(.12f),bounds.getBottomRight(),false));
    g.fillRoundedRectangle(bounds,11.f);
    g.setColour(selected?copper:olive.withAlpha(.35f));
    g.drawRoundedRectangle(bounds.reduced(1),11.f,selected?3.f:1.f);
    auto enclosure=bounds.reduced(15.f,10.f);
    enclosure.removeFromTop(15.f);
    const auto colour=colourFor(settings.model);
    const juce::Colour text=contrast(colour);
    g.setGradientFill(juce::ColourGradient(colour.brighter(.12f),enclosure.getTopLeft(),
                                           colour.darker(.22f),enclosure.getBottomRight(),false));
    g.fillRoundedRectangle(enclosure,11.f);
    g.setColour(graphite.withAlpha(.55f));
    g.drawRoundedRectangle(enclosure.reduced(.7f),10.f,1.8f);
    // Original machined screw heads, no trademark imagery.
    for(float x:{enclosure.getX()+13.f,enclosure.getRight()-13.f})
        for(float y:{enclosure.getY()+12.f,enclosure.getBottom()-12.f}) {
            g.setColour(graphite.withAlpha(.6f));g.fillEllipse(x-3.4f,y-3.4f,6.8f,6.8f);
            g.setColour(ivory.withAlpha(.6f));g.drawLine(x-2.f,y,x+2.f,y,.8f);
        }
    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(15.f,juce::Font::bold)));
    const auto name=juce::String(pedal::info(settings.model).name.data());
    g.drawFittedText(name,enclosure.toNearestInt().reduced(12).removeFromTop(31),juce::Justification::centred,1);
    g.setFont(11.f);
    g.drawText(juce::String(index+1).paddedLeft('0',2),rect.getX()+11,rect.getY()+3,38,20,juce::Justification::centredLeft);
    const float knobY=enclosure.getY()+enclosure.getHeight()*.47f;
    const float knobR=std::clamp(enclosure.getWidth()/17.f,10.f,17.f);
    for(int k=0;k<pedal::kControls;++k) {
        const float knobX=enclosure.getX()+enclosure.getWidth()*(.15f+float(k)*.233f);
        juce::Rectangle<float> circle{knobX-knobR,knobY-knobR,knobR*2,knobR*2};
        g.setColour(graphite.withAlpha(.52f));g.fillEllipse(circle.translated(1.f,2.f));
        g.setGradientFill(juce::ColourGradient(ivory.brighter(.1f),circle.getTopLeft(),graphite.brighter(.5f),circle.getBottomRight(),false));
        g.fillEllipse(circle.reduced(1.6f));
        const float value=settings.controls[size_t(k)];
        const float theta=(.75f+value*1.5f)*juce::MathConstants<float>::pi;
        g.setColour(graphite);
        g.drawLine(knobX,knobY,knobX+std::cos(theta)*knobR*.68f,knobY+std::sin(theta)*knobR*.68f,2.f);
        g.setColour(text);
        g.setFont(9.5f);
        g.drawFittedText(juce::String(pedal::info(settings.model).controls[size_t(k)].data()),
                         juce::Rectangle<int>(int(knobX-27.f),int(knobY+knobR+5.f),54,17),juce::Justification::centred,1);
    }
    const float switchX=enclosure.getCentreX(),switchY=enclosure.getBottom()-24.f;
    g.setColour(graphite.withAlpha(.7f));g.fillEllipse(switchX-12.f,switchY-12.f,24.f,24.f);
    g.setColour(ivory);g.fillEllipse(switchX-9.f,switchY-9.f,18.f,18.f);
    g.setColour(active?amber:graphite.withAlpha(.75f));
    g.fillEllipse(enclosure.getRight()-29.f,enclosure.getY()+41.f,10.f,10.f);
    g.setColour(active?amber.withAlpha(.7f):graphite.withAlpha(.5f));
    g.drawEllipse(enclosure.getRight()-31.f,enclosure.getY()+39.f,14.f,14.f,1.5f);
    if(!active) {
        g.setColour(graphite.withAlpha(.44f));g.fillRoundedRectangle(enclosure,11.f);
        g.setColour(ivory);g.setFont(juce::Font(juce::FontOptions(15.f,juce::Font::bold)));
        g.drawText("BYPASSED",enclosure.toNearestInt().withY(int(enclosure.getBottom()-52.f)).withHeight(22),juce::Justification::centred);
    }
}
void PedalLegendsAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(graphite);
    auto top=getLocalBounds().removeFromTop(88).toFloat();
    g.setGradientFill(juce::ColourGradient(ivory,top.getTopLeft(),champagne,top.getBottomRight(),false));
    g.fillRect(top);
    g.setColour(copper);g.fillRect(0,86,getWidth(),3);
    auto bottom=juce::Rectangle<int>(0,getHeight()-304,getWidth(),304);
    g.setColour(olive);g.fillRect(bottom);
    g.setColour(copper);g.fillRect(bottom.removeFromTop(3));
    const auto cfg=processor.snapshot();
    // Serpentine signal flow: 1→2→3→4 ↓ 5←6←7←8, preserving DSP order 1..8.
    g.setColour(copper.withAlpha(.9f));
    for(int s=0;s<3;++s) {
        const float x1=float(cards[s].getRight()+2),x2=float(cards[s+1].getX()-2);
        const float y=float(cards[s].getCentreY());
        g.drawArrow({x1,y,x2,y},2.f,8.f,7.f);
    }
    for(int s=4;s<7;++s) {
        const float x1=float(cards[s].getX()-2),x2=float(cards[s+1].getRight()+2);
        const float y=float(cards[s].getCentreY());
        g.drawArrow({x1,y,x2,y},2.f,8.f,7.f);
    }
    const float cableX=float(getWidth()-8);
    const float upper=float(cards[3].getCentreY());
    const float lower=float(cards[4].getCentreY());
    g.drawLine(float(cards[3].getRight()),upper,cableX,upper,2.f);
    g.drawLine(cableX,upper,cableX,lower,2.f);
    g.drawArrow({cableX,lower,float(cards[4].getRight()),lower},2.f,8.f,7.f);
    for(int s=0;s<pedal::kSlots;++s)drawPedal(g,cards[s],s,cfg.slots[size_t(s)]);
    // Actual chain order numbers are drawn beside the pedals; visual line follows row 1→2→3→4 then 5→6→7→8.
    const int meterWidth=61;
    const auto drawMeter=[&](int x,float level,const juce::String& label){
        g.setColour(olive);g.fillRoundedRectangle(float(x),58.f,float(meterWidth),9.f,3.f);
        g.setColour(level>.9f?juce::Colours::orangered:amber);
        g.fillRoundedRectangle(float(x),58.f,float(meterWidth)*std::clamp(level,0.f,1.f),9.f,3.f);
        g.setFont(10.f);g.setColour(olive);g.drawText(label,x,70,meterWidth,12,juce::Justification::centred);
    };
    drawMeter(getWidth()-145,processor.inputPeak(),"IN PEAK");
    drawMeter(getWidth()-75,processor.outputPeak(),"OUT PEAK");
    g.setColour(ivory.withAlpha(.4f));
    g.setFont(12.f);
    g.drawText("CLICK A PEDAL TO EDIT  •  CLICK ITS STATUS LED TO BYPASS",22,getHeight()-326,getWidth()-44,17,juce::Justification::centredLeft);
}
void PedalLegendsAudioProcessorEditor::resized() {
    const int w=getWidth(),h=getHeight();
    logo.setBounds(22,9,340,35);
    tagline.setBounds(23,48,305,20);
    presetName.setBounds(370,14,std::max(200,w-870),26);
    previousButton.setBounds(w-507,50,38,30);
    nextButton.setBounds(w-466,50,38,30);
    browserButton.setBounds(w-419,50,96,30);
    bypassButton.setBounds(w-312,12,160,32);
    const int centerTop=100,centerBottom=h-341;
    const int rowHeight=(centerBottom-centerTop-12)/2;
    const int spacing=12,margin=18;
    const int cardWidth=(w-2*margin-spacing*3)/4;
    for(int row=0;row<2;++row)
        for(int col=0;col<4;++col) {
            const int slot=row==0?col:7-col;
            cards[slot]={margin+col*(cardWidth+spacing),centerTop+row*(rowHeight+12),cardWidth,rowHeight};
        }
    const int base=h-294;
    selectedName.setBounds(24,base,240,31);
    slotStatus.setBounds(25,base+32,225,24);
    modelSelector.setBounds(24,base+67,220,35);
    enabledButton.setBounds(26,base+110,160,30);
    leftButton.setBounds(25,base+166,100,33);
    rightButton.setBounds(138,base+166,100,33);
    resetButton.setBounds(25,base+208,214,32);
    const int controlsStart=260;
    const int usable=std::max(300,w-650);
    const int controlWidth=usable/4;
    for(int k=0;k<pedal::kControls;++k) {
        knobLabels[size_t(k)].setBounds(controlsStart+k*controlWidth,base+35,controlWidth-5,22);
        knobs[size_t(k)].setBounds(controlsStart+k*controlWidth,base+58,controlWidth-5,150);
    }
    const int right=w-352;
    inLabel.setBounds(right,base+28,106,22);input.setBounds(right,base+54,106,142);
    outLabel.setBounds(right+113,base+28,106,22);output.setBounds(right+113,base+54,106,142);
    mixLabel.setBounds(right+226,base+28,106,22);mix.setBounds(right+226,base+54,106,142);
}
void PedalLegendsAudioProcessorEditor::mouseDown(const juce::MouseEvent& event) {
    const auto point=event.getPosition();
    for(int s=0;s<pedal::kSlots;++s) {
        if(!cards[s].contains(point))continue;
        const auto state=processor.snapshot().slots[size_t(s)];
        const auto led=juce::Rectangle<int>(cards[s].getRight()-52,cards[s].getY()+38,30,35);
        if(led.contains(point))processor.changeSlotEnabled(s,!state.enabled);
        selectSlot(s);
        return;
    }
}
void PedalLegendsAudioProcessorEditor::selectSlot(int slot) {
    selectedSlot=std::clamp(slot,0,7);
    const auto state=processor.snapshot().slots[size_t(selectedSlot)];
    const auto& metadata=pedal::info(state.model);
    selectedName.setText("SLOT "+juce::String(selectedSlot+1)+" — PEDAL EDIT",juce::dontSendNotification);
    slotStatus.setText(juce::String(metadata.family.data())+"  |  "+juce::String(metadata.era.data()),juce::dontSendNotification);
    modelSelector.setSelectedId(int(state.model)+1,juce::dontSendNotification);
    enabledButton.setToggleState(state.enabled,juce::dontSendNotification);
    for(int c=0;c<pedal::kControls;++c){
        knobLabels[size_t(c)].setText(juce::String(metadata.controls[size_t(c)].data()),juce::dontSendNotification);
        knobs[size_t(c)].setValue(state.controls[size_t(c)],juce::dontSendNotification);
    }
    leftButton.setEnabled(selectedSlot>0);
    rightButton.setEnabled(selectedSlot<7);
    repaint();
}
void PedalLegendsAudioProcessorEditor::showBrowser() {
    auto panel=std::make_unique<BrowserPanel>(processor);
    panel->setSize(480,std::min(690,getHeight()-110));
    juce::CallOutBox::launchAsynchronously(std::move(panel),browserButton.getScreenBounds(),nullptr);
}
void PedalLegendsAudioProcessorEditor::timerCallback() {
    const auto& list=pedal::factoryPresets();
    const int index=processor.selectedFactoryPreset();
    const juce::String changed=processor.isPresetModified()?"  • MODIFIED":"";
    presetName.setText(juce::String(list[size_t(index)].name)+changed,juce::dontSendNotification);
    const auto cfg=processor.snapshot();
    bypassButton.setToggleState(cfg.globalBypass,juce::dontSendNotification);
    input.setValue(cfg.inputDb,juce::dontSendNotification);
    output.setValue(cfg.outputDb,juce::dontSendNotification);
    mix.setValue(cfg.globalMix,juce::dontSendNotification);
    const auto& slot=cfg.slots[size_t(selectedSlot)];
    if(modelSelector.getSelectedId()!=int(slot.model)+1)selectSlot(selectedSlot);
    enabledButton.setToggleState(slot.enabled,juce::dontSendNotification);
    for(int c=0;c<pedal::kControls;++c)
        if(!knobs[size_t(c)].isMouseButtonDown())knobs[size_t(c)].setValue(slot.controls[size_t(c)],juce::dontSendNotification);
    repaint();
}
