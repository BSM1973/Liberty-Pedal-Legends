#include "PedalDSP.h"
#include "FactoryPresets.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

using namespace pedal;
constexpr int size=48000;
std::vector<float> render(const Settings& setup, int rate=48000) {
    Engine e;
    e.prepare(rate,2);
    std::vector<float> l(size),r(size);
    for (int i=0;i<size;++i) {
        l[size_t(i)]=.1f*std::sin(2.f*3.14159265f*220.f*i/rate)+.03f*std::sin(2.f*3.14159265f*880.f*i/rate);
        r[size_t(i)]=.1f*std::sin(2.f*3.14159265f*329.6f*i/rate);
    }
    for (int start=0;start<size;start+=128) {
        float* channels[]={l.data()+start,r.data()+start};
        e.process(channels,2,std::min(128,size-start),setup);
    }
    for (auto v:l) assert(std::isfinite(v) && std::abs(v)<=16.f);
    for (auto v:r) assert(std::isfinite(v) && std::abs(v)<=16.f);
    return l;
}
int main() {
    assert(catalog().size()==kModelCount);
    assert(kSlots==8);
    Settings base;
    auto original=render(base);
    for (int model=1;model<kModelCount;++model) {
        Settings cfg;
        cfg.slots[0]={Model(model),true,info(Model(model)).defaults};
        if (Model(model)==Model::SculptEQ) cfg.slots[0].controls={.8f,.45f,.65f,.5f};
        const auto modified=render(cfg);
        double delta=0.;
        for (int i=0;i<size;++i) delta+=std::abs(modified[size_t(i)]-original[size_t(i)]);
        if (delta<.1) { std::cerr << "Model not meaningfully processing: " << info(Model(model)).name << " " << delta << '\n'; return 1; }
    }
    // Slot order must alter the actual signal, not just visual arrangement.
    Settings a;
    a.slots[0]={Model::VerdantDrive,true,info(Model::VerdantDrive).defaults};
    a.slots[1]={Model::TapeVoyager,true,info(Model::TapeVoyager).defaults};
    auto first=render(a);
    std::swap(a.slots[0],a.slots[1]);
    auto swapped=render(a);
    double difference=0.;
    for (int i=0;i<size;++i) difference+=std::abs(first[size_t(i)]-swapped[size_t(i)]);
    assert(difference>0.1);
    // Identical model in two slots must process twice independently.
    a.slots[0]={Model::VerdantDrive,true,{.2f,.4f,.6f,1.f}};
    a.slots[1]={Model::VerdantDrive,true,{.9f,.4f,.6f,1.f}};
    auto two=render(a);
    a.slots[1].enabled=false;
    auto one=render(a);
    double separate=0.;
    for(int i=0;i<size;++i) separate+=std::abs(two[size_t(i)]-one[size_t(i)]);
    assert(separate>0.1);
    // The approved editor's additional per-slot MIX and LEVEL knobs must
    // change audio and must remain independent from the four model controls.
    Settings stageMix;
    stageMix.slots[0]={Model::VerdantDrive,true,info(Model::VerdantDrive).defaults};
    auto wetTone=render(stageMix);
    stageMix.slots[0].mix=0.f;
    auto dryTone=render(stageMix);
    double mixDifference=0.;
    for(int i=0;i<size;++i)mixDifference+=std::abs(wetTone[size_t(i)]-dryTone[size_t(i)]);
    assert(mixDifference>0.1);
    stageMix.slots[0].mix=1.f;
    stageMix.slots[0].levelDb=-12.f;
    auto quietTone=render(stageMix);
    stageMix.slots[0].levelDb=6.f;
    auto loudTone=render(stageMix);
    double quietEnergy=0.,loudEnergy=0.;
    for(int i=0;i<size;++i){quietEnergy+=quietTone[size_t(i)]*quietTone[size_t(i)];loudEnergy+=loudTone[size_t(i)]*loudTone[size_t(i)];}
    assert(loudEnergy>quietEnergy);
    // All eight slots are in the actual signal chain.
    for(int count=1;count<=8;++count) {
        Settings chain;
        for(int i=0;i<count;++i) chain.slots[size_t(i)]={Model::CrystalLift,true,{.11f,.5f,.5f,.45f}};
        auto out=render(chain);
        double error=0.;
        for(int i=0;i<size;++i) error+=std::abs(out[size_t(i)]-original[size_t(i)]);
        assert(error>.1);
        if(count>1){
            chain.slots[size_t(count-1)].enabled=false;
            auto shorter=render(chain);
            double delta=0.;
            for(int i=0;i<size;++i) delta+=std::abs(out[size_t(i)]-shorter[size_t(i)]);
            assert(delta>.1);
        }
    }
    // Bypass must not change the input.
    Settings bypass;
    bypass.globalBypass=true;
    bypass.slots[0]={Model::SteelFang,true,{1.f,1.f,1.f,1.f}};
    auto bypassed=render(bypass);
    for(int i=0;i<size;++i) assert(std::abs(bypassed[size_t(i)]-original[size_t(i)])<.00001f);
    assert(kModelCount==41);
    const auto& presets=factoryPresets();
    assert(presets.size()==100);
    std::set<std::string> names;
    std::set<std::string> chains;
    for(int c=0;c<10;++c) {
        for(int n=0;n<10;++n) {
            const auto& preset=presets[size_t(c*10+n)];
            assert(preset.category==presetCategories()[size_t(c)]);
            assert(!preset.name.empty());
            assert(names.insert(preset.name).second);
            std::ostringstream key;
            int active=0;
            for(auto& slot:preset.settings.slots) {
                key<<int(slot.model)<<':'<<slot.enabled;
                for(float x:slot.controls)key<<'/'<<int(x*10000.f);
                if(slot.enabled) { ++active; assert(slot.model!=Model::Empty); }
            }
            assert(chains.insert(key.str()).second);
            assert(active>=5);
            auto rendered=render(preset.settings);
            double energy=0.;
            for(auto v:rendered) energy+=double(v)*double(v);
            assert(energy>0.0001);
        }
    }
    // All advertised sample rates must process safely; mono is a separate bus layout.
    for(int rate:{44100,48000,88200,96000}) {
        Engine mono;
        mono.prepare(rate,1);
        std::array<float,257> input{};
        for(int i=0;i<int(input.size());++i)input[size_t(i)]=.18f*std::sin(6.2831853f*220.f*i/rate);
        for(int model=1;model<kModelCount;++model) {
            Settings cfg;
            cfg.slots[0]={Model(model),true,{1.f,1.f,1.f,1.f}};
            mono.reset();
            float* ptr[]{input.data()};
            mono.process(ptr,1,int(input.size()),cfg);
            for(float x:input)assert(std::isfinite(x));
        }
    }
    std::cout << "PASS: 40 effect models, 8 audio slots, stereo, order, bypass, independent instances, finite outputs, 100 factory presets\n";
}
