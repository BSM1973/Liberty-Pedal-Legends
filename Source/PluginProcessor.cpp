#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>

namespace {
namespace AP = juce;
AP::StringArray modelNames() {
    AP::StringArray names;
    for(const auto& item:pedal::catalog())names.add(AP::String(item.name.data(),int(item.name.size())));
    return names;
}
} // namespace

juce::String PedalLegendsAudioProcessor::parameterID(int slot,int component) {
    const juce::String prefix="slot"+juce::String(slot+1)+"_";
    if(component==0)return prefix+"model";
    if(component==1)return prefix+"on";
    return prefix+"p"+juce::String(component-1);
}
juce::AudioProcessorValueTreeState::ParameterLayout PedalLegendsAudioProcessor::makeLayout() {
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> items;
    items.reserve(52);
    const auto models=modelNames();
    for(int s=0;s<pedal::kSlots;++s) {
        const auto prefix="Slot "+String(s+1)+" ";
        items.push_back(std::make_unique<AudioParameterChoice>(ParameterID(parameterID(s,0),1),prefix+"Pedal",models,0));
        items.push_back(std::make_unique<AudioParameterBool>(ParameterID(parameterID(s,1),1),prefix+"Enabled",false));
        for(int c=0;c<pedal::kControls;++c)
            items.push_back(std::make_unique<AudioParameterFloat>(ParameterID(parameterID(s,c+2),1),prefix+"Control "+String(c+1),NormalisableRange<float>(0.f,1.f),.5f));
    }
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_input",1),"Input dB",NormalisableRange<float>(-24.f,24.f,.1f),0.f));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_output",1),"Output dB",NormalisableRange<float>(-24.f,24.f,.1f),0.f));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_mix",1),"Global Wet Dry",NormalisableRange<float>(0.f,1.f),1.f));
    items.push_back(std::make_unique<AudioParameterBool>(ParameterID("global_bypass",1),"Global Bypass",false));
    return {items.begin(),items.end()};
}
PedalLegendsAudioProcessor::PedalLegendsAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true)
                                  .withOutput("Output",juce::AudioChannelSet::stereo(),true)),
  parameters(*this,nullptr,"PEDAL_LEGENDS_STATE",makeLayout()) {
    for(int s=0;s<pedal::kSlots;++s) {
        auto& ref=refs[size_t(s)];
        ref.model=parameters.getRawParameterValue(parameterID(s,0));
        ref.enabled=parameters.getRawParameterValue(parameterID(s,1));
        for(int c=0;c<pedal::kControls;++c)
            ref.controls[size_t(c)]=parameters.getRawParameterValue(parameterID(s,c+2));
    }
    inputGain=parameters.getRawParameterValue("global_input");
    outputGain=parameters.getRawParameterValue("global_output");
    mix=parameters.getRawParameterValue("global_mix");
    bypass=parameters.getRawParameterValue("global_bypass");
    loadFactoryPreset(0);
}
bool PedalLegendsAudioProcessor::isBusesLayoutSupported(const BusesLayout& b) const {
    auto ins=b.getMainInputChannelSet(),outs=b.getMainOutputChannelSet();
    return ins==outs && (ins==juce::AudioChannelSet::mono()||ins==juce::AudioChannelSet::stereo());
}
void PedalLegendsAudioProcessor::prepareToPlay(double rate,int) {
    engine.prepare(rate,getTotalNumInputChannels());
    inPeak.store(0); outPeak.store(0);
}
void PedalLegendsAudioProcessor::releaseResources() { engine.reset(); }
pedal::Settings PedalLegendsAudioProcessor::snapshot() const noexcept {
    pedal::Settings state;
    for(int s=0;s<pedal::kSlots;++s) {
        const auto& ref=refs[size_t(s)];
        auto& slot=state.slots[size_t(s)];
        const int index=std::clamp(int(ref.model->load(std::memory_order_relaxed)),0,pedal::kModelCount-1);
        slot.model=pedal::Model(index);
        slot.enabled=ref.enabled->load(std::memory_order_relaxed)>.5f;
        for(int c=0;c<pedal::kControls;++c)
            slot.controls[size_t(c)]=std::clamp(ref.controls[size_t(c)]->load(std::memory_order_relaxed),0.f,1.f);
    }
    state.inputDb=inputGain->load(std::memory_order_relaxed);
    state.outputDb=outputGain->load(std::memory_order_relaxed);
    state.globalMix=mix->load(std::memory_order_relaxed);
    state.globalBypass=bypass->load(std::memory_order_relaxed)>.5f;
    return state;
}
void PedalLegendsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int inputChannels=getTotalNumInputChannels();
    const int channels=std::min(inputChannels,buffer.getNumChannels());
    for(int c=channels;c<buffer.getNumChannels();++c)buffer.clear(c,0,buffer.getNumSamples());
    if(channels<1||buffer.getNumSamples()<1)return;
    float before=0.f;
    for(int c=0;c<channels;++c)before=std::max(before,buffer.getMagnitude(c,0,buffer.getNumSamples()));
    float* pointers[2]={buffer.getWritePointer(0),channels>1?buffer.getWritePointer(1):nullptr};
    const auto setting=snapshot();
    engine.process(pointers,channels,buffer.getNumSamples(),setting);
    float after=0.f;
    for(int c=0;c<channels;++c)after=std::max(after,buffer.getMagnitude(c,0,buffer.getNumSamples()));
    inPeak.store(std::min(before,4.f),std::memory_order_relaxed);
    outPeak.store(std::min(after,4.f),std::memory_order_relaxed);
}
void PedalLegendsAudioProcessor::setParameterValue(const juce::String& id,float realValue) {
    if(auto* parameter=parameters.getParameter(id)) {
        if(auto* ranged=dynamic_cast<juce::RangedAudioParameter*>(parameter)) {
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost(ranged->convertTo0to1(realValue));
            parameter->endChangeGesture();
        }
    }
}
void PedalLegendsAudioProcessor::applySettings(const pedal::Settings& cfg) {
    for(int s=0;s<pedal::kSlots;++s) {
        const auto& slot=cfg.slots[size_t(s)];
        // Change model while off, then restore enabled state.
        setParameterValue(parameterID(s,1),0.f);
        setParameterValue(parameterID(s,0),float(int(slot.model)));
        for(int c=0;c<pedal::kControls;++c)
            setParameterValue(parameterID(s,c+2),slot.controls[size_t(c)]);
        setParameterValue(parameterID(s,1),slot.enabled?1.f:0.f);
    }
    setParameterValue("global_input",cfg.inputDb);
    setParameterValue("global_output",cfg.outputDb);
    setParameterValue("global_mix",cfg.globalMix);
    setParameterValue("global_bypass",cfg.globalBypass?1.f:0.f);
}
void PedalLegendsAudioProcessor::loadFactoryPreset(int index) {
    if(index<0||index>=pedal::kFactoryPresetCount)return;
    applySettings(pedal::factoryPresets()[size_t(index)].settings);
    factoryPresetIndex.store(index);
    presetModified.store(false);
}
const juce::String PedalLegendsAudioProcessor::getProgramName(int index) {
    if(index<0||index>=pedal::kFactoryPresetCount)return {};
    return juce::String(pedal::factoryPresets()[size_t(index)].name);
}
void PedalLegendsAudioProcessor::setModel(int slot,pedal::Model model) {
    if(slot<0||slot>=pedal::kSlots||int(model)<0||int(model)>=pedal::kModelCount)return;
    setParameterValue(parameterID(slot,0),float(int(model)));
    for(int c=0;c<pedal::kControls;++c)
        setParameterValue(parameterID(slot,c+2),pedal::info(model).defaults[size_t(c)]);
    setParameterValue(parameterID(slot,1),model==pedal::Model::Empty?0.f:1.f);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::resetSlot(int slot) {
    if(slot<0||slot>=pedal::kSlots)return;
    const auto cfg=snapshot().slots[size_t(slot)];
    setModel(slot,cfg.model);
}
void PedalLegendsAudioProcessor::moveSlot(int from,int to) {
    if(from<0||from>=pedal::kSlots||to<0||to>=pedal::kSlots||from==to)return;
    const auto cfg=snapshot();
    auto stages=cfg.slots;
    // Adjacent MOVE LEFT/RIGHT exchanges entire stages, not just images.
    std::swap(stages[size_t(from)],stages[size_t(to)]);
    for(int s: {from,to}) {
        setParameterValue(parameterID(s,1),0.f);
        setParameterValue(parameterID(s,0),float(int(stages[size_t(s)].model)));
        for(int c=0;c<pedal::kControls;++c)
            setParameterValue(parameterID(s,c+2),stages[size_t(s)].controls[size_t(c)]);
        setParameterValue(parameterID(s,1),stages[size_t(s)].enabled?1.f:0.f);
    }
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::changeSlotEnabled(int slot,bool value) {
    if(slot<0||slot>=pedal::kSlots)return;
    setParameterValue(parameterID(slot,1),value?1.f:0.f);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setControl(int slot,int control,float value) {
    if(slot<0||slot>=pedal::kSlots||control<0||control>=pedal::kControls)return;
    setParameterValue(parameterID(slot,control+2),std::clamp(value,0.f,1.f));
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setGlobal(int which,float value) {
    constexpr const char* ids[]={"global_input","global_output","global_mix","global_bypass"};
    if(which<0||which>3)return;
    setParameterValue(ids[which],value);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::getStateInformation(juce::MemoryBlock& out) {
    auto state=parameters.copyState();
    state.setProperty("FactoryPresetIndex",factoryPresetIndex.load(),nullptr);
    state.setProperty("FactoryPresetModified",presetModified.load(),nullptr);
    if(auto xml=state.createXml())copyXmlToBinary(*xml,out);
}
void PedalLegendsAudioProcessor::setStateInformation(const void* input,int size) {
    if(auto xml=getXmlFromBinary(input,size)) {
        if(xml->hasTagName(parameters.state.getType().toString())) {
            auto state=juce::ValueTree::fromXml(*xml);
            const int preset=std::clamp(int(state.getProperty("FactoryPresetIndex",0)),0,pedal::kFactoryPresetCount-1);
            const bool changed=bool(state.getProperty("FactoryPresetModified",false));
            parameters.replaceState(state);
            factoryPresetIndex.store(preset);
            presetModified.store(changed);
        }
    }
}
juce::AudioProcessorEditor* PedalLegendsAudioProcessor::createEditor() {return new PedalLegendsAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {return new PedalLegendsAudioProcessor();}
