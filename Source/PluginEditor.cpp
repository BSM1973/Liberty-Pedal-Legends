#include "PluginEditor.h"
#include "UILocalization.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace {
using R = juce::Rectangle<float>;
const juce::Colour midnight{0xff071728},navy{0xff0c263d},sapphire{0xff153651},
    navySoft{0xff22445f},cream{0xfff3e7cb},paper{0xffeee0c3},
    gold{0xffd5aa66},goldLight{0xfff3d59a},goldDark{0xff946733},
    ink{0xff142c3e},muted{0xff8799a0},amber{0xffffb846};
constexpr float pi=juce::MathConstants<float>::pi;
void surface(juce::Graphics& g,R r,juce::Colour top,juce::Colour bottom,float radius=12.f,
             juce::Colour outline=goldDark) {
    g.setColour(midnight.withAlpha(.6f)); g.fillRoundedRectangle(r.translated(1.7f,4.f),radius);
    g.setGradientFill(juce::ColourGradient(top,r.getTopLeft(),bottom,r.getBottomRight(),false));
    g.fillRoundedRectangle(r,radius);
    g.setColour(outline.withAlpha(.8f));g.drawRoundedRectangle(r.reduced(.9f),radius,1.5f);
    g.setColour(goldLight.withAlpha(.15f));g.drawRoundedRectangle(r.reduced(3.f),radius-2.f,.8f);
}
void line(juce::Graphics& g,float x1,float y1,float x2,float y2,juce::Colour c,float thick=1.f) {
    g.setColour(c);g.drawLine(x1,y1,x2,y2,thick);
}
juce::String familyText(std::string_view family) {
    const juce::String f{std::string(family)};
    if(f=="Compressor")return "COMPRESSOR & LIMITER";
    if(f=="EQ / Filter")return "EQ & FILTER";
    if(f=="Delay / Echo")return "DELAY & ECHO";
    return f.toUpperCase();
}
juce::String yearText(std::string_view era) {
    juce::String s{std::string(era)};
    if(s.contains("1970")||s.contains("1973"))return "1973";
    if(s.contains("1980"))return "1985";
    if(s.contains("1990"))return "1995";
    if(s.contains("2000"))return "2004";
    if(s.contains("2010"))return "2015";
    if(s.contains("2020"))return "2026";
    return "2026";
}
juce::String yearForModel(pedal::Model m) {
    // Reference-screen era captions are storytelling labels, not hardware-clone claims.
    switch (m) {
        case pedal::Model::OpticalSilk: return "2004";
        case pedal::Model::VerdantDrive: return "1979";
        case pedal::Model::SculptEQ: return "1982";
        case pedal::Model::AzureChorus: return "1984";
        case pedal::Model::TapeVoyager: return "1973";
        case pedal::Model::GoldenPlate: return "1985";
        default: return yearText(pedal::info(m).era);
    }
}
juce::Colour pedalPaint(pedal::Model m) {
    static constexpr unsigned shades[]={0xff1e4769,0xff145273,0xff254661,0xff32658c,
                                        0xff1a4162,0xff45728c,0xff233952,0xff164b67};
    const int index=std::clamp(int(m),1,pedal::kModelCount-1);
    return juce::Colour(shades[(index*17+index/5)%8]);
}
juce::String soundDescription(pedal::Model m) {
    const auto f=juce::String(std::string(pedal::info(m).family));
    if(f=="Overdrive")return juce::String::fromUTF8("Saturation progressive, harmonies chaleureuses et attaque de guitare préservée.");
    if(f=="Distortion")return juce::String::fromUTF8("Distorsion articulée, grain dense et réponse précise aux variations de jeu.");
    if(f=="Fuzz")return "Texture vintage riche en harmoniques, de la douceur aux sons les plus rugueux.";
    if(f=="Compressor")return juce::String::fromUTF8("Compression souple inspirée des grands compresseurs de pédalier et de studio.");
    if(f=="EQ / Filter")return juce::String::fromUTF8("Sculpture tonale expressive avec fréquences et résonances musicales.");
    if(f=="Wah / Envelope")return juce::String::fromUTF8("Filtrage résonant vivant, idéal pour un jeu rythmique et expressif.");
    if(f=="Modulation")return juce::String::fromUTF8("Mouvement enveloppant, profondeur et caractère inspirés des modulations classiques.");
    if(f=="Tremolo")return "Variation rythmique et mouvement du signal, du vintage au contemporain.";
    if(f=="Delay / Echo")return juce::String::fromUTF8("Répétitions musicales aux couleurs analogiques et numériques distinctives.");
    if(f=="Reverb")return juce::String::fromUTF8("Espace et profondeur, avec décroissance musicale et détail des transitoires.");
    if(f=="Pitch / Octave")return "Effet d'octave monophonique et nuances harmoniques pour guitare et basse.";
    if(f=="Experimental")return juce::String::fromUTF8("Textures évolutives et délais créatifs pour l'expérimentation sonore.");
    return juce::String::fromUTF8("Préamplification dynamique et couleur musicale pour votre guitare.");
}
juce::String historicalInfluence(pedal::Model m) {
    using M=pedal::Model;
    switch(m) {
        case M::VerdantDrive:return juce::String::fromUTF8("Ibanez Tube Screamer — inspiration de médiums et d'overdrive classique");
        case M::GoldenArc:return juce::String::fromUTF8("Klon Centaur — famille des overdrives transparentes et dynamiques");
        case M::AmberBreaker:return juce::String::fromUTF8("Marshall Bluesbreaker — inspiration du crunch blues britannique");
        case M::OrangeVoltage:return juce::String::fromUTF8("Pro Co RAT — famille des distorsions rock avec filtrage marqué");
        case M::SteelFang:return juce::String::fromUTF8("Boss DS-1 — grande tradition des distorsions compactes");
        case M::GermaniumSun:return juce::String::fromUTF8("Dallas-Arbiter Fuzz Face — tempérament germanium vintage");
        case M::SiliconStorm:return juce::String::fromUTF8("Fuzz Face silicium — caractère plus mordant et affirmé");
        case M::OpticalSilk:return juce::String::fromUTF8("Diamond Compressor — compression optique douce, référence esthétique générale");
        case M::VelvetSustain:return juce::String::fromUTF8("Ross Compressor — esprit de compression vintage et sustain");
        case M::AzureChorus:return juce::String::fromUTF8("Boss CE-2 — richesse du chorus analogique");
        case M::OrbitPhase:return juce::String::fromUTF8("MXR Phase 90 — phasing analogique classique");
        case M::CelestialFlange:return juce::String::fromUTF8("Electro-Harmonix Electric Mistress — esprit du flanging vintage");
        case M::TapeVoyager:return juce::String::fromUTF8("Maestro Echoplex — échos à bande et saturation du temps");
        case M::MemoryTrail:return juce::String::fromUTF8("Electro-Harmonix Deluxe Memory Man — inspiration de délai à modulation");
        case M::SpringGarden:return juce::String::fromUTF8("Réverbérations à ressort des amplificateurs vintage");
        case M::GoldenPlate:return juce::String::fromUTF8("Réverbérations à plaque de studio des années 1970");
        case M::SculptEQ:return juce::String::fromUTF8("Égaliseurs de tonalité analogiques pour guitare");
        case M::VocalWah:return juce::String::fromUTF8("Vox et Cry Baby — familles de pédales wah emblématiques");
        default: return juce::String::fromUTF8("Pédales et appareils de la famille ")+juce::String(std::string(pedal::info(m).family))+
                        juce::String::fromUTF8(" — inspiration générale de période");
    }
}
}

PedalLegendsAudioProcessorEditor::PedalLegendsAudioProcessorEditor(PedalLegendsAudioProcessor& p)
: juce::AudioProcessorEditor(&p),processor(p) {
    setOpaque(true);
    setResizable(true,true);
    setResizeLimits(1072,624,2160,1296);
    setSize(1600,930);
    setWantsKeyboardFocus(true);
    currentLanguage=liberty_ui::safeLanguage(p.getUILanguage());
    languageSelector.addItem(juce::String::fromUTF8("Français")),1);
    languageSelector.addItem("English",2);
    languageSelector.addItem(juce::String::fromUTF8("Español")),3);
    languageSelector.setColour(juce::ComboBox::backgroundColourId,navy);
    languageSelector.setColour(juce::ComboBox::textColourId,goldLight);
    languageSelector.setColour(juce::ComboBox::outlineColourId,goldDark);
    languageSelector.setColour(juce::ComboBox::arrowColourId,gold);
    languageSelector.setJustificationType(juce::Justification::centred);
    languageSelector.setSelectedId(currentLanguage+1,juce::dontSendNotification);
    languageSelector.onChange=[this] {languageChanged();};
    addAndMakeVisible(languageSelector);
    search.setTextToShowWhenEmpty(localized("Rechercher..."),muted);
    search.setColour(juce::TextEditor::backgroundColourId,paper);
    search.setColour(juce::TextEditor::textColourId,ink);
    search.setColour(juce::TextEditor::outlineColourId,goldDark);
    search.setColour(juce::TextEditor::focusedOutlineColourId,gold);
    search.setFont(juce::Font(juce::FontOptions(17.f)));
    search.onTextChange=[this] {updateSearch();};
    addAndMakeVisible(search);
    search.setVisible(false);
    snapshotA=p.snapshot();snapshotB=snapshotA;
    gateA=gateB=p.gateEnabled();
    thresholdA=thresholdB=p.gateThreshold();
    startTimerHz(15);
}
juce::String PedalLegendsAudioProcessorEditor::localized(const juce::String& text) const {
    return liberty_ui::translate(text,currentLanguage);
}
void PedalLegendsAudioProcessorEditor::languageChanged() {
    if(languageSelector.getSelectedId()<1)return;
    currentLanguage=liberty_ui::safeLanguage(languageSelector.getSelectedId()-1);
    processor.setUILanguage(currentLanguage);
    search.setTextToShowWhenEmpty(localized("Rechercher..."),muted);
    repaint();
}
void PedalLegendsAudioProcessorEditor::updateSearch() {
    browserOffset=libraryOffset=0;
    repaint();
}
void PedalLegendsAudioProcessorEditor::setScreen(Screen next) {
    screen=next;
    search.setVisible(next==Screen::Presets||next==Screen::Library);
    languageSelector.setVisible(next==Screen::Board);
    if(search.isVisible()) {
        search.clear();
        resized();
        search.grabKeyboardFocus();
    }
    repaint();
}
void PedalLegendsAudioProcessorEditor::selectSlot(int slot) {
    selectedSlot=std::clamp(slot,0,7);
    setScreen(Screen::Editor);
}
void PedalLegendsAudioProcessorEditor::openHistory(int slot) {
    selectedSlot=std::clamp(slot,0,7);
    const auto s=processor.snapshot().slots[size_t(selectedSlot)];
    historyModel=std::max(1,int(s.model));
    historyTab=0;
    setScreen(Screen::History);
}
void PedalLegendsAudioProcessorEditor::showModel(int model) {
    const auto m=std::clamp(model,0,pedal::kModelCount-1);
    processor.setModel(selectedSlot,pedal::Model(m));
    setScreen(Screen::Board);
}
void PedalLegendsAudioProcessorEditor::toggleAB() {
    if(!viewingB) {
        snapshotA=processor.snapshot();
        gateA=processor.gateEnabled();thresholdA=processor.gateThreshold();
        processor.restoreSettings(snapshotB);
        processor.setGateEnabled(gateB);processor.setGateThreshold(thresholdB);
    } else {
        snapshotB=processor.snapshot();
        gateB=processor.gateEnabled();thresholdB=processor.gateThreshold();
        processor.restoreSettings(snapshotA);
        processor.setGateEnabled(gateA);processor.setGateThreshold(thresholdA);
    }
    viewingB=!viewingB;
    repaint();
}
void PedalLegendsAudioProcessorEditor::savePreset() {
    const auto presets=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                           .getChildFile("Liberty Music Software/Pedal Legends/Presets");
    presets.createDirectory();
    auto name=juce::String(pedal::factoryPresets()[size_t(processor.selectedFactoryPreset())].name)
                   .retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-");
    fileChooser=std::make_unique<juce::FileChooser>(juce::String::fromUTF8("Enregistrer le pédalier"),presets.getChildFile(name+".plegends"),"*.plegends");
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode|
                             juce::FileBrowserComponent::canSelectFiles|
                             juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& chooser) {
            auto file=chooser.getResult();
            if(file==juce::File{})return;
            if(!file.hasFileExtension(".plegends"))file=file.withFileExtension(".plegends");
            juce::MemoryBlock block;processor.getStateInformation(block);
            if(file.replaceWithData(block.getData(),block.getSize()))lastSavedPreset=file.getFileNameWithoutExtension();
            repaint();
        });
}
void PedalLegendsAudioProcessorEditor::openUserPreset() {
    const auto folder=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            .getChildFile("Liberty Music Software/Pedal Legends/Presets");
    fileChooser=std::make_unique<juce::FileChooser>(juce::String::fromUTF8("Charger un pédalier"),folder,"*.plegends");
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode|
                            juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser) {
            auto file=chooser.getResult();
            if(!file.existsAsFile())return;
            juce::MemoryBlock block;
            if(file.loadFileAsData(block)&&block.getSize()>0) {
                processor.setStateInformation(block.getData(),int(block.getSize()));
                lastSavedPreset=file.getFileNameWithoutExtension();
                setScreen(Screen::Board);
            }
        });
}
void PedalLegendsAudioProcessorEditor::timerCallback() {
    const int saved=liberty_ui::safeLanguage(processor.getUILanguage());
    if(saved!=currentLanguage) {
        currentLanguage=saved;
        languageSelector.setSelectedId(saved+1,juce::dontSendNotification);
        search.setTextToShowWhenEmpty(localized("Rechercher..."),muted);
    }
    repaint();
}
juce::Point<float> PedalLegendsAudioProcessorEditor::logicalPoint(juce::Point<int> p) const {
    return {(float(p.x)-xOffset)/scale,(float(p.y)-yOffset)/scale};
}
void PedalLegendsAudioProcessorEditor::resized() {
    scale=std::min(getWidth()/canvasWidth,getHeight()/canvasHeight);
    xOffset=(getWidth()-canvasWidth*scale)*.5f;
    yOffset=(getHeight()-canvasHeight*scale)*.5f;
    const auto menu=R{1138.f,48.f,151.f,33.f};
    languageSelector.setBounds(juce::Rectangle<int>(int(xOffset+menu.getX()*scale),int(yOffset+menu.getY()*scale),
                                                    int(menu.getWidth()*scale),int(menu.getHeight()*scale)));
    auto r=R(385.f,190.f,700.f,37.f);
    search.setBounds(juce::Rectangle<int>(int(xOffset+r.getX()*scale),int(yOffset+r.getY()*scale),
                                            int(r.getWidth()*scale),int(r.getHeight()*scale)));
}
void PedalLegendsAudioProcessorEditor::drawLabel(juce::Graphics& g,const juce::String& s,R r,
                                                  float size,juce::Colour colour,bool serif,
                                                  juce::Justification alignment,bool bold) {
    g.setColour(colour);
    const auto fontName=serif?juce::Font::getDefaultSerifFontName():juce::Font::getDefaultSansSerifFontName();
    g.setFont(juce::Font(juce::FontOptions(fontName,size,bold?juce::Font::bold:juce::Font::plain)));
    const auto cleaned=liberty_ui::safeTypography(localized(s));
    g.drawFittedText(cleaned,r.toNearestInt(),alignment,2,.86f);
}
void PedalLegendsAudioProcessorEditor::hit(R r,std::function<void()> fn) {
    hotspots.push_back({r,std::move(fn),{},0.f});
}
void PedalLegendsAudioProcessorEditor::drag(R r,float value,std::function<void(float)> fn) {
    hotspots.push_back({r,{},std::move(fn),std::clamp(value,0.f,1.f)});
}
void PedalLegendsAudioProcessorEditor::button(juce::Graphics& g,R r,const juce::String& s,
                                              std::function<void()> fn,bool filled,bool selected,float size) {
    surface(g,r,filled?goldLight:(selected?sapphire:navySoft),filled?gold:navy,
             std::min(6.f,r.getHeight()*.24f),goldDark);
    drawLabel(g,s,r.reduced(4.f),size,filled?ink:(selected?goldLight:cream),false,
              juce::Justification::centred,true);
    if(fn)hit(r,std::move(fn));
}
void PedalLegendsAudioProcessorEditor::drawKnob(juce::Graphics& g,float x,float y,float radius,
                                                float value,const juce::String& title,
                                                const juce::String& display,std::function<void(float)> setter,bool darkBackground) {
    const R shadow{x-radius+2.f,y-radius+3.f,radius*2,radius*2};
    g.setColour(midnight.withAlpha(.7f));g.fillEllipse(shadow);
    g.setGradientFill(juce::ColourGradient(goldLight,{x-radius,y-radius},goldDark,{x+radius,y+radius},false));
    g.fillEllipse(x-radius,y-radius,2*radius,2*radius);
    g.setColour(ink);g.drawEllipse(x-radius+3.f,y-radius+3.f,2*radius-6,2*radius-6,2.f);
    g.setGradientFill(juce::ColourGradient(cream,{x-radius*.65f,y-radius*.8f},gold,{x+radius*.6f,y+radius*.8f},false));
    g.fillEllipse(x-radius*.7f,y-radius*.7f,radius*1.4f,radius*1.4f);
    const float angle=(.75f+std::clamp(value,0.f,1.f)*1.5f)*pi;
    line(g,x,y,x+std::cos(angle)*radius*.62f,y+std::sin(angle)*radius*.62f,ink,2.7f);
    for(int k=0;k<=10;++k) {
        const float a=(.75f+1.5f*float(k)/10.f)*pi;
        const auto dx=std::cos(a),dy=std::sin(a);
        line(g,x+dx*(radius+2.f),y+dy*(radius+2.f),x+dx*(radius+6.f),y+dy*(radius+6.f),goldDark,1.f);
    }
    drawLabel(g,title,R{x-radius-45.f,y+radius+13.f,2*radius+90.f,19.f},13.f,darkBackground?cream:ink,false,
              juce::Justification::centred,true);
    if(display.isNotEmpty()) {
        const auto box=R{x-54.f,y+radius+38.f,108.f,26.f};
        g.setColour(goldDark.withAlpha(.12f));g.fillRoundedRectangle(box,4.f);
        g.setColour(goldDark.withAlpha(.35f));g.drawRoundedRectangle(box,4.f,.9f);
        drawLabel(g,display,box,13.f,darkBackground?cream:ink,false,juce::Justification::centred);
    }
    if(setter)drag(R{x-radius-7.f,y-radius-7.f,2*(radius+7.f),2*(radius+7.f)},value,std::move(setter));
}
void PedalLegendsAudioProcessorEditor::drawBackplate(juce::Graphics& g) {
    g.fillAll(midnight);
    g.setGradientFill(juce::ColourGradient(sapphire,{0.f,0.f},midnight,{0.f,930.f},false));
    g.fillRect(R{0,0,1600,930});
    for(int n=1;n<100;++n) {
        const float y=float(n)*9.35f;
        line(g,0.f,y,1600.f,y,cream.withAlpha(n%3==0?.021f:.007f));
    }
    line(g,20.f,118.f,1580.f,118.f,goldDark.withAlpha(.7f),1.5f);
    // Liberty monogram, embossed enclosure-inspired badge.
    juce::Path badge;badge.addQuadrilateral(61.f,27.f,85.f,54.f,61.f,80.f,37.f,54.f);
    g.setColour(goldDark);g.strokePath(badge,juce::PathStrokeType(3.f));
    drawLabel(g,"L",R{45,32,33,41},30,goldLight,true,juce::Justification::centred,true);
    drawLabel(g,"LIBERTY MUSIC SOFTWARE",R{106,17,310,19},13,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,"PEDAL LEGENDS",R{106,34,365,49},36,goldLight,true,juce::Justification::centredLeft);
    drawLabel(g,"Vintage Soul. Modern Power.",R{107,83,360,22},16,gold,true);
    surface(g,R{492,17,626,84},navySoft,midnight,10.f,gold);
    drawLabel(g,"LANGUE",R{1141,23,146,19},11,goldLight,false,juce::Justification::centred,true);
    drawLabel(g,processor.isPresetModified()?juce::String::fromUTF8("• MODIFICATIONS NON ENREGISTRÉES"):juce::String::fromUTF8("• FACTORY / USER"),
              R{513,23,290,18},11,gold,false,juce::Justification::centredLeft,true);
    const int index=std::clamp(processor.selectedFactoryPreset(),0,99);
    const auto display=lastSavedPreset.isNotEmpty()?lastSavedPreset:
        juce::String(pedal::factoryPresets()[size_t(index)].name);
    drawLabel(g,display,R{514,46,360,42},23,cream,true);
    drawLabel(g,"FACTORY / USER",R{838,27,140,18},11,muted,false,juce::Justification::centred,true);
    button(g,R{911,61,30,30},"<",[this]{processor.loadFactoryPreset((processor.selectedFactoryPreset()+99)%100);},false,false,18.f);
    button(g,R{944,61,30,30},">",[this]{processor.loadFactoryPreset((processor.selectedFactoryPreset()+1)%100);},false,false,18.f);
    button(g,R{983,47,120,41},"BROWSE  100",[this]{setScreen(Screen::Presets);},true,false,13.f);
    button(g,R{1306,18,67,37},"SAVE",[this]{savePreset();},true,false,12.f);
    button(g,R{1382,18,75,37},viewingB?"A / [B]":"[A] / B",[this]{toggleAB();},true,false,12.f);
    const bool bypass=processor.snapshot().globalBypass;
    button(g,R{1465,18,112,37},bypass?"BYPASSED":"BYPASS",
           [this,bypass]{processor.setGlobal(3,bypass?0.f:1.f);},true,bypass,12.f);
    drawMeter(g,R{1338,78,95,12},processor.inputPeak(),"IN");
    drawMeter(g,R{1487,78,88,12},processor.outputPeak(),"OUT");
}
void PedalLegendsAudioProcessorEditor::drawMeter(juce::Graphics& g,R box,float peak,const juce::String& label) {
    g.setColour(midnight);g.fillRoundedRectangle(box,3.f);
    g.setColour(goldDark.withAlpha(.8f));g.drawRoundedRectangle(box,3.f,1.f);
    const float level=std::clamp(std::sqrt(std::max(0.f,peak)),0.f,1.f);
    const int segments=18;
    for(int n=0;n<segments;++n) {
        const bool lit=level*segments>float(n);
        g.setColour(lit?(n>15?juce::Colours::orangered:goldLight):navySoft);
        const float x=box.getX()+3.f+float(n)*(box.getWidth()-6.f)/segments;
        g.fillRect(R{x,box.getY()+3.f,(box.getWidth()-8.f)/segments,box.getHeight()-6.f});
    }
    drawLabel(g,label,R{box.getX()-48.f,box.getY()-1.f,44.f,box.getHeight()+3.f},11,goldLight,false,juce::Justification::centredRight,true);
}
void PedalLegendsAudioProcessorEditor::drawPedal(juce::Graphics& g,R r,pedal::Model model,
                                                  const pedal::SlotSettings& state,bool large) {
    if(model==pedal::Model::Empty)return;
    const auto paint=pedalPaint(model);
    g.setColour(midnight.withAlpha(.8f));g.fillRoundedRectangle(r.translated(5.f,8.f),15.f);
    g.setGradientFill(juce::ColourGradient(paint.brighter(.25f),r.getTopLeft(),paint.darker(.4f),r.getBottomRight(),false));
    g.fillRoundedRectangle(r,13.f);
    g.setColour(goldDark);g.drawRoundedRectangle(r.reduced(2.f),12.f,2.f);
    g.setColour(goldLight.withAlpha(.38f));g.drawRoundedRectangle(r.reduced(5.f),10.f,.9f);
    for(float y=r.getY()+12.f;y<r.getBottom()-10.f;y+=4.f) {
        g.setColour(cream.withAlpha(.022f));g.drawHorizontalLine(int(y),r.getX()+7.f,r.getRight()-7.f);
    }
    for(float x:{r.getX()+14.f,r.getRight()-14.f})for(float y:{r.getY()+13.f,r.getBottom()-13.f}) {
        g.setColour(goldLight);g.fillEllipse(x-3.f,y-3.f,6.f,6.f);
        line(g,x-2.f,y+1.f,x+2.f,y-1.f,ink,.7f);
    }
    drawLabel(g,juce::String::fromUTF8("L I B E R T Y   —   ")+yearForModel(model),
              R{r.getX()+15,r.getY()+7,r.getWidth()-30,17},large?12.f:9.f,goldLight,false,
              juce::Justification::centred,true);
    drawLabel(g,juce::String(std::string(pedal::info(model).name)),
              R{r.getX()+13,r.getY()+24,r.getWidth()-26,38},large?29.f:25.f,goldLight,true,
              juce::Justification::centred,true);
    drawLabel(g,familyText(pedal::info(model).family),
              R{r.getX()+13,r.getY()+63,r.getWidth()-26,16},large?11.f:9.f,cream,false,
              juce::Justification::centred,true);
    const float baseY=r.getY()+r.getHeight()*.52f;
    for(int i=0;i<3;++i) {
        const float x=r.getX()+r.getWidth()*(.205f+.295f*i);
        const float rad=large?23.f:17.f;
        g.setColour(midnight.withAlpha(.7f));g.fillEllipse(x-rad-2,baseY-rad+3,2*rad+4,2*rad+4);
        g.setGradientFill(juce::ColourGradient(goldLight,{x-rad,baseY-rad},goldDark,{x+rad,baseY+rad},false));
        g.fillEllipse(x-rad,baseY-rad,rad*2,rad*2);
        g.setColour(ink);g.drawEllipse(x-rad+3,baseY-rad+3,rad*2-6,rad*2-6,1.7f);
        const float a=(.75f+state.controls[size_t(i)]*1.5f)*pi;
        line(g,x,baseY,x+std::cos(a)*rad*.7f,baseY+std::sin(a)*rad*.7f,ink,2.2f);
        drawLabel(g,juce::String(std::string(pedal::info(model).controls[size_t(i)])),
                  R{x-43,baseY+rad+3,86,14},large?11.f:9.f,cream,false,juce::Justification::centred,true);
    }
    const float switchX=r.getCentreX(),switchY=r.getBottom()-37.f;
    g.setColour(goldLight.withAlpha(.3f));
    g.drawEllipse(switchX-43.f,switchY-13.f,86.f,26.f,.9f);
    g.setColour(goldDark);g.fillEllipse(switchX-17,switchY-17,34,34);
    g.setGradientFill(juce::ColourGradient(cream,{switchX-14,switchY-14},goldDark,{switchX+14,switchY+14},false));
    g.fillEllipse(switchX-13,switchY-13,26.f,26.f);
    g.setColour(state.enabled?amber:navy);
    g.fillEllipse(switchX-60,switchY-5,9.f,9.f);
    drawLabel(g,state.enabled?"ENGAGED":"BYPASS",R{switchX+22,switchY-10,68,19},9,cream,false,juce::Justification::centredLeft);
    drawLabel(g,juce::String::fromUTF8("ORIGINAL MODEL  ·  LMS"),R{r.getX()+16,r.getBottom()-14,r.getWidth()-32,12},
              8.f,goldLight.withAlpha(.85f),false,juce::Justification::centred,true);
}
void PedalLegendsAudioProcessorEditor::drawBoard(juce::Graphics& g) {
    surface(g,R{28,139,1544,641},navySoft,midnight,18.f,goldDark);
    drawLabel(g,juce::String::fromUTF8("• THE LIBERTY COLLECTION"),R{68,149,330,27},15,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,"40 ORIGINAL PEDALS",R{322,151,185,22},12,gold,false,juce::Justification::centredLeft,true);
    drawLabel(g,juce::String::fromUTF8("IN   →   1  →  2  →  3  →  4   →   5  →  6  →  7  →  8   →   OUT"),
              R{1000,149,534,25},12,goldLight,false,juce::Justification::centredRight,true);
    const auto settings=processor.snapshot();
    std::array<R,pedal::kSlots> rects{};
    const float left=56.f,w=355.f,space=26.f;
    for(int i=0;i<8;++i) {
        const int row=i/4,col=i%4;
        rects[size_t(i)]={left+col*(w+space),177.f+row*294.f,w,281.f};
    }
    // Gold cable follows the real eight-slot order: left to right on both rows.
    // The inter-row U return is routed behind the transparent card surfaces.
    g.setColour(goldDark.withAlpha(.65f));
    juce::Path routing;
    for(int k=0;k<3;++k){
        const auto a=rects[size_t(k)],b=rects[size_t(k+1)];
        juce::Path segment;
        segment.startNewSubPath(a.getRight()-34,a.getY()+166.f);
        segment.cubicTo(a.getRight()+15,a.getY()+166,b.getX()-17,b.getY()+166,b.getX()+37,b.getY()+166);
        g.strokePath(segment,juce::PathStrokeType(6.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    }
    juce::Path returnLine;
    returnLine.startNewSubPath(rects[3].getRight()-35,rects[3].getY()+168);
    returnLine.cubicTo(1588.f,340.f,1587.f,466.f,1525.f,467.f);
    returnLine.lineTo(70.f,467.f);
    returnLine.cubicTo(42.f,467.f,38.f,504.f,95.f,565.f);
    g.strokePath(returnLine,juce::PathStrokeType(5.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    for(int k=4;k<7;++k){
        const auto a=rects[size_t(k)],b=rects[size_t(k+1)];
        juce::Path segment;
        segment.startNewSubPath(a.getRight()-34,a.getY()+166.f);
        segment.cubicTo(a.getRight()+15,a.getY()+166,b.getX()-17,b.getY()+166,b.getX()+37,b.getY()+166);
        g.strokePath(segment,juce::PathStrokeType(6.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    }
    for(int i=0;i<pedal::kSlots;++i) {
        const auto box=rects[size_t(i)];
        const auto& stage=settings.slots[size_t(i)];
        const bool has=stage.model!=pedal::Model::Empty;
        const auto& metadata=pedal::info(stage.model);
        surface(g,box,navy.withAlpha(.96f),midnight,10.f,selectedSlot==i?goldDark:navySoft);
        drawLabel(g,juce::String(i+1).paddedLeft('0',2),R{box.getX()+15,box.getY()+8,38,25},21,goldLight,true,
                  juce::Justification::centredLeft,true);
        drawLabel(g,has?familyText(metadata.family):"OPEN STAGE",
                  R{box.getX()+53,box.getY()+11,146,20},11,cream,false,juce::Justification::centredLeft,true);
        if(has) {
            button(g,R{box.getRight()-165,box.getY()+8,108,23},"HISTORY & SOUND",
                   [this,i]{openHistory(i);},false,false,9.5f);
            button(g,R{box.getRight()-50,box.getY()+8,42,23},stage.enabled?"ON":"BYPASS",
                   [this,i,on=stage.enabled]{processor.changeSlotEnabled(i,!on);},false,stage.enabled,9.5f);
            const R enclosure{box.getX()+37,box.getY()+39,box.getWidth()-74,202};
            drawPedal(g,enclosure,stage.model,stage);
            // Clicking the enclosure opens the focused editor; the actual
            // footswitch has a more specific hotspot added afterward.
            hit(enclosure,[this,i]{selectSlot(i);});
            hit(R{enclosure.getCentreX()-26,enclosure.getBottom()-63,52,52},
                [this,i,on=stage.enabled]{processor.changeSlotEnabled(i,!on);});
            button(g,R{box.getX()+38,box.getBottom()-34,96,26},"EDIT PEDAL",[this,i]{selectSlot(i);},false,false,11.f);
            button(g,R{box.getX()+140,box.getBottom()-34,91,26},"LIBRARY  v",
                   [this,i]{selectedSlot=i;libraryCategory=-1;setScreen(Screen::Library);},false,false,11.f);
            button(g,R{box.getX()+239,box.getBottom()-34,31,26} ,"<",
                   [this,i]{if(i>0)processor.moveSlot(i,i-1);},false,false,19.f);
            button(g,R{box.getX()+275,box.getBottom()-34,31,26},juce::String::fromUTF8("›"),
                   [this,i]{if(i<7)processor.moveSlot(i,i+1);},false,false,19.f);
        } else {
            drawLabel(g,"EMPTY",R{box.getRight()-64,box.getY()+8,53,25},11,muted,false,juce::Justification::centred);
            const R empty{box.getX()+52,box.getY()+44,box.getWidth()-104,195};
            g.setColour(goldDark.withAlpha(.65f));g.drawRoundedRectangle(empty,12.f,1.3f);
            g.setColour(goldLight);g.drawEllipse(empty.getCentreX()-25,empty.getY()+36,50,50,1.5f);
            drawLabel(g,"+",R{empty.getCentreX()-22,empty.getY()+43,44,39},38,goldLight,false,
                      juce::Justification::centred);
            drawLabel(g,"MAKE SOME HISTORY",R{empty.getX(),empty.getY()+105,empty.getWidth(),25},14,goldLight,false,
                      juce::Justification::centred,true);
            drawLabel(g,"Choose a Liberty pedal",R{empty.getX(),empty.getY()+137,empty.getWidth(),22},13,cream,false,
                      juce::Justification::centred);
            hit(empty,[this,i]{selectedSlot=i;libraryCategory=-1;setScreen(Screen::Library);});
            button(g,R{box.getX()+50,box.getBottom()-34,112,26},"ADD PEDAL",
                   [this,i]{selectedSlot=i;libraryCategory=-1;setScreen(Screen::Library);},false,false,11.f);
            button(g,R{box.getX()+171,box.getBottom()-34,99,26},"LIBRARY  v",
                   [this,i]{selectedSlot=i;libraryCategory=-1;setScreen(Screen::Library);},false,false,11.f);
        }
    }
    // The bottom bar uses the same visual language as the reference image.
    surface(g,R{23,790,1553,126},sapphire,midnight,8.f,goldDark);
    drawLabel(g,"MASTER",R{33,826,90,18},14,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,"SIGNAL",R{33,850,90,18},14,goldLight,false,juce::Justification::centredLeft,true);
    line(g,110,806,110,894,goldDark,1.5f);
    drawKnob(g,174,815,23,(settings.inputDb+24.f)/48.f,"INPUT",juce::String(settings.inputDb,1)+" dB",
             [this](float v){processor.setGlobal(0,v*48.f-24.f);},true);
    drawKnob(g,287,815,23,settings.globalMix,"GLOBAL MIX",juce::String(int(settings.globalMix*100.f))+" %",
             [this](float v){processor.setGlobal(2,v);},true);
    drawKnob(g,401,815,23,(settings.outputDb+24.f)/48.f,"OUTPUT",juce::String(settings.outputDb,1)+" dB",
             [this](float v){processor.setGlobal(1,v*48.f-24.f);},true);
    line(g,475,807,475,896,goldDark,1.5f);
    drawLabel(g,"INPUT PEAK",R{505,813,165,23},13,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,"OUTPUT PEAK",R{754,813,167,23},13,goldLight,false,juce::Justification::centredLeft,true);
    drawMeter(g,R{554,844,175,19},processor.inputPeak()," ");
    drawMeter(g,R{806,844,175,19},processor.outputPeak()," ");
    drawLabel(g,juce::String::fromUTF8("−60      −24      −12       0       +6"),R{552,868,182,18},10,muted);
    drawLabel(g,juce::String::fromUTF8("−60      −24      −12       0       +6"),R{803,868,182,18},10,muted);
    line(g,1018,807,1018,896,goldDark,1.5f);
    drawLabel(g,"INPUT NOISE MANAGEMENT",R{1036,809,253,19},11,muted,false,
              juce::Justification::centredLeft,true);
    const bool gate=processor.gateEnabled();
    button(g,R{1045,835,174,37},gate?"NOISE GATE ON":"NOISE GATE OFF",
           [this,gate]{processor.setGateEnabled(!gate);},true,gate,12.f);
    drawLabel(g,juce::String::fromUTF8("Stereo linked · no added hiss"),R{1041,880,200,18},10,cream);
    const float threshold=processor.gateThreshold();
    drawKnob(g,1280,815,22,(threshold+90.f)/75.f,"THRESHOLD",juce::String(threshold,0)+" dB",
             [this](float v){processor.setGateThreshold(v*75.f-90.f);},true);
    line(g,1370,806,1370,896,goldDark,1.5f);
    drawLabel(g,juce::String::fromUTF8("1973—2026"),R{1390,812,170,40},26,goldLight,true,juce::Justification::centred);
    drawLabel(g,juce::String::fromUTF8("VINTAGE SOUL  ·  MODERN POWER"),R{1390,853,172,19},9,gold,false,juce::Justification::centred,true);
    drawLabel(g,"Liberty",R{1407,873,142,27},19,goldLight,true,juce::Justification::centred);
    drawLabel(g,juce::String::fromUTF8("LIBERTY PEDAL LEGENDS  ·  40 ORIGINAL MODELS  ·  EIGHT SERIAL AUDIO STAGES"),
              R{33,913,800,17},10,muted,false,juce::Justification::centredLeft);
    drawLabel(g,juce::String::fromUTF8("LIBERTY MUSIC SOFTWARE   © 2026"),R{1210,913,347,17},10,gold,false,
              juce::Justification::centredRight);
}
void PedalLegendsAudioProcessorEditor::drawOverlay(juce::Graphics& g,R bounds,const juce::String& heading,bool drawClose) {
    // Modal input is exclusive: none of the board's hitboxes remain active.
    hotspots.clear();
    g.setColour(midnight.withAlpha(.81f));g.fillRect(R{0,0,canvasWidth,canvasHeight});
    surface(g,bounds,paper,cream,15.f,gold);
    g.setColour(ink.withAlpha(.05f));
    for(float y=bounds.getY()+6.f;y<bounds.getBottom();y+=5.f)
        line(g,bounds.getX()+3.f,y,bounds.getRight()-3.f,y,ink.withAlpha(.012f));
    drawLabel(g,heading,R{bounds.getX()+36.f,bounds.getY()+10.f,bounds.getWidth()-125.f,23.f},12,
              goldDark,false,juce::Justification::centredLeft,true);
    if(drawClose)button(g,R{bounds.getRight()-71,bounds.getY()+30,41,41},"X",
           [this]{setScreen(Screen::Board);},false,false,23.f);
}
void PedalLegendsAudioProcessorEditor::drawEditor(juce::Graphics& g) {
    const R panel{151,91,1298,727};
    drawOverlay(g,panel,"FOCUSED EDITOR   /   SIGNAL STAGE "+juce::String(selectedSlot+1).paddedLeft('0',2));
    auto stage=processor.snapshot().slots[size_t(selectedSlot)];
    const bool has=stage.model!=pedal::Model::Empty;
    const auto& meta=pedal::info(stage.model);
    drawLabel(g,has?juce::String(std::string(meta.name)):"EMPLACEMENT LIBRE",
              R{192,126,522,57},36,ink,true,juce::Justification::centredLeft);
    if(has) {
        button(g,R{996,125,181,41},"HISTORY & SOUND",[this]{openHistory(selectedSlot);},true,false,13.f);
        drawLabel(g,familyText(meta.family),R{1190,123,180,26},13,ink,false,juce::Justification::centredRight,true);
        drawLabel(g,yearForModel(stage.model),R{1250,148,115,43},31,goldDark,true,juce::Justification::centredRight);
    }
    line(g,155.f,194.f,1445.f,194.f,goldDark.withAlpha(.38f),1.f);
    line(g,627.f,217.f,627.f,712.f,goldDark.withAlpha(.35f),1.f);
    if(has) {
        drawPedal(g,R{189,236,392,319},stage.model,stage,true);
        drawLabel(g,soundDescription(stage.model),R{188,580,392,63},16,ink,true,
                  juce::Justification::centredTop);
        drawLabel(g,juce::String::fromUTF8("Interprétation sonore originale · aucune revendication de copie exacte de circuit."),
                  R{190,650,392,30},11,goldDark,true,juce::Justification::centred);
    } else {
        drawLabel(g,"+",R{286,300,170,170},92,goldDark,false,juce::Justification::centred);
        drawLabel(g,juce::String::fromUTF8("Choisissez une pédale de la collection Liberty"),R{203,515,370,36},17,ink,true,
                  juce::Justification::centred);
    }
    if(has) {
        // The DSP defines four per-model controls, not six invented controls.
        // The other two actual knobs are the independent stage mix and stage gain.
        const float xx[]={775.f,1025.f,1275.f,775.f};
        const float yy[]={271.f,271.f,271.f,446.f};
        for(int c=0;c<pedal::kControls;++c) {
            const float v=stage.controls[size_t(c)];
            const auto nm=juce::String(std::string(meta.controls[size_t(c)]));
            drawKnob(g,xx[c],yy[c],31.f,v,nm,juce::String(int(std::round(v*100)))+" %",
                     [this,c](float value){processor.setControl(selectedSlot,c,value);});
        }
        drawKnob(g,1025.f,446.f,31.f,stage.mix,"PEDAL MIX",
                 juce::String(int(std::round(stage.mix*100.f)))+" %",
                 [this](float v){processor.setSlotMix(selectedSlot,v);});
        drawKnob(g,1275.f,446.f,31.f,(stage.levelDb+18.f)/36.f,"PEDAL LEVEL",
                 juce::String(stage.levelDb,1)+" dB",
                 [this](float v){processor.setSlotLevel(selectedSlot,v*36.f-18.f);});
        line(g,672.f,560.f,1406.f,560.f,goldDark.withAlpha(.28f));
        button(g,R{683,598,155,42},stage.enabled?"PEDAL ON":"PEDAL OFF",
               [this,on=stage.enabled]{processor.changeSlotEnabled(selectedSlot,!on);},false,stage.enabled,14.f);
        drawLabel(g,juce::String::fromUTF8("Quatre paramètres propres au modèle, mix et niveau indépendants."),
                  R{872,601,326,35},13,ink,true,juce::Justification::centred);
    }
    button(g,R{1213,595,184,48},has?"REPLACE PEDAL":"ADD PEDAL",
           [this]{libraryCategory=-1;setScreen(Screen::Library);},true,false,14.f);
    line(g,165.f,731.f,1436.f,731.f,goldDark.withAlpha(.44f));
    button(g,R{187,754,145,38},"RESET MODEL",[this]{processor.resetSlot(selectedSlot);},true,false,12.f);
    button(g,R{341,754,93,38},"COPY",[this]{clipboardSlot=processor.snapshot().slots[size_t(selectedSlot)];hasClipboard=true;},
           true,false,12.f);
    if(hasClipboard)button(g,R{442,754,96,38},"PASTE",[this]{
        auto s=processor.snapshot();s.slots[size_t(selectedSlot)]=clipboardSlot;processor.restoreSettings(s);
    },true,false,12.f);
    else {
        surface(g,R{442,754,96,38},paper.darker(.04f),paper.darker(.1f),5.f,goldDark.withAlpha(.2f));
        drawLabel(g,"PASTE",R{442,754,96,38},12,muted,false,juce::Justification::centred);
    }
    drawLabel(g,juce::String::fromUTF8("Le déplacement échange les deux étages DSP, pas seulement leurs illustrations."),
              R{553,751,450,45},12,ink,true,juce::Justification::centred);
    if(selectedSlot>0)button(g,R{1060,754,165,38},"< MOVE LEFT",
        [this]{processor.moveSlot(selectedSlot,selectedSlot-1);selectSlot(selectedSlot-1);},true,false,12.f);
    if(selectedSlot<7)button(g,R{1231,754,170,38},"MOVE RIGHT >",
        [this]{processor.moveSlot(selectedSlot,selectedSlot+1);selectSlot(selectedSlot+1);},true,false,12.f);
}
void PedalLegendsAudioProcessorEditor::drawHistory(juce::Graphics& g) {
    const R panel{103,64,1394,815};
    drawOverlay(g,panel,juce::String::fromUTF8("LIBERTY MUSIC SOFTWARE · ENCYCLOPÉDIE"),false);
    surface(g,R{109,71,1382,110},navySoft,midnight,11.f,gold);
    g.setColour(goldLight);
    g.drawRoundedRectangle(R{157,109,36,30},4.f,1.7f);
    line(g,175,110,175,137,goldLight,1.5f);
    line(g,161,116,171,116,goldLight,1.f);
    line(g,180,116,190,116,goldLight,1.f);
    drawLabel(g,juce::String::fromUTF8("LIBERTY MUSIC SOFTWARE · ENCYCLOPÉDIE"),R{212,80,470,20},12,goldLight,false,
              juce::Justification::centredLeft,true);
    drawLabel(g,"PEDAL HISTORY",R{213,103,510,51},35,goldLight,true);
    drawLabel(g,"Discover the legends behind the sound",R{213,154,422,19},15,gold,true);
    drawLabel(g,juce::String::fromUTF8("● LECTURE SEULE"),R{1142,88,153,23},13,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,juce::String::fromUTF8("Aucun réglage n'est appliqué"),R{1104,111,212,21},11,cream,true,
              juce::Justification::centredLeft);
    button(g,R{1292,98,149,42},juce::String::fromUTF8("RETOUR À L'ÉDITEUR"),[this]{setScreen(Screen::Editor);},true,false,11.f);
    // Make sure close remains visible on top of the modal header.
    button(g,R{1452,81,36,36},"X",[this]{setScreen(Screen::Board);},false,false,21.f);
    // Side navigation is always in the same place and does not load a pedal.
    surface(g,R{108,180,393,649},navySoft,midnight,4.f,goldDark);
    const auto model=pedal::Model(std::clamp(historyModel,1,pedal::kModelCount-1));
    const auto& md=pedal::info(model);
    drawLabel(g,localized("entry_label")+juce::String(historyModel).paddedLeft('0',2)+"   /   40",
              R{136,195,290,20},12,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,juce::String(std::string(md.name)),R{136,225,335,48},32,cream,true);
    drawLabel(g,familyText(md.family),R{137,275,329,22},13,goldLight,false,juce::Justification::centredLeft,true);
    drawLabel(g,localized("period_intro")+juce::String(std::string(md.era)),R{137,301,317,49},15,cream,true);
    pedal::SlotSettings sample;sample.model=model;sample.enabled=true;sample.controls=md.defaults;
    drawPedal(g,R{138,359,332,239},model,sample);
    drawLabel(g,juce::String::fromUTF8("Illustration Liberty · lecture seule"),R{141,608,327,17},11,cream,true,
              juce::Justification::centred);
    line(g,138,639,472,639,goldDark.withAlpha(.65f));
    drawLabel(g,"EXPLORER LES 40 FICHES",R{138,652,290,22},12,goldLight,false,
              juce::Justification::centredLeft,true);
    button(g,R{137,683,39,40} ,"<",[this]{historyModel=historyModel<=1?40:historyModel-1;},false,false,21.f);
    button(g,R{182,683,235,40},juce::String(std::string(md.name)),
           [this]{historyModel=historyModel==40?1:historyModel+1;},true,false,14.f);
    button(g,R{423,683,40,40},juce::String::fromUTF8("›"),[this]{historyModel=historyModel>=40?1:historyModel+1;},false,false,21.f);
    drawLabel(g,juce::String::fromUTF8("La consultation ne charge aucune pédale."),R{137,726,328,17},11,cream,true);
    line(g,138,754,472,754,goldDark.withAlpha(.65f));
    drawLabel(g,juce::String::fromUTF8("DANS VOTRE PÉDALIER"),R{137,765,305,21},11,goldLight,false,
              juce::Justification::centredLeft,true);
    const auto cfg=processor.snapshot();
    for(int slot=0;slot<8;++slot) {
        const float x=137.f+slot*39.5f;
        const bool active=slot==selectedSlot;
        const int stageModel=int(cfg.slots[size_t(slot)].model);
        if(stageModel==0) {
            surface(g,R{x,794,34,29},midnight,navy,4.f,navySoft);
            drawLabel(g,juce::String(slot+1).paddedLeft('0',2),R{x,794,34,29},10,muted,false,
                      juce::Justification::centred);
        } else {
            button(g,R{x,794,34,29},juce::String(slot+1).paddedLeft('0',2),
                   [this,slot,stageModel]{selectedSlot=slot;historyModel=stageModel;historyTab=0;},
                   active,false,10.f);
        }
    }
    // Reference-style parchment reading page.
    surface(g,R{507,180,984,649},cream,paper,4.f,goldDark.withAlpha(.6f));
    for(int tab=0;tab<3;++tab){
        constexpr const char* names[]={"L'HISTOIRE","LE SON","LES CONSEILS"};
        button(g,R{530+tab*128.f,190,122,38},names[tab],[this,tab]{historyTab=tab;},
               historyTab==tab,historyTab==tab,12.f);
    }
    drawLabel(g,juce::String::fromUTF8("PÉDALE DE L'EMPLACEMENT SÉLECTIONNÉ"),R{1115,189,341,39},11,goldDark,false,
              juce::Justification::centredRight,true);
    line(g,507,234,1490,234,goldDark.withAlpha(.34f));
    const auto labelSection=[this,&g](char letter,const juce::String& title,float y){
        surface(g,R{533,y,34,32},paper,gold,5.f,goldDark);
        drawLabel(g,juce::String(std::string(1,letter)),R{533,y,34,32},20,goldDark,true,
                  juce::Justification::centred,true);
        drawLabel(g,title,R{582,y,664,32},15,ink,false,juce::Justification::centredLeft,true);
    };
    const auto title=juce::String(std::string(md.name));
    const auto family=juce::String(std::string(md.family));
    if(historyTab==0) {
        labelSection('A',"L'HISTOIRE",256.f);
        drawLabel(g,"Les effets de la famille "+family+juce::String::fromUTF8(" ont façonné le son de la guitare électrique. De l'époque ")+
                  juce::String(std::string(md.era))+juce::String::fromUTF8(" à nos jours, les musiciens ont exploré leurs circuits, leur réponse et leurs couleurs. ") +
                  title+juce::String::fromUTF8(" réinterprète cet héritage avec un traitement DSP original, pensé pour le jeu et la création musicale."),
                  R{535,294,925,109},18,ink,true,juce::Justification::topLeft);
        labelSection('B',"INSPIRATIONS HISTORIQUES",418.f);
        drawLabel(g,juce::String::fromUTF8("Repères sonores et culturels, jamais une revendication de réplique officielle."),
                  R{535,450,918,28},13,goldDark,true);
        surface(g,R{535,483,925,82},paper,goldLight,7.f,goldDark);
        drawLabel(g,historicalInfluence(model),R{556,495,880,29},17,ink,true,
                  juce::Justification::centredLeft,true);
        drawLabel(g,juce::String::fromUTF8("Influence de famille · les marques et matériels cités sont leurs propres références."),
                  R{558,530,871,25},13,ink,true);
        labelSection('C',juce::String::fromUTF8("CARACTÈRE SONORE"),589.f);
        drawLabel(g,soundDescription(model)+juce::String::fromUTF8(" Les paramètres répondent au niveau d'entrée, au toucher et à l'ordre des effets. ")
                  juce::String::fromUTF8("L'architecture garde une signature propre à chaque modèle."),R{535,631,920,72},17,ink,true,
                  juce::Justification::topLeft);
        labelSection('D',"CONSEILS DE SOUND DESIGN",719.f);
        drawLabel(g,juce::String::fromUTF8("Commencez par un mix modéré puis ajustez le gain et le caractère à votre guitare. ")
                  juce::String::fromUTF8("Toutes les valeurs sont indicatives et rien n'est appliqué automatiquement."),
                  R{535,755,921,51},15,ink,true,juce::Justification::topLeft);
    } else if(historyTab==1) {
        labelSection('A',"LA SIGNATURE SONORE",266.f);
        drawLabel(g,soundDescription(model),R{534,318,911,78},21,ink,true,juce::Justification::topLeft);
        labelSection('B',"LE COMPORTEMENT AUDIO",425.f);
        drawLabel(g,juce::String::fromUTF8("Ce modèle traite réellement l'audio dans son slot, avec des réglages indépendants, ")
                  juce::String::fromUTF8("la compensation de niveau et un bypass progressif. Les effets s'enchaînent dans l'ordre 1 à 8."),
                  R{535,469,908,89},19,ink,true,juce::Justification::topLeft);
        labelSection('C',juce::String::fromUTF8("RÉGLAGES DU MODÈLE"),586.f);
        juce::String controls;
        for(int c=0;c<pedal::kControls;++c) {
            if(c)controls+=juce::String::fromUTF8("    ·    ");
            controls+=juce::String(std::string(md.controls[size_t(c)]));
        }
        drawLabel(g,controls,R{535,635,916,43},17,ink,true);
        drawLabel(g,juce::String::fromUTF8("Les réglages affichés dans l'éditeur agissent sur les paramètres réels du DSP."),
                  R{535,688,913,60},16,ink,true);
    } else {
        labelSection('A',juce::String::fromUTF8("PREMIER RÉGLAGE"),270.f);
        drawLabel(g,"Chargez "+title+" seul sur un emplacement, placez les autres effets en bypass "
                  juce::String::fromUTF8("et ajustez les contrôles progressivement. Comparez les sons à niveau de sortie équivalent."),
                  R{535,319,916,89},19,ink,true,juce::Justification::topLeft);
        labelSection('B',juce::String::fromUTF8("CHAÎNE DE TRAITEMENT"),447.f);
        drawLabel(g,juce::String::fromUTF8("Testez différentes positions avant et après les pédales de saturation, puis ajoutez ")
                  juce::String::fromUTF8("une modulation, un délai ou une réverbération selon votre intention musicale."),
                  R{535,496,916,89},19,ink,true,juce::Justification::topLeft);
        labelSection('C',juce::String::fromUTF8("EXPÉRIENCES MUSICALES"),621.f);
        drawLabel(g,juce::String::fromUTF8("Expérimentez avec un son clair, un riff dynamique et des accords longs. ")
                  juce::String::fromUTF8("Ajustez le mix par slot pour retrouver de la définition sans perdre la couleur de l'effet."),
                  R{535,666,918,93},19,ink,true,juce::Justification::topLeft);
    }
    drawLabel(g,juce::String::fromUTF8("40 fiches françaises  ·  références pédagogiques  ·  illustrations originales Liberty"),
              R{126,840,880,22},11,goldDark,true);
    drawLabel(g,"VINTAGE SOUL. MODERN POWER.",R{1140,840,321,22},12,goldDark,false,
              juce::Justification::centredRight,true);
}
void PedalLegendsAudioProcessorEditor::drawPresets(juce::Graphics& g) {
    const R panel{321,120,958,685};
    drawOverlay(g,panel,juce::String::fromUTF8("BIBLIOTHÈQUE DE PRESETS · 100 PRESETS D'USINE"));
    drawLabel(g,juce::String::fromUTF8("LE SON POUR CHAQUE ÉPOQUE"),R{383,141,742,42},28,ink,true);
    drawLabel(g,"Rechercher parmi 10 familles musicales",R{385,225,700,24},14,goldDark,true);
    for(int i=0;i<11;++i) {
        const int row=i/6,col=i%6;
        const auto rect=R{377+float(col)*145,263+float(row)*40,139,34};
        const auto caption=i==0?juce::String("TOUTES"):
            juce::String(std::string(pedal::presetCategories()[size_t(i-1)]));
        button(g,rect,caption,[this,i]{browserCategory=i-1;browserOffset=0;},
               browserCategory==i-1,browserCategory==i-1,10.f);
    }
    std::vector<int> matches;
    const auto term=search.getText().trim();
    for(int i=0;i<pedal::kFactoryPresetCount;++i){
        const auto& preset=pedal::factoryPresets()[size_t(i)];
        if(browserCategory>=0&&preset.category!=std::string(pedal::presetCategories()[size_t(browserCategory)]))continue;
        if(!term.isEmpty()&&!juce::String(preset.name).containsIgnoreCase(term))continue;
        matches.push_back(i);
    }
    browserOffset=std::clamp(browserOffset,0,std::max(0,int(matches.size())-10));
    drawLabel(g,juce::String::fromUTF8("FACTORY  ·  ")+juce::String(matches.size())+juce::String::fromUTF8(" résultats"),R{386,356,535,30},16,ink,true);
    for(int i=0;i<10&&browserOffset+i<int(matches.size());++i){
        const int actual=matches[size_t(browserOffset+i)];
        const auto& preset=pedal::factoryPresets()[size_t(actual)];
        const R r{385,391+float(i)*30.f,812,28};
        if(actual==processor.selectedFactoryPreset()){
            g.setColour(goldLight.withAlpha(.46f));g.fillRoundedRectangle(r,4.f);
        }
        button(g,r,juce::String(actual+1).paddedLeft('0',3)+"   "+juce::String(preset.name)+
               juce::String::fromUTF8("      ·      ")+juce::String(preset.category),
               [this,actual]{processor.loadFactoryPreset(actual);lastSavedPreset.clear();setScreen(Screen::Board);},
               actual==processor.selectedFactoryPreset(),false,12.8f);
    }
    drawLabel(g,"PAGE  "+juce::String(browserOffset+1)+juce::String::fromUTF8("–")+
              juce::String(std::min(int(matches.size()),browserOffset+10))+" / "+juce::String(matches.size()),
              R{390,706,462,26},14,ink,true);
    button(g,R{940,705,102,35},juce::String::fromUTF8("PRÉCÉDENT"),[this]{browserOffset=std::max(0,browserOffset-10);},true,false,11.f);
    button(g,R{1048,705,113,35},"SUIVANT",[this,maxItems=int(matches.size())]{browserOffset=std::min(std::max(0,maxItems-10),browserOffset+10);},
           true,false,11.f);
    button(g,R{768,756,427,35},"OUVRIR UN PRESET UTILISATEUR  (.plegends)",
           [this]{openUserPreset();},true,false,12.f);
}
void PedalLegendsAudioProcessorEditor::drawLibrary(juce::Graphics& g) {
    const R panel{251,117,1098,709};
    drawOverlay(g,panel,juce::String::fromUTF8("LA COLLECTION LIBERTY  ·  40 MODÈLES DSP"));
    drawLabel(g,juce::String::fromUTF8("CHOISIR LA PÉDALE DE L'EMPLACEMENT ")+juce::String(selectedSlot+1),
              R{383,139,885,44},27,ink,true);
    drawLabel(g,juce::String::fromUTF8("Rechercher un modèle, une famille ou une époque"),R{385,230,700,24},14,goldDark,true);
    static constexpr const char* families[]={"Overdrive","Distortion","Fuzz","Boost / Preamp","Compressor",
                  "EQ / Filter","Wah / Envelope","Modulation","Tremolo","Delay / Echo","Reverb",
                  "Pitch / Octave","Experimental"};
    button(g,R{277,273,153,31},"TOUTES",[this]{libraryCategory=-1;libraryOffset=0;},
           libraryCategory<0,libraryCategory<0,10.5f);
    for(int i=0;i<13;++i){
        const int row=(i+1)/7,col=(i+1)%7;
        const auto r=R{277+col*151.f,273+row*38.f,147,32};
        button(g,r,juce::String(families[i]),[this,i]{libraryCategory=i;libraryOffset=0;},
               libraryCategory==i,libraryCategory==i,10.f);
    }
    std::vector<int> matches;
    const auto term=search.getText().trim();
    for(int i=1;i<pedal::kModelCount;++i) {
        const auto& item=pedal::info(pedal::Model(i));
        if(libraryCategory>=0&&item.family!=std::string_view(families[libraryCategory]))continue;
        const auto combined=juce::String(std::string(item.name))+" "+juce::String(std::string(item.family))+" "+juce::String(std::string(item.era));
        if(!term.isEmpty()&&!combined.containsIgnoreCase(term))continue;
        matches.push_back(i);
    }
    libraryOffset=std::clamp(libraryOffset,0,std::max(0,int(matches.size())-12));
    drawLabel(g,juce::String(matches.size())+juce::String::fromUTF8(" pédales dans cette sélection"),
              R{287,375,700,30},17,ink,true);
    for(int k=0;k<12&&libraryOffset+k<int(matches.size());++k){
        const int i=matches[size_t(libraryOffset+k)];
        const int col=k%3,row=k/3;
        const auto& item=pedal::info(pedal::Model(i));
        const auto r=R{277+float(col)*344.f,420+float(row)*79.f,326,67};
        surface(g,r,paper,cream,8.f,goldDark);
        g.setGradientFill(juce::ColourGradient(pedalPaint(pedal::Model(i)).brighter(.3f),
                               r.getTopLeft(),pedalPaint(pedal::Model(i)).darker(.3f),r.getBottomRight(),false));
        g.fillRoundedRectangle(R{r.getX()+8,r.getY()+9,58,49},7.f);
        g.setColour(gold);g.drawRoundedRectangle(R{r.getX()+8,r.getY()+9,58,49},6.f,1.5f);
        drawLabel(g,juce::String(std::string(item.name)),R{r.getX()+79,r.getY()+7,243,29},20,ink,true,
                  juce::Justification::centredLeft,true);
        drawLabel(g,familyText(item.family)+juce::String::fromUTF8("  ·  ")+juce::String(std::string(item.era)),
                  R{r.getX()+79,r.getY()+37,242,22},10,goldDark,false);
        hit(r,[this,i]{showModel(i);});
    }
    button(g,R{278,750,197,41},"EMPLACEMENT VIDE",[this]{showModel(0);},true,false,12.f);
    button(g,R{972,750,142,41},juce::String::fromUTF8("‹ PRÉCÉDENT"),[this]{libraryOffset=std::max(0,libraryOffset-12);},true,false,11.f);
    button(g,R{1119,750,176,41},juce::String::fromUTF8("SUIVANT ›"),[this,maxItems=int(matches.size())]{
        libraryOffset=std::min(std::max(0,maxItems-12),libraryOffset+12);
    },true,false,11.f);
}
void PedalLegendsAudioProcessorEditor::paint(juce::Graphics& g) {
    hotspots.clear();
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::scale(scale).translated(xOffset,yOffset));
    drawBackplate(g);
    drawBoard(g);
    switch(screen){
        case Screen::Editor:drawEditor(g);break;
        case Screen::History:drawHistory(g);break;
        case Screen::Presets:drawPresets(g);break;
        case Screen::Library:drawLibrary(g);break;
        case Screen::Board:break;
    }
}
void PedalLegendsAudioProcessorEditor::handleClick(juce::Point<float> p) {
    for(auto i=hotspots.rbegin();i!=hotspots.rend();++i) {
        if(!i->bounds.contains(p))continue;
        if(i->drag) {
            dragCallback=i->drag;
            dragStart=i->value;
            dragY=p.y;
        } else if(i->click) i->click();
        return;
    }
}
void PedalLegendsAudioProcessorEditor::mouseDown(const juce::MouseEvent& e) {
    dragCallback={};
    handleClick(logicalPoint(e.getPosition()));
    repaint();
}
void PedalLegendsAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e) {
    if(!dragCallback)return;
    const auto p=logicalPoint(e.getPosition());
    dragCallback(std::clamp(dragStart+(dragY-p.y)/170.f,0.f,1.f));
    repaint();
}
void PedalLegendsAudioProcessorEditor::mouseUp(const juce::MouseEvent&) {dragCallback={};}
void PedalLegendsAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent&,
                                                       const juce::MouseWheelDetails& wheel) {
    const int shift=wheel.deltaY>0.f?-2:2;
    if(screen==Screen::Presets)browserOffset=std::max(0,browserOffset+shift);
    else if(screen==Screen::Library)libraryOffset=std::max(0,libraryOffset+shift);
    else return;
    repaint();
}
bool PedalLegendsAudioProcessorEditor::keyPressed(const juce::KeyPress& key) {
    if(key.getKeyCode()==juce::KeyPress::escapeKey&&screen!=Screen::Board){setScreen(Screen::Board);return true;}
    return false;
}
