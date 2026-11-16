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
juce::String PedalLegendsAudioProcessor::midiParameterID(int target) {
    using namespace pedal::midi;
    if(target<0||target>=kTargetCount)return {};
    if(target<kSlots*kPerSlot) {
        const int s=target/kPerSlot,c=target%kPerSlot;
        const auto prefix="slot"+juce::String(s+1)+"_";
        switch(c) {
            case Enabled:return parameterID(s,1);
            case Mix:return prefix+"mix";
            case Level:return prefix+"level";
            case Branch:return prefix+"branch";
            case Control1:case Control2:case Control3:case Control4:
                return parameterID(s,c-2); // Control1 (4) => p1 (component 2)
            default:return {};
        }
    }
    static const char* globals[]={"global_input","global_output","global_mix",
       "global_bypass","global_gate","global_gate_threshold",
       "routing_parallel","routing_blend"};
    return globals[target-kSlots*kPerSlot];
}
juce::AudioProcessorValueTreeState::ParameterLayout PedalLegendsAudioProcessor::makeLayout() {
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> items;
    items.reserve(90);
    const auto models=modelNames();
    for(int s=0;s<pedal::kSlots;++s) {
        const auto prefix="Slot "+String(s+1)+" ";
        items.push_back(std::make_unique<AudioParameterChoice>(ParameterID(parameterID(s,0),1),prefix+"Pedal",models,0));
        items.push_back(std::make_unique<AudioParameterBool>(ParameterID(parameterID(s,1),1),prefix+"Enabled",false));
        items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("slot"+String(s+1)+"_mix",1),prefix+"Wet Dry",NormalisableRange<float>(0.f,1.f),1.f));
        items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("slot"+String(s+1)+"_level",1),prefix+"Level dB",NormalisableRange<float>(-18.f,18.f,.1f),0.f));
        items.push_back(std::make_unique<AudioParameterBool>(ParameterID("slot"+String(s+1)+"_branch",1),prefix+"Parallel Lane B",s>=4));
        for(int c=0;c<pedal::kControls;++c)
            items.push_back(std::make_unique<AudioParameterFloat>(ParameterID(parameterID(s,c+2),1),prefix+"Control "+String(c+1),NormalisableRange<float>(0.f,1.f),.5f));
    }
    items.push_back(std::make_unique<AudioParameterBool>(ParameterID("routing_parallel",1),"Parallel Routing",false));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("routing_blend",1),"Parallel A B Blend",NormalisableRange<float>(0.f,1.f),.5f));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_input",1),"Input dB",NormalisableRange<float>(-24.f,24.f,.1f),0.f));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_output",1),"Output dB",NormalisableRange<float>(-24.f,24.f,.1f),0.f));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_mix",1),"Global Wet Dry",NormalisableRange<float>(0.f,1.f),1.f));
    items.push_back(std::make_unique<AudioParameterBool>(ParameterID("global_bypass",1),"Global Bypass",false));
    items.push_back(std::make_unique<AudioParameterBool>(ParameterID("global_gate",1),"Global Noise Gate",false));
    items.push_back(std::make_unique<AudioParameterFloat>(ParameterID("global_gate_threshold",1),"Noise Gate Threshold dB",NormalisableRange<float>(-90.f,-15.f,.1f),-66.f));
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
        ref.mix=parameters.getRawParameterValue("slot"+juce::String(s+1)+"_mix");
        ref.level=parameters.getRawParameterValue("slot"+juce::String(s+1)+"_level");
        ref.branch=parameters.getRawParameterValue("slot"+juce::String(s+1)+"_branch");
        for(int c=0;c<pedal::kControls;++c)
            ref.controls[size_t(c)]=parameters.getRawParameterValue(parameterID(s,c+2));
    }
    inputGain=parameters.getRawParameterValue("global_input");
    outputGain=parameters.getRawParameterValue("global_output");
    mix=parameters.getRawParameterValue("global_mix");
    bypass=parameters.getRawParameterValue("global_bypass");
    routingMode=parameters.getRawParameterValue("routing_parallel");
    parallelBlendParam=parameters.getRawParameterValue("routing_blend");
    noiseGate=parameters.getRawParameterValue("global_gate");
    noiseGateThreshold=parameters.getRawParameterValue("global_gate_threshold");
    for(int i=0;i<pedal::midi::kTargetCount;++i) {
        midiCC[size_t(i)].store(-1,std::memory_order_relaxed);
        midiPending[size_t(i)].store(-1,std::memory_order_relaxed);
        midiParameters[size_t(i)]=parameters.getParameter(midiParameterID(i));
        jassert(midiParameters[size_t(i)]!=nullptr);
    }
    loadFactoryPreset(0);
    startTimerHz(30); // MIDI remains functional when the plugin editor is closed.
}
bool PedalLegendsAudioProcessor::isBusesLayoutSupported(const BusesLayout& b) const {
    auto ins=b.getMainInputChannelSet(),outs=b.getMainOutputChannelSet();
    return ins==outs && (ins==juce::AudioChannelSet::mono()||ins==juce::AudioChannelSet::stereo());
}
void PedalLegendsAudioProcessor::prepareToPlay(double rate,int) {
    engine.prepare(rate,getTotalNumInputChannels());
    inPeak.store(0); outPeak.store(0);
    gateEnvelope={{0.f,0.f}}; gateGain=1.f;
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
        slot.mix=std::clamp(ref.mix->load(std::memory_order_relaxed),0.f,1.f);
        slot.levelDb=std::clamp(ref.level->load(std::memory_order_relaxed),-18.f,18.f);
        slot.branch=ref.branch->load(std::memory_order_relaxed)>.5f?1:0;
        for(int c=0;c<pedal::kControls;++c)
            slot.controls[size_t(c)]=std::clamp(ref.controls[size_t(c)]->load(std::memory_order_relaxed),0.f,1.f);
    }
    state.inputDb=inputGain->load(std::memory_order_relaxed);
    state.outputDb=outputGain->load(std::memory_order_relaxed);
    state.globalMix=mix->load(std::memory_order_relaxed);
    state.globalBypass=bypass->load(std::memory_order_relaxed)>.5f;
    state.routing=routingMode->load(std::memory_order_relaxed)>.5f?pedal::RoutingMode::Parallel:pedal::RoutingMode::Series;
    state.parallelBlend=std::clamp(parallelBlendParam->load(std::memory_order_relaxed),0.f,1.f);
    return state;
}
void PedalLegendsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer& messages) {
    juce::ScopedNoDenormals noDenormals;
    receiveMidiControllers(messages); // Audio thread: atomic writes only.
    const int inputChannels=getTotalNumInputChannels();
    const int channels=std::min(inputChannels,buffer.getNumChannels());
    for(int c=channels;c<buffer.getNumChannels();++c)buffer.clear(c,0,buffer.getNumSamples());
    if(channels<1||buffer.getNumSamples()<1)return;
    float before=0.f;
    for(int c=0;c<channels;++c)before=std::max(before,buffer.getMagnitude(c,0,buffer.getNumSamples()));
    // Simple stereo-linked noise gate, smoothed sample-by-sample with no allocations.
    // All automation parameters are atomic and the envelope state lives in the processor.
    if(noiseGate->load(std::memory_order_relaxed)>0.5f) {
        const float sr=static_cast<float>(getSampleRate()>0?getSampleRate():48000.0);
        const float threshold=std::pow(10.f,noiseGateThreshold->load(std::memory_order_relaxed)/20.f);
        const float attack=1.f-std::exp(-1.f/(sr*.003f));
        const float release=1.f-std::exp(-1.f/(sr*.14f));
        for(int frame=0;frame<buffer.getNumSamples();++frame) {
            float envelope=0.f;
            for(int ch=0;ch<channels;++ch) {
                const float level=std::abs(buffer.getSample(ch,frame));
                auto& current=gateEnvelope[size_t(ch)];
                const float coefficient=level>current?attack:release;
                current+=(level-current)*coefficient;
                envelope=std::max(envelope,current);
            }
            const float desired=envelope>threshold?1.f:0.f;
            gateGain+=(desired-gateGain)*(desired>gateGain?attack:release);
            for(int ch=0;ch<channels;++ch)
                buffer.getWritePointer(ch)[frame]*=gateGain;
        }
    } else gateGain=1.f;
    float* pointers[2]={buffer.getWritePointer(0),channels>1?buffer.getWritePointer(1):nullptr};
    const auto setting=snapshot();
    engine.process(pointers,channels,buffer.getNumSamples(),setting);
    float after=0.f;
    for(int c=0;c<channels;++c)after=std::max(after,buffer.getMagnitude(c,0,buffer.getNumSamples()));
    inPeak.store(std::min(before,4.f),std::memory_order_relaxed);
    outPeak.store(std::min(after,4.f),std::memory_order_relaxed);
}
void PedalLegendsAudioProcessor::beginMidiLearn(int target) noexcept {
    if(target>=0&&target<pedal::midi::kTargetCount)
        waitingMidiLearn.store(target,std::memory_order_release);
}
void PedalLegendsAudioProcessor::cancelMidiLearn() noexcept {
    waitingMidiLearn.store(-1,std::memory_order_release);
}
void PedalLegendsAudioProcessor::clearMidiLearn(int target) noexcept {
    if(target<0||target>=pedal::midi::kTargetCount)return;
    midiCC[size_t(target)].store(-1,std::memory_order_release);
    midiPending[size_t(target)].store(-1,std::memory_order_release);
    if(waitingMidiLearn.load(std::memory_order_acquire)==target)cancelMidiLearn();
}
int PedalLegendsAudioProcessor::midiControllerFor(int target) const noexcept {
    return target>=0&&target<pedal::midi::kTargetCount
        ?midiCC[size_t(target)].load(std::memory_order_acquire):-1;
}
void PedalLegendsAudioProcessor::receiveMidiControllers(const juce::MidiBuffer& messages) noexcept {
    for(const auto metadata:messages) {
        const auto msg=metadata.getMessage();
        if(!msg.isController())continue;
        const int encoded=pedal::midi::encode(msg.getChannel(),msg.getControllerNumber());
        if(encoded<0)continue; // Ignore channel-mode messages.
        const int target=waitingMidiLearn.exchange(-1,std::memory_order_acq_rel);
        if(target>=0&&target<pedal::midi::kTargetCount) {
            // One MIDI controller addresses one target per channel.
            for(int i=0;i<pedal::midi::kTargetCount;++i)
                if(i!=target&&midiCC[size_t(i)].load(std::memory_order_relaxed)==encoded)
                    midiCC[size_t(i)].store(-1,std::memory_order_release);
            midiCC[size_t(target)].store(encoded,std::memory_order_release);
        }
        for(int i=0;i<pedal::midi::kTargetCount;++i)
            if(midiCC[size_t(i)].load(std::memory_order_relaxed)==encoded)
                midiPending[size_t(i)].store(msg.getControllerValue(),std::memory_order_release);
    }
}
void PedalLegendsAudioProcessor::timerCallback() {
    // Host notifications on the JUCE message thread, never inside audio DSP.
    for(int i=0;i<pedal::midi::kTargetCount;++i) {
        const int value=midiPending[size_t(i)].exchange(-1,std::memory_order_acq_rel);
        if(value<0)continue;
        auto* param=midiParameters[size_t(i)];
        if(param==nullptr)continue;
        const float normalized=juce::jlimit(0.f,1.f,float(value)/127.f);
        param->beginChangeGesture();
        param->setValueNotifyingHost(normalized);
        param->endChangeGesture();
        presetModified.store(true,std::memory_order_relaxed);
    }
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
        setParameterValue("slot"+juce::String(s+1)+"_mix",slot.mix);
        setParameterValue("slot"+juce::String(s+1)+"_level",slot.levelDb);
        setParameterValue("slot"+juce::String(s+1)+"_branch",float(slot.branch==1));
        setParameterValue(parameterID(s,1),slot.enabled?1.f:0.f);
    }
    setParameterValue("global_input",cfg.inputDb);
    setParameterValue("global_output",cfg.outputDb);
    setParameterValue("global_mix",cfg.globalMix);
    setParameterValue("global_bypass",cfg.globalBypass?1.f:0.f);
    setParameterValue("routing_parallel",cfg.routing==pedal::RoutingMode::Parallel?1.f:0.f);
    setParameterValue("routing_blend",std::clamp(cfg.parallelBlend,0.f,1.f));
}
void PedalLegendsAudioProcessor::restoreSettings(const pedal::Settings& settings) {
    applySettings(settings);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setGateEnabled(bool enabled) {
    setParameterValue("global_gate",enabled?1.f:0.f);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setGateThreshold(float db) {
    setParameterValue("global_gate_threshold",std::clamp(db,-90.f,-15.f));
    presetModified.store(true);
}
bool PedalLegendsAudioProcessor::gateEnabled() const noexcept {
    return noiseGate->load(std::memory_order_relaxed)>.5f;
}
float PedalLegendsAudioProcessor::gateThreshold() const noexcept {
    return noiseGateThreshold->load(std::memory_order_relaxed);
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
        setParameterValue("slot"+juce::String(s+1)+"_mix",stages[size_t(s)].mix);
        setParameterValue("slot"+juce::String(s+1)+"_level",stages[size_t(s)].levelDb);
        setParameterValue("slot"+juce::String(s+1)+"_branch",float(stages[size_t(s)].branch==1));
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
void PedalLegendsAudioProcessor::setRoutingMode(pedal::RoutingMode mode) {
    setParameterValue("routing_parallel",mode==pedal::RoutingMode::Parallel?1.f:0.f);
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setRoutingBranch(int slot,int branch) {
    if(slot<0||slot>=pedal::kSlots)return;
    setParameterValue("slot"+juce::String(slot+1)+"_branch",float(branch!=0));
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setParallelBlend(float blend) {
    setParameterValue("routing_blend",std::clamp(blend,0.f,1.f));
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setSlotMix(int slot,float value) {
    if(slot<0||slot>=pedal::kSlots)return;
    setParameterValue("slot"+juce::String(slot+1)+"_mix",std::clamp(value,0.f,1.f));
    presetModified.store(true);
}
void PedalLegendsAudioProcessor::setSlotLevel(int slot,float dbValue) {
    if(slot<0||slot>=pedal::kSlots)return;
    setParameterValue("slot"+juce::String(slot+1)+"_level",std::clamp(dbValue,-18.f,18.f));
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
    state.setProperty("UILanguage",getUILanguage(),nullptr);
    // Maps live in host project state, not in the factory preset bank.
    // Older projects without these properties remain loadable.
    for(int i=0;i<pedal::midi::kTargetCount;++i) {
        const int cc=midiCC[size_t(i)].load(std::memory_order_acquire);
        if(cc>=0)state.setProperty("MidiCC_"+juce::String(i),cc,nullptr);
    }
    if(auto xml=state.createXml())copyXmlToBinary(*xml,out);
}
void PedalLegendsAudioProcessor::setStateInformation(const void* input,int size) {
    if(auto xml=getXmlFromBinary(input,size)) {
        if(xml->hasTagName(parameters.state.getType().toString())) {
            auto state=juce::ValueTree::fromXml(*xml);
            const int preset=std::clamp(int(state.getProperty("FactoryPresetIndex",0)),0,pedal::kFactoryPresetCount-1);
            const bool changed=bool(state.getProperty("FactoryPresetModified",false));
            setUILanguage(int(state.getProperty("UILanguage",0)));
            cancelMidiLearn();
            for(int i=0;i<pedal::midi::kTargetCount;++i) {
                const int saved=int(state.getProperty("MidiCC_"+juce::String(i),-1));
                const int channel=pedal::midi::channel(saved);
                const int controller=pedal::midi::controller(saved);
                midiCC[size_t(i)].store(pedal::midi::encode(channel,controller),std::memory_order_release);
                midiPending[size_t(i)].store(-1,std::memory_order_release);
            }
            parameters.replaceState(state);
            factoryPresetIndex.store(preset);
            presetModified.store(changed);
        }
    }
}
juce::AudioProcessorEditor* PedalLegendsAudioProcessor::createEditor() {return new PedalLegendsAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {return new PedalLegendsAudioProcessor();}
