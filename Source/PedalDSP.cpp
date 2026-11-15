#include "PedalDSP.h"
#include <algorithm>
#include <limits>

namespace pedal {
namespace {
constexpr float pi = 3.14159265358979323846f;
constexpr float tau = 6.28318530717958647692f;
float clamp(float x, float lo, float hi) noexcept { return std::clamp(x, lo, hi); }
float saturate(float x) noexcept { return std::tanh(x); }
float db(float x) noexcept { return std::pow(10.f, x * (1.f / 20.f)); }
float lp(float x, float& z, float hz, double sr) noexcept {
    const float coeff = clamp(1.f - std::exp(-tau * hz / float(sr)), 0.f, 1.f);
    z += coeff * (x - z);
    if (std::abs(z) < 1.e-20f) z = 0.f;
    return z;
}
float hp(float x, float& z, float hz, double sr) noexcept { return x - lp(x, z, hz, sr); }
float cycle(float& phase, float hz, double sr) noexcept {
    phase += hz / float(sr);
    if (phase >= 1.f) phase -= 1.f;
    return phase;
}
float shaped(float x, float gain, float bias) noexcept {
    return (saturate(gain * (x + bias)) - saturate(gain * bias));
}
}

const std::array<ModelInfo, kModelCount>& catalog() noexcept {
    static const std::array<ModelInfo, kModelCount> c {{
        {Model::Empty, "EMPTY", "None", "—", 0x56564f, {"—", "—", "—", "—"}, {.5f,.5f,.5f,.5f}},
        {Model::VerdantDrive,"Verdant Drive","Overdrive","1980s",0x667848,{"DRIVE","TONE","LEVEL","MIX"},{.42f,.55f,.55f,1.f}},
        {Model::GoldenArc,"Golden Arc","Overdrive","1990s",0xbba567,{"GAIN","BODY","CLEAN","LEVEL"},{.48f,.55f,.3f,.55f}},
        {Model::CopperSoul,"Copper Soul","Overdrive","2000s",0xaf784e,{"GAIN","BIAS","TONE","LEVEL"},{.4f,.48f,.55f,.5f}},
        {Model::OrangeVoltage,"Orange Voltage","Distortion","1970s",0xc16f37,{"DIST","TONE","PRESENCE","LEVEL"},{.56f,.5f,.52f,.5f}},
        {Model::SteelFang,"Steel Fang","Distortion","1990s",0x687276,{"GAIN","TIGHT","TONE","LEVEL"},{.55f,.6f,.6f,.5f}},
        {Model::VelvetFury,"Velvet Fury","Fuzz","1970s",0x693f65,{"FUZZ","BIAS","TONE","LEVEL"},{.7f,.5f,.45f,.42f}},
        {Model::SiliconStorm,"Silicon Storm","Fuzz","1980s",0x5686a8,{"FUZZ","OCTAVE","FILTER","LEVEL"},{.65f,.3f,.53f,.5f}},
        {Model::CrystalLift,"Crystal Lift","Boost / Preamp","2000s",0x9bada4,{"BOOST","BASS","TREBLE","LEVEL"},{.3f,.5f,.5f,.5f}},
        {Model::BritishSpark,"British Spark","Boost / Preamp","1970s",0xb79c5f,{"BOOST","SPARK","BODY","LEVEL"},{.4f,.68f,.43f,.5f}},
        {Model::VelvetSustain,"Velvet Sustain","Compressor","1980s",0x8d8878,{"SUSTAIN","ATTACK","BLEND","LEVEL"},{.55f,.3f,.75f,.5f}},
        {Model::SculptEQ,"Olive Tone","EQ / Filter","2000s",0x719a8b,{"LOW","MID","HIGH","LEVEL"},{.5f,.5f,.5f,.5f}},
        {Model::VocalWah,"Vocal Wah","Wah / Envelope","1970s",0x948048,{"POSITION","Q","DRIVE","MIX"},{.5f,.52f,.35f,.85f}},
        {Model::AzureChorus,"Azure Chorus","Modulation","1980s",0x497caa,{"RATE","DEPTH","TONE","MIX"},{.3f,.55f,.65f,.5f}},
        {Model::OrbitPhase,"Orbit Phase","Modulation","1970s",0x937bba,{"RATE","DEPTH","REGEN","MIX"},{.4f,.7f,.3f,.72f}},
        {Model::CelestialFlange,"Celestial Flange","Modulation","1980s",0x558b9b,{"RATE","DEPTH","FEEDBACK","MIX"},{.36f,.65f,.48f,.55f}},
        {Model::WarmTremolo,"Warm Tremolo","Tremolo","1973–1979",0x9f694e,{"RATE","DEPTH","SHAPE","MIX"},{.35f,.7f,.5f,1.f}},
        {Model::TapeVoyager,"Tape Voyager","Delay / Echo","1970s",0x8c744c,{"TIME","REPEATS","AGE","MIX"},{.42f,.42f,.65f,.38f}},
        {Model::CrystalRepeat,"Crystal Repeat","Delay / Echo","1990s",0x829cb4,{"TIME","FEEDBACK","TONE","MIX"},{.35f,.42f,.8f,.38f}},
        {Model::GoldenPlate,"Golden Plate","Reverb","1970s",0xc19a61,{"SIZE","DECAY","DAMP","MIX"},{.55f,.62f,.45f,.35f}},
        {Model::DreamFracture,"Dream Fracture","Experimental","2020s",0x725c93,{"RATE","CHOP","ECHO","MIX"},{.48f,.68f,.44f,.6f}},
        {Model::AmberBreaker,"Amber Breaker","Overdrive","1973–1990s",0xc4a276,{"GAIN","DYNAMICS","TONE","LEVEL"},{.48f,.7f,.56f,.48f}},
        {Model::VintageBloom,"Vintage Bloom","Overdrive","1970s",0x877b4a,{"DRIVE","BASS","AIR","LEVEL"},{.4f,.55f,.4f,.55f}},
        {Model::IronHammer,"Iron Hammer","Distortion","1980s",0x616875,{"GAIN","MID SCOOP","EDGE","LEVEL"},{.68f,.7f,.6f,.45f}},
        {Model::CrimsonRiot,"Crimson Riot","Distortion","1990s",0x9c4a47,{"GAIN","MID","BITE","LEVEL"},{.56f,.58f,.61f,.5f}},
        {Model::BlackReactor,"Black Reactor","Distortion","2010s",0x3b4144,{"GAIN","TIGHT","THRUST","LEVEL"},{.67f,.7f,.55f,.45f}},
        {Model::GermaniumSun,"Germanium Sun","Fuzz","1970s",0xc69c53,{"FUZZ","BIAS","TONE","LEVEL"},{.7f,.52f,.48f,.44f}},
        {Model::WoollyThunder,"Woolly Thunder","Fuzz","1990s",0x635f4c,{"FUZZ","GATE","LOW","LEVEL"},{.65f,.38f,.7f,.4f}},
        {Model::GoldlinePreamp,"Goldline Preamp","Boost / Preamp","2000s",0x9f937b,{"GAIN","BODY","SHEEN","LEVEL"},{.35f,.45f,.5f,.5f}},
        {Model::OpticalSilk,"Copper Optic","Compressor","2000s",0xc1ad8a,{"THRESH","RELEASE","BLEND","LEVEL"},{.5f,.55f,.85f,.52f}},
        {Model::ResonantSweep,"Resonant Sweep","EQ / Filter","2010s",0x647e69,{"FREQ","Q","DRIVE","MIX"},{.5f,.55f,.28f,.9f}},
        {Model::FunkEnvelope,"Funk Envelope","Wah / Envelope","1970s",0x9c854e,{"SENSE","RESON","DECAY","MIX"},{.58f,.55f,.47f,.82f}},
        {Model::AnalogMirage,"Analog Mirage","Modulation","1980s",0x718c81,{"RATE","DEPTH","WARMTH","MIX"},{.27f,.5f,.65f,.55f}},
        {Model::HarmonicPulse,"Harmonic Pulse","Tremolo","1973–1979",0x836a63,{"RATE","DEPTH","SPLIT","MIX"},{.3f,.55f,.56f,.95f}},
        {Model::MemoryTrail,"Memory Trail","Delay / Echo","1980s",0x797b58,{"TIME","FEEDBACK","DARK","MIX"},{.36f,.5f,.68f,.35f}},
        {Model::CathedralAir,"Cathedral Air","Reverb","2000s",0x7d85a1,{"SPACE","DECAY","DAMP","MIX"},{.65f,.7f,.6f,.42f}},
        {Model::SpringGarden,"Spring Garden","Reverb","1973–1979",0xc1a16d,{"TENSION","DWELL","TONE","MIX"},{.4f,.55f,.65f,.35f}},
        {Model::OctaveCrown,"Octave Crown","Pitch / Octave","1970s",0x857453,{"OCT UP","OCT DOWN","TONE","MIX"},{.45f,.34f,.64f,.7f}},
        {Model::SubmarineVoice,"Submarine Voice","Pitch / Octave","1980s",0x4a6b78,{"SUB","ATTACK","TONE","MIX"},{.65f,.35f,.45f,.8f}},
        {Model::TimeGarden,"Time Garden","Experimental","2020s",0x668b7b,{"RATE","DELAY","DRIFT","MIX"},{.28f,.48f,.5f,.6f}},
        {Model::InfiniteDrift,"Infinite Drift","Experimental","2020s",0x77739a,{"SPEED","FEED","FILTER","MIX"},{.38f,.52f,.55f,.55f}}
    }};
    return c;
}
const ModelInfo& info(Model model) noexcept {
    const int i = int(model);
    return catalog()[i >= 0 && i < kModelCount ? i : 0];
}

float Engine::DelayLine::read(float samples, uint64_t age) const noexcept {
    if (data.empty()) return 0.f;
    const float d = clamp(samples, 1.f, float(data.size() - 2));
    if (age <= static_cast<uint64_t>(std::ceil(d))) return 0.f;
    float idx = float(index) - d;
    if (idx < 0.f) idx += float(data.size());
    const int a = int(idx);
    const int b = (a + 1) % int(data.size());
    return data[size_t(a)] + (data[size_t(b)] - data[size_t(a)]) * (idx - float(a));
}
void Engine::DelayLine::write(float value) noexcept {
    if (data.empty()) return;
    data[size_t(index)] = clamp(value, -8.f, 8.f);
    if (++index == int(data.size())) index = 0;
}
void Engine::ChannelState::resetScalars() noexcept {
    lp1 = lp2 = lp3 = env = feedback = phase = phaserFeedback = prevInput = 0.f;
    sub = 1.f;
    for (float& p : phaser) p = 0.f;
    age = 0;
    delay.index = 0;
    for (auto& c : comb) { c.index = 0; c.filter = 0.f; }
    for (auto& a : allpass) a.index = 0;
}
void Engine::prepare(double sampleRate, int channels) {
    sr_ = clamp(float(sampleRate), 8000.f, 384000.f);
    preparedChannels_ = std::clamp(channels, 1, 2);
    masterInitialized_ = false;
    lastRouting_ = RoutingMode::Series;
    lastBranches_.fill(0);
    previousOutput_.fill(0.f);
    routingFadeSamples_ = 0;
    routingFadeLength_ = std::max(32,int(sr_*.006));
    smoothedParallelBlend_ = .5f;
    for (auto& slot : states_) {
        slot.lastModel = Model::Empty;
        slot.wet = 0.f;
        for (auto& ch : slot.channel) {
            ch.delay.data.assign(size_t(sr_ * 2.1) + 4, 0.f);
            // Different prime-like lengths reduce coincident metallic modes.
            constexpr float combSec[] = { .0297f, .0371f, .0411f, .0437f };
            constexpr float passSec[] = { .005f, .0017f };
            for (int i=0; i<4; ++i) ch.comb[size_t(i)].data.assign(std::max(8, int(sr_ * combSec[i])), 0.f);
            for (int i=0; i<2; ++i) ch.allpass[size_t(i)].data.assign(std::max(8, int(sr_ * passSec[i])), 0.f);
            ch.resetScalars();
        }
    }
}
void Engine::reset() noexcept {
    masterInitialized_ = false;
    lastRouting_ = RoutingMode::Series;
    lastBranches_.fill(0);
    previousOutput_.fill(0.f);
    routingFadeSamples_ = 0;
    routingFadeLength_ = std::max(32,int(sr_*.006));
    smoothedParallelBlend_ = .5f;
    for (auto& slot : states_) {
        slot.lastModel = Model::Empty;
        slot.wet = 0.f;
        for (auto& ch : slot.channel) {
            std::fill(ch.delay.data.begin(), ch.delay.data.end(), 0.f);
            for (auto& c : ch.comb) std::fill(c.data.begin(), c.data.end(), 0.f);
            for (auto& p : ch.allpass) std::fill(p.data.begin(), p.data.end(), 0.f);
            ch.resetScalars();
        }
    }
}
float Engine::processOne(Model model, ChannelState& s, int channel, float x,
                         const std::array<float, kControls>& p) noexcept {
    const float a = p[0], b = p[1], c = p[2], d = p[3];
    float y = x;
    switch (model) {
        case Model::Empty: return x;
        case Model::VerdantDrive: {
            float mid = hp(x, s.lp1, 160.f, sr_);
            mid = lp(mid, s.lp2, 3600.f, sr_);
            const float drive = 1.5f + 30.f*a*a;
            const float clipped = saturate(mid * drive) * .75f;
            const float shapedTone = lp(clipped, s.lp3, 650.f + b*7500.f, sr_);
            y = ((1.f-d)*x + d*shapedTone) * (.5f + c*1.6f);
            break;
        }
        case Model::GoldenArc: {
            const float high = hp(x, s.lp1, 60.f + (1.f-b)*230.f, sr_);
            float crunch = saturate(high * (1.f + a*a*21.f));
            crunch = lp(crunch, s.lp2, 2700.f + b*5600.f, sr_);
            y = ((1.f-c)*crunch + c*x) * (.35f + d*1.7f);
            break;
        }
        case Model::CopperSoul: {
            const float filtered = hp(x, s.lp1, 130.f, sr_);
            const float bias = (b-.5f)*.5f;
            float out = shaped(filtered, 1.5f + a*a*35.f, bias);
            out = lp(out, s.lp2, 1200.f+c*6500.f, sr_);
            y = out * (.3f+d*1.65f);
            break;
        }
        case Model::OrangeVoltage: {
            const float src = hp(x, s.lp1, 90.f, sr_);
            const float gain = 1.f + a*36.f;
            float hard = clamp(src*gain, -.48f, .48f) * 1.6f;
            hard = lp(hard, s.lp2, 850.f + b*7200.f, sr_);
            const float high = hp(hard, s.lp3, 1500.f, sr_);
            y = (hard+(c-.5f)*.9f*high)*(.25f+d*1.55f);
            break;
        }
        case Model::SteelFang: {
            float tight = hp(x, s.lp1, 55.f + b*620.f, sr_);
            float first = saturate(tight*(2.f+18.f*a));
            first = hp(first, s.lp2, 140.f, sr_);
            y = saturate(first*(2.f+15.f*a));
            y = lp(y, s.lp3, 1100.f+c*7600.f, sr_)*(.22f+d*1.1f);
            break;
        }
        case Model::VelvetFury: {
            const float bias = (b-.5f)*.85f;
            const float fuzz = (1.f+55.f*a)*hp(x, s.lp1, 30.f, sr_);
            const float squashed = shaped(fuzz, 2.5f, bias);
            y = lp(squashed, s.lp2, 450.f+c*7600.f, sr_)*(.16f+d*1.15f);
            break;
        }
        case Model::SiliconStorm: {
            const float src = hp(x, s.lp1, 55.f, sr_)*(2.f+38.f*a);
            const float rectified = 2.f*std::abs(src)-.65f;
            const float blended = (1.f-b)*src + b*rectified;
            const float gated = std::abs(blended)<.05f ? 0.f : clamp(blended,-.92f,.92f);
            y = lp(gated, s.lp2, 600.f+c*9400.f, sr_)*(.2f+d*1.15f);
            break;
        }
        case Model::CrystalLift: {
            const float low = lp(x,s.lp1,250.f,sr_);
            const float high = hp(x,s.lp2,2200.f,sr_);
            y = (x+(b-.5f)*1.2f*low+(c-.5f)*1.2f*high) * db(a*18.f) * (.5f+d);
            break;
        }
        case Model::BritishSpark: {
            const float treble = hp(x,s.lp1,400.f+c*900.f,sr_);
            const float body = lp(x,s.lp2,1100.f,sr_);
            y = saturate((treble*(.35f+b*2.f)+body*(.3f+c))*(1.f+9.f*a))*(.25f+d*1.5f);
            break;
        }
        case Model::VelvetSustain: {
            const float attack = .001f + b*.04f, release = .07f+c*.13f;
            const float coeff = std::exp(-1.f / (float(sr_)*(std::abs(x)>s.env?attack:release)));
            s.env = coeff*s.env+(1.f-coeff)*std::abs(x);
            const float threshold = .35f-.28f*a;
            const float ratio = 1.5f+a*9.f;
            const float over = std::max(0.f,s.env-threshold);
            const float gain = 1.f/(1.f+over*ratio*12.f);
            const float comp = x*gain*(1.f+a*2.2f);
            y = ((1.f-c)*x+c*comp)*(.4f+d*1.5f);
            break;
        }
        case Model::SculptEQ: {
            const float low = lp(x,s.lp1,230.f,sr_);
            const float high = hp(x,s.lp2,3200.f,sr_);
            const float mid = x-low-high;
            y = (low*db((a-.5f)*24.f)+mid*db((b-.5f)*24.f)+high*db((c-.5f)*24.f))*(.5f+d);
            break;
        }
        case Model::VocalWah: {
            const float hz = 310.f*std::pow(7.3f,a);
            const float q = 1.6f+16.f*b;
            // Chamberlin SVF, input saturates gently for resonance stability.
            const float f = clamp(2.f*std::sin(pi*hz/float(sr_)),0.f,.86f);
            const float input = saturate(x*(1.f+c*4.f));
            const float high = input-s.lp2-s.lp1/q;
            s.lp1 += f*high;
            s.lp2 += f*s.lp1;
            s.lp1 = clamp(s.lp1,-6.f,6.f);
            s.lp2 = clamp(s.lp2,-6.f,6.f);
            y = x*(1.f-d)+d*s.lp1*.82f;
            break;
        }
        case Model::AzureChorus: {
            const float speed = .12f+a*5.3f;
            const float ph = cycle(s.phase,speed,sr_);
            const float lfo = std::sin(tau*(ph+channel*.23f));
            const float ds = (.013f+b*.007f*(1.f+lfo))*float(sr_);
            const float delayed = s.delay.read(ds,s.age);
            s.delay.write(x);
            y = x*(1.f-d)+lp(delayed,s.lp1,1400.f+c*11000.f,sr_)*d;
            break;
        }
        case Model::OrbitPhase: {
            const float ph = cycle(s.phase,.08f+a*8.f,sr_);
            const float hz = 180.f+(.5f+.5f*std::sin(tau*ph))*b*3300.f;
            const float t = std::tan(pi*std::min(hz,float(sr_)*.3f)/float(sr_));
            const float coef = (1.f-t)/(1.f+t);
            float v = saturate(x+s.phaserFeedback*(c*.72f));
            for (float& z : s.phaser) { const float next = -coef*v+z; z=v+coef*next; v=next; }
            s.phaserFeedback = v;
            y = (1.f-d)*x+d*(x+v)*.6f;
            break;
        }
        case Model::CelestialFlange: {
            const float ph = cycle(s.phase,.07f+a*2.4f,sr_);
            const float ds = (1.5f+b*6.f*(1.f+std::sin(tau*(ph+channel*.18f))))*.001f*float(sr_);
            const float delayed = s.delay.read(ds,s.age);
            const float fb = (c-.5f)*1.5f;
            s.delay.write(saturate(x+delayed*fb));
            y = x*(1.f-d)+d*(x+delayed)*.58f;
            break;
        }
        case Model::WarmTremolo: {
            const float ph = cycle(s.phase,.25f+a*13.f,sr_);
            const float sine = .5f+.5f*std::sin(tau*ph);
            const float square = ph<.5f ? 1.f : 0.f;
            const float mod = (1.f-c)*sine+c*square;
            const float depth = b*d;
            y = x*(1.f-depth+depth*mod);
            break;
        }
        case Model::TapeVoyager: {
            const float ph = cycle(s.phase,.37f,sr_);
            const float wow = .002f*std::sin(tau*ph);
            const float ds = (.09f + a*1.5f+wow)*float(sr_);
            const float echo = s.delay.read(ds,s.age);
            const float dark = lp(echo,s.lp1,800.f+(1.f-c)*6500.f,sr_);
            s.delay.write(saturate(x+dark*(.08f+b*.84f)));
            y = (1.f-d)*x+d*dark;
            break;
        }
        case Model::CrystalRepeat: {
            const float ds = (.035f+a*1.8f)*float(sr_);
            const float echo = s.delay.read(ds,s.age);
            const float tone = lp(echo,s.lp1,1400.f+c*14000.f,sr_);
            s.delay.write(saturate(x+tone*(.06f+b*.89f)));
            y = (1.f-d)*x+d*tone;
            break;
        }
        case Model::GoldenPlate: {
            float sum = 0.f;
            const float fb = clamp(.4f + b*.47f,0.f,.9f);
            for (auto& comb : s.comb) {
                const auto idx = size_t(comb.index);
                const float read = s.age>comb.data.size() ? comb.data[idx] : 0.f;
                const float damp = .08f+c*.75f;
                comb.filter += damp*(read-comb.filter);
                comb.data[idx] = clamp(x*.2f+comb.filter*fb,-4.f,4.f);
                if (++comb.index==int(comb.data.size())) comb.index=0;
                sum += read*.25f;
            }
            for (auto& pass : s.allpass) {
                const auto idx = size_t(pass.index);
                const float old = s.age>pass.data.size() ? pass.data[idx] : 0.f;
                const float val = sum+old*.63f;
                pass.data[idx]=clamp(val,-4.f,4.f);
                if (++pass.index==int(pass.data.size())) pass.index=0;
                sum = old-.63f*val;
            }
            const float tail = lp(sum,s.lp1,2400.f+a*9000.f,sr_);
            y=(1.f-d)*x+d*tail*.9f;
            break;
        }
        case Model::DreamFracture: {
            const float hz=.5f+a*12.f;
            const float ph=cycle(s.phase,hz,sr_);
            const float duty=.12f+(1.f-b)*.68f;
            const float target=ph<duty ? 1.f : .03f;
            const float gate=lp(target,s.lp2,100.f,sr_);
            const float echo=s.delay.read((.11f+a*.37f)*float(sr_),s.age);
            const float fed=lp(echo,s.lp1,1300.f+c*6500.f,sr_);
            s.delay.write(saturate(x*gate+fed*(.2f+c*.55f)));
            y=(1.f-d)*x+d*(x*gate*.65f+fed*.42f);
            break;
        }
        case Model::AmberBreaker: {
            const float filtered = hp(x,s.lp1,80.f,sr_);
            const float edge = saturate(filtered*(1.f+16.f*a));
            const float compression = .9f/(1.f+b*.7f*std::abs(edge));
            y=lp(edge*compression,s.lp2,1400.f+c*6500.f,sr_)*(.3f+1.6f*d);
            break;
        }
        case Model::VintageBloom: {
            const float bass=lp(x,s.lp1,240.f,sr_);
            const float mids=x-bass;
            const float warmed=saturate((mids+bass*(.6f+b))*(1.f+a*17.f));
            const float air=hp(warmed,s.lp2,1600.f,sr_);
            y=(warmed+(c-.5f)*air)*(.25f+1.5f*d);
            break;
        }
        case Model::IronHammer: {
            const float tight=hp(x,s.lp1,110.f,sr_);
            const float first=clamp(tight*(1.f+a*27.f),-.57f,.57f);
            const float low=lp(first,s.lp2,380.f,sr_);
            const float high=hp(first,s.lp3,1700.f,sr_);
            y=(first-low*b*.93f + high*(c-.5f)*.6f)*(.4f+d);
            break;
        }
        case Model::CrimsonRiot: {
            const float first=saturate(hp(x,s.lp1,70.f,sr_)*(1.f+a*32.f));
            const float crushed=clamp(first*2.f,-.71f,.77f);
            const float center=lp(crushed,s.lp2,900.f,sr_);
            const float presence=hp(crushed,s.lp3,3200.f,sr_);
            y=(crushed+(b-.5f)*center+presence*c*.55f)*(.25f+d*1.2f);
            break;
        }
        case Model::BlackReactor: {
            const float pre=hp(x,s.lp1,120.f+b*450.f,sr_);
            const float stage1=shaped(pre,3.f+28.f*a,.07f);
            const float stage2=saturate(stage1*(3.f+12.f*a));
            y=lp(stage2,s.lp2,1700.f+6300.f*c,sr_)*(.18f+d);
            break;
        }
        case Model::GermaniumSun: {
            const float filtered=lp(x,s.lp1,7000.f,sr_);
            const float bias=(b-.5f)*.48f;
            const float fuzz=shaped(filtered,1.f+24.f*a,bias);
            y=lp(fuzz,s.lp2,800.f+c*4800.f,sr_)*(.22f+1.2f*d);
            break;
        }
        case Model::WoollyThunder: {
            const float low=lp(x,s.lp1,290.f,sr_);
            const float driven=saturate((x+low*c*2.5f)*(2.f+a*45.f));
            const float threshold=.015f+b*.18f;
            const float gated=std::abs(driven)<threshold ? 0.f:driven;
            y=lp(gated,s.lp2,4500.f,sr_)*(.18f+d*1.25f);
            break;
        }
        case Model::GoldlinePreamp: {
            const float bass=lp(x,s.lp1,220.f,sr_);
            const float high=hp(x,s.lp2,2200.f,sr_);
            const float body=x+(b-.5f)*bass+(c-.5f)*high;
            y=saturate(body*(1.f+a*7.f))*(.5f+d);
            break;
        }
        case Model::OpticalSilk: {
            const float envTarget=std::abs(x);
            const float decay=.015f+b*.30f;
            const float coeff=std::exp(-1.f/(float(sr_)*(envTarget>s.env?.015f:decay)));
            s.env=coeff*s.env+(1.f-coeff)*envTarget;
            const float threshold=.27f-.23f*a;
            const float excess=std::max(0.f,s.env-threshold);
            const float attenuation=std::exp(-excess*8.f);
            y=((1.f-c)*x+c*x*attenuation*(1.6f+a))*(.5f+d);
            break;
        }
        case Model::ResonantSweep: {
            // Stable TPT-inspired two-state filter, with limited resonance and cutoff.
            const float hz=120.f*std::pow(60.f,a);
            const float f=clamp(2.f*std::sin(pi*hz/float(sr_)),0.f,.72f);
            const float q=1.2f+b*7.f;
            const float input=saturate(x*(1.f+c*3.f));
            const float h=input-s.lp2-s.lp1/q;
            s.lp1=clamp(s.lp1+f*h,-4.f,4.f);
            s.lp2=clamp(s.lp2+f*s.lp1,-4.f,4.f);
            y=(1.f-d)*x+d*s.lp1;
            break;
        }
        case Model::FunkEnvelope: {
            const float decay=.012f+c*.22f;
            const float envIn=std::abs(x);
            const float coeff=std::exp(-1.f/(float(sr_)*(envIn>s.env?.002f:decay)));
            s.env=coeff*s.env+(1.f-coeff)*envIn;
            const float hz=280.f+clamp(s.env*(3.f+25.f*a),0.f,1.f)*3300.f;
            const float f=clamp(2.f*std::sin(pi*hz/float(sr_)),0.f,.85f);
            const float h=saturate(x)-s.lp2-s.lp1/(1.5f+b*10.f);
            s.lp1=clamp(s.lp1+f*h,-6.f,6.f);
            s.lp2=clamp(s.lp2+f*s.lp1,-6.f,6.f);
            y=x*(1.f-d)+s.lp1*d;
            break;
        }
        case Model::AnalogMirage: {
            const float ph=cycle(s.phase,.11f+a*3.5f,sr_);
            const float mod=std::sin(tau*(ph+channel*.17f));
            const float ds=(.008f+.013f*b*(1.f+mod))*float(sr_);
            const float tap=s.delay.read(ds,s.age);
            s.delay.write(lp(x,s.lp1,4500.f+c*5000.f,sr_));
            y=x*(1.f-d)+d*(x+tap)*.52f;
            break;
        }
        case Model::HarmonicPulse: {
            const float ph=cycle(s.phase,.2f+a*8.f,sr_);
            const float osc=std::sin(tau*(ph+channel*.18f));
            const float bass=lp(x,s.lp1,220.f+c*900.f,sr_);
            const float high=x-bass;
            const float slow=bass*(1.f-b*d*(.5f+.5f*osc));
            const float fast=high*(1.f-b*d*(.5f-.5f*osc));
            y=slow+fast;
            break;
        }
        case Model::MemoryTrail: {
            const float ds=(.07f+a*.76f)*float(sr_);
            const float delayed=s.delay.read(ds,s.age);
            const float dark=lp(delayed,s.lp1,1600.f+(1.f-c)*5800.f,sr_);
            const float decayed=saturate(dark*.8f);
            s.delay.write(saturate(x+decayed*(.12f+b*.73f)));
            y=(1.f-d)*x+d*dark;
            break;
        }
        case Model::CathedralAir: {
            float sum=0.f;
            const float fb=.58f+b*.33f;
            for(auto& comb:s.comb) {
                const auto idx=size_t(comb.index);
                const float tap=s.age>comb.data.size()?comb.data[idx]:0.f;
                comb.filter+=(.08f+c*.6f)*(tap-comb.filter);
                comb.data[idx]=clamp((x*.15f+comb.filter*fb),-4.f,4.f);
                if(++comb.index==int(comb.data.size()))comb.index=0;
                sum+=tap*.24f;
            }
            for(auto& pass:s.allpass) {
                const auto idx=size_t(pass.index);
                const float tap=s.age>pass.data.size()?pass.data[idx]:0.f;
                const float combined=sum+tap*.72f;
                pass.data[idx]=clamp(combined,-4.f,4.f);
                if(++pass.index==int(pass.data.size()))pass.index=0;
                sum=tap-.72f*combined;
            }
            const float shimmer=lp(sum,s.lp1,1300.f+a*5000.f,sr_);
            y=(1.f-d)*x+d*shimmer;
            break;
        }
        case Model::SpringGarden: {
            // Short bright reflective taps and strongly differentiated spring-like splash.
            const float dt=(.022f+.045f*a)*float(sr_);
            const float first=s.delay.read(dt,s.age);
            const float second=s.delay.read(dt*1.37f,s.age);
            const float splash=first*.54f-second*.37f;
            const float resonant=hp(splash,s.lp1,550.f,sr_);
            const float tone=lp(resonant,s.lp2,750.f+10000.f*c,sr_);
            s.delay.write(saturate(x*.7f+tone*(.2f+.65f*b)));
            y=(1.f-d)*x+d*tone;
            break;
        }
        case Model::OctaveCrown: {
            // Monophonic analog-style octave synthesis, not a polyphonic tracker.
            const float up=hp(std::abs(x),s.lp1,110.f,sr_)*2.f;
            if(s.prevInput<=0.f && x>0.f)s.sub=-s.sub;
            s.prevInput=x;
            const float sub=lp(s.sub*std::abs(x),s.lp2,300.f+c*1300.f,sr_);
            const float effect=a*up+b*sub;
            y=(1.f-d)*x+d*effect;
            break;
        }
        case Model::SubmarineVoice: {
            // Zero crossing divider: monophonic and attack sensitive by design.
            if(s.prevInput<=0.f&&x>0.f)s.sub=-s.sub;
            s.prevInput=x;
            const float envelope=lp(std::abs(x),s.lp1,14.f+(1.f-b)*140.f,sr_);
            const float sub=lp(s.sub*envelope,s.lp2,90.f+c*900.f,sr_);
            y=(1.f-d)*x+d*a*2.f*sub;
            break;
        }
        case Model::TimeGarden: {
            const float phase=cycle(s.phase,.05f+a*3.f,sr_);
            const float wow=.003f*std::sin(tau*(phase+channel*.28f));
            const float ds=(.06f+b*1.05f+wow)*float(sr_);
            const float delayed=s.delay.read(ds,s.age);
            const float echo=lp(delayed,s.lp1,700.f+c*7100.f,sr_);
            s.delay.write(saturate(x*.65f+echo*.71f));
            y=(1.f-d)*x+d*echo;
            break;
        }
        case Model::InfiniteDrift: {
            const float phase=cycle(s.phase,.035f+a*1.7f,sr_);
            const float slow=std::sin(tau*(phase+channel*.13f));
            const float ds=(.15f+.43f*(.5f+.5f*slow))*float(sr_);
            const float delayed=s.delay.read(ds,s.age);
            const float fed=lp(delayed,s.lp1,450.f+c*4400.f,sr_);
            s.delay.write(saturate(x*.55f+fed*(.24f+b*.68f)));
            y=(1.f-d)*x+d*fed;
            break;
        }
        default: return x;
    }
    ++s.age;
    if (!std::isfinite(y)) { s.resetScalars(); return 0.f; }
    return clamp(y,-16.f,16.f);
}

void Engine::process(float* const* audio, int channels, int numSamples,
                     const Settings& settings) noexcept {
    if (!audio || numSamples<=0 || channels<1 || channels>preparedChannels_) return;
    for (int i=0; i<kSlots; ++i) {
        auto& slot = states_[size_t(i)];
        const auto model = settings.slots[size_t(i)].model;
        if (model!=slot.lastModel) {
            slot.lastModel=model;
            slot.wet=0.f; // fade from dry on model replacement
            slot.smoothed=settings.slots[size_t(i)].controls;
            slot.smoothedMix=std::clamp(settings.slots[size_t(i)].mix,0.f,1.f);
            slot.smoothedLevel=db(std::clamp(settings.slots[size_t(i)].levelDb,-18.f,18.f));
            for (auto& ch : slot.channel) ch.resetScalars(); // no vector clear/allocation in callback
        }
    }
    const float inGain = db(clamp(settings.inputDb,-24.f,24.f));
    const float outGain = db(clamp(settings.outputDb,-24.f,24.f));
    const float globalMix = clamp(settings.globalMix,0.f,1.f);
    const float targetOn=settings.globalBypass?0.f:1.f;
    const bool parallel = settings.routing == RoutingMode::Parallel;
    if (settings.routing != lastRouting_) {
        lastRouting_ = settings.routing;
        routingFadeSamples_ = routingFadeLength_; // 6ms transition, no abrupt topology jump.
    }
    if (parallel) {
        for(int i=0;i<kSlots;++i) {
            const int branch=std::clamp(settings.slots[size_t(i)].branch,0,1);
            if(branch!=lastBranches_[size_t(i)]) {
                lastBranches_[size_t(i)]=branch;
                routingFadeSamples_=routingFadeLength_;
            }
        }
    }
    const float smooth = clamp(1.f-std::exp(-1.f/(float(sr_)*.008f)),.00001f,1.f);
    if (!masterInitialized_) {
        smoothedInputGain_=inGain;
        smoothedOutputGain_=outGain;
        smoothedMix_=globalMix;
        smoothedGlobalOn_=targetOn;
        smoothedParallelBlend_=clamp(settings.parallelBlend,0.f,1.f);
        masterInitialized_=true;
    }
    for (int frame=0; frame<numSamples; ++frame) {
        smoothedInputGain_ += smooth*(inGain-smoothedInputGain_);
        smoothedOutputGain_ += smooth*(outGain-smoothedOutputGain_);
        smoothedMix_ += smooth*(globalMix-smoothedMix_);
        smoothedGlobalOn_ += smooth*(targetOn-smoothedGlobalOn_);
        smoothedParallelBlend_ += smooth*(clamp(settings.parallelBlend,0.f,1.f)-smoothedParallelBlend_);
        float dry[2]{};
        float wet[2]{};
        float lane[2][2]{};
        for (int ch=0;ch<channels;++ch) {
            dry[ch]=audio[ch][frame];
            wet[ch]=dry[ch]*smoothedInputGain_;
            lane[0][ch]=wet[ch];
            lane[1][ch]=wet[ch]; // Real split: both lanes receive the unprocessed input.
        }
        for (int i=0;i<kSlots;++i) {
            auto& slot=states_[size_t(i)];
            const auto& cfg=settings.slots[size_t(i)];
            const float target=(cfg.enabled && cfg.model!=Model::Empty)?1.f:0.f;
            slot.wet += smooth*(target-slot.wet);
            for (int k=0;k<kControls;++k)
                slot.smoothed[size_t(k)] += smooth*(clamp(cfg.controls[size_t(k)],0.f,1.f)-slot.smoothed[size_t(k)]);
            slot.smoothedMix+=smooth*(clamp(cfg.mix,0.f,1.f)-slot.smoothedMix);
            slot.smoothedLevel+=smooth*(db(clamp(cfg.levelDb,-18.f,18.f))-slot.smoothedLevel);
            if (slot.wet<.00001f) continue;
            const int branch=std::clamp(cfg.branch,0,1);
            for (int ch=0;ch<channels;++ch) {
                float& signal=parallel?lane[branch][ch]:wet[ch];
                const float processed=processOne(cfg.model,slot.channel[size_t(ch)],ch,signal,slot.smoothed);
                const float mixed=signal*(1.f-slot.smoothedMix)+processed*slot.smoothedMix;
                signal+=slot.wet*(mixed*slot.smoothedLevel-signal);
            }
        }
        for (int ch=0;ch<channels;++ch) {
            // Two separately processed serial chains are combined after their last stage.
            // Linear unity-gain blending avoids doubling fully correlated signals.
            const float routed=parallel?(lane[0][ch]*(1.f-smoothedParallelBlend_)+
                                         lane[1][ch]*smoothedParallelBlend_):wet[ch];
            const float processed=(dry[ch]*(1.f-smoothedMix_)+routed*smoothedMix_)*smoothedOutputGain_;
            const float result=dry[ch]+smoothedGlobalOn_*(processed-dry[ch]);
            const float stable=std::isfinite(result)?clamp(result,-16.f,16.f):0.f;
            if(routingFadeSamples_>0) {
                const float t=float(routingFadeSamples_)/float(routingFadeLength_);
                audio[ch][frame]=previousOutput_[size_t(ch)]*t+stable*(1.f-t);
            } else audio[ch][frame]=stable;
            previousOutput_[size_t(ch)]=audio[ch][frame];
        }
        if(routingFadeSamples_>0) --routingFadeSamples_;
    }
}
} // namespace pedal
