#pragma once
#include <JuceHeader.h>
#include <array>
#include <string>
#include <unordered_map>

// All text conversions are explicit UTF-8; no native ASCII constructor for accents.
namespace liberty_ui {
using Translations=std::array<juce::String,3>;
inline int safeLanguage(int id) noexcept {return juce::jlimit(0,2,id);}
inline juce::String decoded(const char* utf8) {return juce::String::fromUTF8(utf8);}
inline const auto& dictionary() {
    static const std::unordered_map<std::string,Translations> words=[] {
        std::unordered_map<std::string,Translations> out;
        out.reserve(190);
        auto add=[&](const char* key,const char* fr,const char* en,const char* es) {
            out.emplace(key,Translations{decoded(fr),decoded(en),decoded(es)});
        };
        add("LANGUE","LANGUE","LANGUAGE","IDIOMA");
        add("• MODIFICATIONS NON ENREGISTRÉES","MODIFICATIONS NON ENREGISTRÉES","UNSAVED CHANGES","CAMBIOS SIN GUARDAR");
        add("• FACTORY / USER","USINE / UTILISATEUR","FACTORY / USER","FÁBRICA / USUARIO");
        add("FACTORY / USER","USINE / UTILISATEUR","FACTORY / USER","FÁBRICA / USUARIO");
        add("BROWSE  100","PARCOURIR  100","BROWSE  100","EXPLORAR  100");
        add("SAVE","SAUVER","SAVE","GUARDAR");
        add("BYPASSED","DÉSACTIVÉ","BYPASSED","DESACTIVADO");
        add("ON","ACTIF","ON","ACTIVO");
        add("• THE LIBERTY COLLECTION","LA COLLECTION LIBERTY","THE LIBERTY COLLECTION","COLECCIÓN LIBERTY");
        add("40 ORIGINAL PEDALS","40 PÉDALES ORIGINALES","40 ORIGINAL PEDALS","40 PEDALES ORIGINALES");
        add("HISTORY & SOUND","HISTOIRE & SON","HISTORY & SOUND","HISTORIA Y SONIDO");
        add("EDIT PEDAL","ÉDITER","EDIT PEDAL","EDITAR");
        add("LIBRARY  v","BIBLIOTHÈQUE v","LIBRARY v","BIBLIOTECA v");
        add("ADD PEDAL","AJOUTER","ADD PEDAL","AÑADIR");
        add("EMPTY","VIDE","EMPTY","VACÍO");
        add("OPEN STAGE","EMPLACEMENT LIBRE","OPEN STAGE","ESPACIO LIBRE");
        add("MAKE SOME HISTORY","CRÉEZ VOTRE SON","MAKE SOME HISTORY","CREA TU SONIDO");
        add("Choose a Liberty pedal","Choisissez une pédale Liberty","Choose a Liberty pedal","Elige un pedal Liberty");
        add("MASTER","SECTION","MASTER","SECCIÓN");
        add("SIGNAL","MASTER","SIGNAL","MAESTRA");
        add("INPUT","ENTRÉE","INPUT","ENTRADA");
        add("OUTPUT","SORTIE","OUTPUT","SALIDA");
        add("GLOBAL MIX","MIX GLOBAL","GLOBAL MIX","MEZCLA GLOBAL");
        add("INPUT PEAK","CRÊTE ENTRÉE","INPUT PEAK","PICO ENTRADA");
        add("OUTPUT PEAK","CRÊTE SORTIE","OUTPUT PEAK","PICO SALIDA");
        add("INPUT NOISE MANAGEMENT","GESTION DU BRUIT","INPUT NOISE MANAGEMENT","CONTROL DE RUIDO");
        add("NOISE GATE ON","GATE ACTIF","NOISE GATE ON","PUERTA ACTIVA");
        add("NOISE GATE OFF","GATE INACTIF","NOISE GATE OFF","PUERTA INACTIVA");
        add("Stereo linked · no added hiss","Stéréo liée - sans souffle ajouté","Stereo linked - no added hiss","Estéreo enlazado - sin ruido añadido");
        add("THRESHOLD","SEUIL","THRESHOLD","UMBRAL");
        add("PEDAL MIX","MIX PÉDALE","PEDAL MIX","MEZCLA PEDAL");
        add("PEDAL LEVEL","NIVEAU PÉDALE","PEDAL LEVEL","NIVEL PEDAL");
        add("VINTAGE SOUL  ·  MODERN POWER","ÂME VINTAGE - PUISSANCE MODERNE","VINTAGE SOUL - MODERN POWER","ALMA VINTAGE - POTENCIA MODERNA");
        add("LIBERTY PEDAL LEGENDS  ·  40 ORIGINAL MODELS  ·  EIGHT SERIAL AUDIO STAGES","LIBERTY PEDAL LEGENDS - 40 MODÈLES - 8 ÉTAGES AUDIO EN SÉRIE","LIBERTY PEDAL LEGENDS - 40 MODELS - 8 SERIAL AUDIO STAGES","LIBERTY PEDAL LEGENDS - 40 MODELOS - 8 ETAPAS DE AUDIO EN SERIE");
        add("LIBERTY MUSIC SOFTWARE   © 2026","LIBERTY MUSIC SOFTWARE (c) 2026","LIBERTY MUSIC SOFTWARE (c) 2026","LIBERTY MUSIC SOFTWARE (c) 2026");
        add("COMPRESSOR & LIMITER","COMPRESSEUR / LIMITEUR","COMPRESSOR & LIMITER","COMPRESOR / LIMITADOR");
        add("EQ & FILTER","ÉGALISEUR / FILTRE","EQ & FILTER","ECUALIZADOR / FILTRO");
        add("DELAY & ECHO","DÉLAI / ÉCHO","DELAY & ECHO","DELAY / ECO");
        add("DISTORTION","DISTORSION","DISTORTION","DISTORSIÓN");
        add("BOOST / PREAMP","BOOST / PRÉAMPLI","BOOST / PREAMP","BOOST / PREAMPLI");
        add("WAH / ENVELOPE","WAH / ENVELOPPE","WAH / ENVELOPE","WAH / ENVOLVENTE");
        add("MODULATION","MODULATION","MODULATION","MODULACIÓN");
        add("TREMOLO","TRÉMOLO","TREMOLO","TRÉMOLO");
        add("REVERB","RÉVERBÉRATION","REVERB","REVERBERACIÓN");
        add("PITCH / OCTAVE","HAUTEUR / OCTAVE","PITCH / OCTAVE","TONO / OCTAVA");
        add("EXPERIMENTAL","EXPÉRIMENTAL","EXPERIMENTAL","EXPERIMENTAL");
        add("FOCUSED EDITOR","ÉDITEUR DE PÉDALE","FOCUSED EDITOR","EDITOR DE PEDAL");
        add("EMPLACEMENT LIBRE","EMPLACEMENT LIBRE","EMPTY STAGE","ESPACIO LIBRE");
        add("Interprétation sonore originale · aucune revendication de copie exacte de circuit.","Interprétation sonore originale. Aucune revendication de copie exacte de circuit.","Original sonic interpretation. No exact circuit-copy claim.","Interpretación sonora original. No es una copia exacta de ningún circuito.");
        add("Choisissez une pédale de la collection Liberty","Choisissez une pédale de la collection Liberty","Choose a pedal from the Liberty collection","Elige un pedal de la colección Liberty");
        add("PEDAL ON","PÉDALE ACTIVE","PEDAL ON","PEDAL ACTIVO");
        add("PEDAL OFF","PÉDALE INACTIVE","PEDAL OFF","PEDAL INACTIVO");
        add("Quatre paramètres propres au modèle, mix et niveau indépendants.","Quatre paramètres propres au modèle, mix et niveau indépendants.","Four model controls with independent mix and level.","Cuatro parámetros con mezcla y nivel independientes.");
        add("REPLACE PEDAL","REMPLACER","REPLACE PEDAL","SUSTITUIR");
        add("RESET MODEL","RÉINITIALISER","RESET MODEL","RESTABLECER");
        add("COPY","COPIER","COPY","COPIAR");
        add("PASTE","COLLER","PASTE","PEGAR");
        add("Le déplacement échange les deux étages DSP, pas seulement leurs illustrations.","Le déplacement échange les deux étages DSP, pas seulement leurs illustrations.","Moving swaps DSP stages, not just their graphics.","Mover intercambia etapas DSP, no solo sus imágenes.");
        add("< MOVE LEFT","< GAUCHE","< MOVE LEFT","< IZQUIERDA");
        add("MOVE RIGHT >","DROITE >","MOVE RIGHT >","DERECHA >");
        add("LIBERTY MUSIC SOFTWARE · ENCYCLOPÉDIE","LIBERTY MUSIC SOFTWARE - ENCYCLOPÉDIE","LIBERTY MUSIC SOFTWARE - ENCYCLOPEDIA","LIBERTY MUSIC SOFTWARE - ENCICLOPEDIA");
        add("PEDAL HISTORY","HISTOIRE DES PÉDALES","PEDAL HISTORY","HISTORIA DE PEDALES");
        add("Discover the legends behind the sound","Découvrez les légendes derrière le son","Discover the legends behind the sound","Descubre las leyendas detrás del sonido");
        add("● LECTURE SEULE","CONSULTATION","READ ONLY","SOLO LECTURA");
        add("Aucun réglage n'est appliqué","Aucun réglage n'est appliqué","No settings are applied","No se aplican ajustes");
        add("RETOUR À L'ÉDITEUR","RETOUR À L'ÉDITEUR","BACK TO EDITOR","VOLVER AL EDITOR");
        add("Illustration Liberty · lecture seule","Illustration Liberty - consultation","Liberty illustration - read only","Ilustración Liberty - solo lectura");
        add("EXPLORER LES 40 FICHES","PARCOURIR LES 40 FICHES","BROWSE 40 ENTRIES","EXPLORAR 40 FICHAS");
        add("La consultation ne charge aucune pédale.","La consultation ne charge aucune pédale.","Browsing does not load a pedal.","Consultar no carga ningún pedal.");
        add("DANS VOTRE PÉDALIER","DANS VOTRE PÉDALIER","ON YOUR PEDALBOARD","EN TU PEDALERA");
        add("L'HISTOIRE","L'HISTOIRE","HISTORY","HISTORIA");
        add("LE SON","LE SON","SOUND","SONIDO");
        add("LES CONSEILS","LES CONSEILS","TIPS","CONSEJOS");
        add("PÉDALE DE L'EMPLACEMENT SÉLECTIONNÉ","PÉDALE DE L'EMPLACEMENT SÉLECTIONNÉ","SELECTED SLOT PEDAL","PEDAL SELECCIONADO");
        add("INSPIRATIONS HISTORIQUES","INSPIRATIONS HISTORIQUES","HISTORICAL INFLUENCES","INSPIRACIONES HISTÓRICAS");
        add("Repères sonores et culturels, jamais une revendication de réplique officielle.","Repères sonores et culturels, jamais une revendication de réplique officielle.","Sonic and cultural references, not official replicas.","Referencias sonoras y culturales, no réplicas oficiales.");
        add("CARACTÈRE SONORE","CARACTÈRE SONORE","TONAL CHARACTER","CARÁCTER SONORO");
        add("CONSEILS DE SOUND DESIGN","CONSEILS DE SOUND DESIGN","SOUND DESIGN TIPS","CONSEJOS DE DISEÑO SONORO");
        add("LA SIGNATURE SONORE","LA SIGNATURE SONORE","SONIC SIGNATURE","FIRMA SONORA");
        add("LE COMPORTEMENT AUDIO","LE COMPORTEMENT AUDIO","AUDIO BEHAVIOR","COMPORTAMIENTO DE AUDIO");
        add("RÉGLAGES DU MODÈLE","RÉGLAGES DU MODÈLE","MODEL CONTROLS","CONTROLES DEL MODELO");
        add("Les réglages affichés dans l'éditeur agissent sur les paramètres réels du DSP.","Les réglages affichés dans l'éditeur agissent sur les paramètres réels du DSP.","The editor controls change real DSP parameters.","Los controles modifican los parámetros DSP reales.");
        add("PREMIER RÉGLAGE","PREMIER RÉGLAGE","STARTING SETTINGS","AJUSTES INICIALES");
        add("CHAÎNE DE TRAITEMENT","CHAÎNE DE TRAITEMENT","SIGNAL CHAIN","CADENA DE EFECTOS");
        add("EXPÉRIENCES MUSICALES","EXPÉRIENCES MUSICALES","MUSICAL EXPERIMENTS","EXPERIMENTOS MUSICALES");
        add("40 fiches françaises  ·  références pédagogiques  ·  illustrations originales Liberty","40 fiches - références pédagogiques - illustrations originales Liberty","40 entries - educational references - original Liberty artwork","40 fichas - referencias educativas - ilustraciones originales Liberty");
        add("VINTAGE SOUL. MODERN POWER.","ÂME VINTAGE. PUISSANCE MODERNE.","VINTAGE SOUL. MODERN POWER.","ALMA VINTAGE. POTENCIA MODERNA.");
        add("BIBLIOTHÈQUE DE PRESETS · 100 PRESETS D'USINE","BIBLIOTHÈQUE DE PRESETS - 100 PRESETS D'USINE","PRESET LIBRARY - 100 FACTORY PRESETS","BIBLIOTECA DE PRESETS - 100 PRESETS DE FÁBRICA");
        add("LE SON POUR CHAQUE ÉPOQUE","LE SON POUR CHAQUE ÉPOQUE","A SOUND FOR EVERY ERA","UN SONIDO PARA CADA ÉPOCA");
        add("Rechercher parmi 10 familles musicales","Rechercher parmi 10 familles musicales","Search 10 musical categories","Busca 10 estilos musicales");
        add("TOUTES","TOUTES","ALL","TODAS");
        add("FACTORY","USINE","FACTORY","FÁBRICA");
        add("résultats","résultats","results","resultados");
        add("PAGE","PAGE","PAGE","PÁGINA");
        add("PRÉCÉDENT","PRÉCÉDENT","PREVIOUS","ANTERIOR");
        add("SUIVANT","SUIVANT","NEXT","SIGUIENTE");
        add("OUVRIR UN PRESET UTILISATEUR  (.plegends)","OUVRIR UN PRESET UTILISATEUR (.plegends)","OPEN USER PRESET (.plegends)","ABRIR PRESET DE USUARIO (.plegends)");
        add("LA COLLECTION LIBERTY  ·  40 MODÈLES DSP","COLLECTION LIBERTY - 40 MODÈLES DSP","LIBERTY COLLECTION - 40 DSP MODELS","COLECCIÓN LIBERTY - 40 MODELOS DSP");
        add("Rechercher un modèle, une famille ou une époque","Rechercher un modèle, une famille ou une époque","Search model, family or era","Busca modelo, familia o época");
        add("pédales dans cette sélection","pédales dans cette sélection","pedals in this selection","pedales en esta selección");
        add("EMPLACEMENT VIDE","EMPLACEMENT VIDE","EMPTY SLOT","ESPACIO VACÍO");
        add("Rechercher...","Rechercher...","Search...","Buscar...");
        add("Enregistrer le pédalier","Enregistrer le pédalier","Save pedalboard","Guardar pedalera");
        add("Charger un pédalier","Charger un pédalier","Open pedalboard","Abrir pedalera");
        add("Vintage Blues","Blues vintage","Vintage Blues","Blues vintage");
        add("Classic Rock","Rock classique","Classic Rock","Rock clásico");
        add("Hard Rock","Hard rock","Hard Rock","Hard rock");
        add("Heavy Metal","Heavy metal","Heavy Metal","Heavy metal");
        add("Modern Metal","Metal moderne","Modern Metal","Metal moderno");
        add("Funk and Clean","Funk et sons clairs","Funk and Clean","Funk y limpios");
        add("Jazz and Warm","Jazz et sons chauds","Jazz and Warm","Jazz cálido");
        add("Ambient and Cinematic","Ambient et cinéma","Ambient and Cinematic","Ambiental y cine");
        add("Psychedelic and Experimental","Psychédélique et expérimental","Psychedelic and Experimental","Psicodélico y experimental");
        add("Studio Essentials","Essentiels studio","Studio Essentials","Esenciales de estudio");
        add("THRESH","SEUIL","THRESH","UMBRAL");
        add("RELEASE","RELÂCHEMENT","RELEASE","RELAJACIÓN");
        add("BLEND","MÉLANGE","BLEND","MEZCLA");
        add("LEVEL","NIVEAU","LEVEL","NIVEL");
        add("GAIN","GAIN","GAIN","GANANCIA");
        add("TONE","TONALITÉ","TONE","TONO");
        add("MIX","MIX","MIX","MEZCLA");
        add("ATTACK","ATTAQUE","ATTACK","ATAQUE");
        add("DECAY","DÉCROISSANCE","DECAY","CAÍDA");
        add("RATE","VITESSE","RATE","VELOCIDAD");
        add("DEPTH","PROFONDEUR","DEPTH","PROFUNDIDAD");
        add("FEEDBACK","RETOUR","FEEDBACK","RETROALIMENTACIÓN");
        add("BASS","GRAVES","BASS","GRAVES");
        add("TREBLE","AIGUS","TREBLE","AGUDOS");
        add("LOW","GRAVES","LOW","GRAVES");
        add("HIGH","AIGUS","HIGH","AGUDOS");
        add("MID","MÉDIUMS","MID","MEDIOS");
        add("DAMP","AMORTISSEMENT","DAMP","AMORTIGUACIÓN");
        add("SIZE","TAILLE","SIZE","TAMAÑO");
        add("TIME","TEMPS","TIME","TIEMPO");
        add("FILTER","FILTRE","FILTER","FILTRO");
        add("FREQ","FRÉQUENCE","FREQ","FRECUENCIA");
        add("SHAPE","FORME","SHAPE","FORMA");
        add("REPEATS","RÉPÉTITIONS","REPEATS","REPETICIONES");
        add("intro_1","Les effets de la famille ","Effects in the ","Los efectos de la familia ");
        add("intro_2"," ont façonné le son de la guitare électrique. Depuis "," family have shaped the electric guitar sound. Since "," han marcado el sonido de la guitarra eléctrica. Desde ");
        add("intro_3",", les musiciens ont exploré leurs réponses et leurs couleurs. ",", players have explored their response and character. ",", los músicos han explorado su respuesta y carácter. ");
        add("intro_4"," propose une interprétation numérique originale, pensée pour le jeu et la création musicale."," offers an original DSP interpretation designed for performance and sound creation."," ofrece una interpretación DSP original para interpretar y crear música.");
        add("tone_suffix"," Les paramètres répondent au niveau d'entrée et au toucher. Chaque modèle possède sa signature."," Controls react to input level and playing touch. Each model has its own signature."," Los controles reaccionan al nivel de entrada y la pulsación. Cada modelo tiene su carácter.");
        add("behavior","Ce modèle traite l'audio dans son slot avec des réglages indépendants et un bypass progressif. La chaîne suit l'ordre 1 à 8.","This model processes audio in its own slot with independent controls and smooth bypass. The chain follows slots 1 to 8.","El modelo procesa audio en su espacio con controles independientes y bypass suave. La cadena sigue los espacios 1 a 8.");
        add("tip_start1","Chargez ","Load ","Carga ");
        add("tip_start2"," seul sur un emplacement, désactivez les autres effets et ajustez progressivement. Comparez les sons au même niveau de sortie."," alone in one slot, bypass other effects and adjust gradually. Compare at matched output levels."," solo en un espacio, desactiva otros efectos y ajusta gradualmente. Compara a igual nivel de salida.");
        add("tip_chain","Testez différentes positions avant et après les saturations, puis ajoutez modulation, délai ou réverbération selon votre intention musicale.","Try different positions before and after drive pedals, then add modulation, delay or reverb.","Prueba posiciones antes y después de las saturaciones; luego añade modulación, delay o reverb.");
        add("tip_exp","Testez un son clair, un riff dynamique et des accords longs. Ajustez le mix par slot pour conserver la définition.","Try clean tones, dynamic riffs and sustained chords. Adjust per-slot mix for clarity.","Prueba sonidos limpios, riffs dinámicos y acordes largos. Ajusta la mezcla por espacio.");
        add("tip_sound","Commencez par un mix modéré puis ajustez le gain. Les valeurs sont indicatives; rien n'est appliqué automatiquement.","Start with a moderate mix and tailor the gain. Settings are suggestions only.","Empieza con mezcla moderada y adapta la ganancia. Los ajustes son solo orientativos.");
        add("history_disclaimer","Influence de famille, sans revendication de copie officielle.","General influence without any claim of official replication.","Influencia general sin afirmación de réplica oficial.");
        add("entry_label","FICHE ","ENTRY ","FICHA ");
        add("period_intro","Inspirations musicales : ","Musical inspirations: ","Inspiraciones musicales: ");
        add("choose_slot","CHOISIR LA PÉDALE DE L'EMPLACEMENT ","CHOOSE PEDAL FOR SLOT ","ELEGIR PEDAL DEL ESPACIO ");
        add("stage_label","EMPLACEMENT ","SLOT ","ESPACIO ");
        add("Saturation progressive, harmonies chaleureuses et attaque de guitare préservée.","Saturation progressive et attaque de guitare préservée.","Progressive saturation and preserved pick attack.","Saturación progresiva y ataque de púa conservado.");
        add("Distorsion articulée, grain dense et réponse précise aux variations de jeu.","Distorsion articulée et réponse précise au jeu.","Articulate distortion with precise playing response.","Distorsión articulada y respuesta precisa.");
        add("Texture vintage riche en harmoniques, de la douceur aux sons les plus rugueux.","Texture vintage riche en harmoniques.","Harmonic-rich vintage texture.","Textura vintage rica en armónicos.");
        add("Compression souple inspirée des grands compresseurs de pédalier et de studio.","Compression souple inspirée des compresseurs classiques.","Smooth classic pedal and studio compression.","Compresión suave de pedal y estudio.");
        add("Sculpture tonale expressive avec fréquences et résonances musicales.","Sculpture tonale expressive.","Expressive tone shaping.","Modelado tonal expresivo.");
        add("Filtrage résonant vivant, idéal pour un jeu rythmique et expressif.","Filtrage résonant expressif.","Expressive resonant filtering.","Filtrado resonante expresivo.");
        add("Mouvement enveloppant, profondeur et caractère inspirés des modulations classiques.","Mouvement et profondeur inspirés des modulations classiques.","Immersive classic modulation and depth.","Modulación clásica envolvente.");
        add("Variation rythmique et mouvement du signal, du vintage au contemporain.","Variation rythmique du signal.","Rhythmic signal variation.","Variación rítmica de señal.");
        add("Répétitions musicales aux couleurs analogiques et numériques distinctives.","Répétitions musicales analogiques et numériques.","Musical analog and digital repeats.","Repeticiones musicales analógicas y digitales.");
        add("Espace et profondeur, avec décroissance musicale et détail des transitoires.","Espace et profondeur avec décroissance musicale.","Depth and musical decay.","Espacio y profundidad con caída musical.");
        add("Effet d'octave monophonique et nuances harmoniques pour guitare et basse.","Octave monophonique pour guitare et basse.","Monophonic octave for guitar and bass.","Octava monofónica para guitarra y bajo.");
        add("Textures évolutives et délais créatifs pour l'expérimentation sonore.","Textures évolutives et délais créatifs.","Evolving textures and creative delays.","Texturas evolutivas y delays creativos.");
        add("Préamplification dynamique et couleur musicale pour votre guitare.","Préamplification dynamique.","Dynamic guitar preamplification.","Preamplificación dinámica de guitarra.");
        return out;
    }();
    return words;
}
inline juce::String translate(const juce::String& raw,int id) {
    const auto& dict=dictionary();
    const auto found=dict.find(raw.toStdString());
    return found==dict.end()?raw:found->second[size_t(safeLanguage(id))];
}
inline juce::String safeTypography(juce::String text) {
    static const std::array<std::pair<juce::String,juce::String>,14> replacements{{
        {decoded("•"),"-"},{decoded("·"),"-"},{decoded("—"),"-"},
        {decoded("–"),"-"},{decoded("−"),"-"},{decoded("→"),">"},
        {decoded("‹"),"<"},{decoded("›"),">"},{decoded("×"),"X"},
        {decoded("▾"),"v"},{decoded("●"),""},{decoded("○"),""},
        {decoded("▣"),"i"},{decoded("©"),"(c)"}
    }};
    for(const auto& pair:replacements)
        if(text.contains(pair.first))text=text.replace(pair.first,pair.second);
    return text;
}
}
