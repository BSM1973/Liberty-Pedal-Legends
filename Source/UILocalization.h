#pragma once
#include <JuceHeader.h>
#include <array>
#include <string>
#include <unordered_map>

// UI-only localisation. Product names and DSP parameters are never rewritten.
// Explicit UTF-8 decoding prevents platform/codepage-dependent mojibake.
namespace liberty_ui {
using Translations=std::array<juce::String,3>;
inline int safeLanguage(int value) noexcept {return juce::jlimit(0,2,value);}
inline juce::String decoded(const char* utf8) {return juce::String::fromUTF8(utf8);}
inline const auto& dictionary() {
    static const std::unordered_map<std::string,Translations> content=[] {
        std::unordered_map<std::string,Translations> result;
        result.reserve(250);
        auto add=[&](const char* key,const char* french,const char* english,const char* spanish) {
            result.emplace(key,Translations{decoded(french),decoded(english),decoded(spanish)});
        };
        add("BROWSE  200","PARCOURIR  200","BROWSE  200","EXPLORAR  200");
        add("PEDALBOARD","PÉDALIER","PEDALBOARD","PEDALERA");
        add("ROUTING","ROUTAGE","ROUTING","RUTEO");
        add("SIGNAL FLOW","CHEMIN DU SIGNAL","SIGNAL FLOW","FLUJO DE SEÑAL");
        add("SIGNAL ROUTING","ROUTAGE AUDIO","AUDIO ROUTING","ENRUTAMIENTO AUDIO");
        add("BACK TO PEDALBOARD","RETOUR AU PÉDALIER","BACK TO PEDALBOARD","VOLVER A LA PEDALERA");
        add("Two independent audio paths, no external plugins","Deux chemins audio indépendants, sans plugin externe","Two independent audio paths, no external plugins","Dos rutas independientes, sin plugins externos");
        add("SERIES","SÉRIE","SERIES","SERIE");
        add("PARALLEL A/B","PARALLÈLE A/B","PARALLEL A/B","PARALELO A/B");
        add("PARALLEL DESCRIPTION","Deux chaînes séparées se rejoignent après le mixage A/B.","Two independent chains are combined at the A/B mixer.","Dos cadenas independientes se unen en la mezcla A/B.");
        add("SERIAL DESCRIPTION","Le signal traverse les huit pédales dans l'ordre.","The signal passes through all eight pedals in order.","La señal atraviesa los ocho pedales en orden.");
        add("SERIES PATH","CHAÎNE SÉRIE","SERIES PATH","CADENA EN SERIE");
        add("SERIES NOTE","Le routing série conserve vos 100 presets d'origine et le son existant.","Serial mode preserves the original 100 presets and processing.","El modo serie conserva los 100 presets originales y el procesamiento.");
        add("LANE A","BRANCHE A","LANE A","RAMA A");
        add("LANE B","BRANCHE B","LANE B","RAMA B");
        add("MOVE TO A","VERS A","MOVE TO A","MOVER A A");
        add("MOVE TO B","VERS B","MOVE TO B","MOVER A B");
        add("DIRECT SIGNAL","SIGNAL DIRECT","DIRECT SIGNAL","SEÑAL DIRECTA");
        add("SPLIT","DIVISION","SPLIT","DIVISIÓN");
        add("MERGE","SOMME","MERGE","SUMA");
        add("BLEND A/B","MIXAGE A/B","A/B BLEND","MEZCLA A/B");
        add("ROUTING MODE SAVED","Routage sauvegardé avec le projet et les presets","Routing saved with project and presets","Ruteo guardado con proyecto y presets");
        add("PRESET LIBRARY 200","NAVIGATEUR DE PRESETS - 200 SONS","PRESET LIBRARY - 200 SOUNDS","BIBLIOTECA DE PRESETS - 200 SONIDOS");
        add("200 FACTORY PRESETS","200 PRESETS D'USINE","200 FACTORY PRESETS","200 PRESETS DE FÁBRICA");
        add("SEARCH PRESETS","RECHERCHER UN PRESET","SEARCH PRESETS","BUSCAR PRESETS");
        add("ALL 200","TOUS 200","ALL 200","TODOS 200");
        add("SERIES 100","SÉRIE 100","SERIES 100","SERIE 100");
        add("PARALLEL 100","PARALLÈLE 100","PARALLEL 100","PARALELO 100");
        add("RESULTS","RÉSULTATS","RESULTS","RESULTADOS");
        add("PREVIOUS","PRÉCÉDENT","PREVIOUS","ANTERIOR");
        add("NEXT","SUIVANT","NEXT","SIGUIENTE");
        add("USER PRESET","OUVRIR UN PRESET UTILISATEUR (.plegends)","OPEN USER PRESET (.plegends)","ABRIR PRESET DE USUARIO (.plegends)");
        add("LANGUE","LANGUE","LANGUAGE","IDIOMA");
        add("• MODIFICATIONS NON ENREGISTRÉES","MODIFICATIONS NON ENREGISTRÉES","UNSAVED CHANGES","CAMBIOS SIN GUARDAR");
        add("• FACTORY / USER","PRESET D’USINE / UTILISATEUR","FACTORY / USER","FÁBRICA / USUARIO");
        add("FACTORY / USER","USINE / UTILISATEUR","FACTORY / USER","FÁBRICA / USUARIO");
        add("BROWSE  100","PARCOURIR  100","BROWSE  100","EXPLORAR  100");
        add("SAVE","SAUVER","SAVE","GUARDAR");
        add("BYPASS","BYPASS","BYPASS","BYPASS");
        add("BYPASSED","DÉSACTIVÉ","BYPASSED","DESACTIVADO");
        add("ON","ACTIF","ON","ACTIVO");
        add("OFF","INACTIF","OFF","INACTIVO");
        add("• THE LIBERTY COLLECTION","LA COLLECTION LIBERTY","THE LIBERTY COLLECTION","COLECCIÓN LIBERTY");
        add("80 ORIGINAL PEDALS","80 PÉDALES ORIGINALES","80 ORIGINAL PEDALS","80 PEDALES ORIGINALES");
        add("HISTORY & SOUND","HISTOIRE & SON","HISTORY & SOUND","HISTORIA Y SONIDO");
        add("EDIT PEDAL","ÉDITER","EDIT PEDAL","EDITAR");
        add("LIBRARY","BIBLIOTHÈQUE","LIBRARY","BIBLIOTECA");
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
        add("●  NOISE GATE ON","GATE ACTIF","NOISE GATE ON","PUERTA ACTIVA");
        add("○  NOISE GATE OFF","GATE INACTIF","NOISE GATE OFF","PUERTA INACTIVA");
        add("Stereo linked · no added hiss","Stéréo liée - sans souffle ajouté","Stereo linked - no added hiss","Estéreo enlazado - sin ruido añadido");
        add("THRESHOLD","SEUIL","THRESHOLD","UMBRAL");
        add("PEDAL MIX","MIX PÉDALE","PEDAL MIX","MEZCLA PEDAL");
        add("PEDAL LEVEL","NIVEAU PÉDALE","PEDAL LEVEL","NIVEL PEDAL");
        add("VINTAGE SOUL  ·  MODERN POWER","ÂME VINTAGE - PUISSANCE MODERNE","VINTAGE SOUL - MODERN POWER","ALMA VINTAGE - POTENCIA MODERNA");
        add("LIBERTY PEDAL LEGENDS  ·  80 ORIGINAL MODELS  ·  EIGHT SERIAL AUDIO STAGES","LIBERTY PEDAL LEGENDS - 80 MODÈLES - 8 ÉTAGES AUDIO EN SÉRIE","LIBERTY PEDAL LEGENDS - 80 MODELS - 8 SERIAL AUDIO STAGES","LIBERTY PEDAL LEGENDS - 80 MODELOS - 8 ETAPAS DE AUDIO EN SERIE");
        add("LIBERTY MUSIC SOFTWARE   © 2026","LIBERTY MUSIC SOFTWARE (c) 2026","LIBERTY MUSIC SOFTWARE (c) 2026","LIBERTY MUSIC SOFTWARE (c) 2026");
        add("COMPRESSOR & LIMITER","COMPRESSEUR / LIMITEUR","COMPRESSOR & LIMITER","COMPRESOR / LIMITADOR");
        add("EQ & FILTER","ÉGALISEUR / FILTRE","EQ & FILTER","ECUALIZADOR / FILTRO");
        add("DELAY & ECHO","DÉLAI / ÉCHO","DELAY & ECHO","DELAY / ECO");
        add("OVERDRIVE","OVERDRIVE","OVERDRIVE","OVERDRIVE");
        add("DISTORTION","DISTORSION","DISTORTION","DISTORSIÓN");
        add("FUZZ","FUZZ","FUZZ","FUZZ");
        add("BOOST / PREAMP","BOOST / PRÉAMPLI","BOOST / PREAMP","BOOST / PREAMPLI");
        add("WAH / ENVELOPE","WAH / ENVELOPPE","WAH / ENVELOPE","WAH / ENVOLVENTE");
        add("MODULATION","MODULATION","MODULATION","MODULACIÓN");
        add("TREMOLO","TRÉMOLO","TREMOLO","TRÉMOLO");
        add("REVERB","RÉVERBÉRATION","REVERB","REVERBERACIÓN");
        add("PITCH / OCTAVE","HAUTEUR / OCTAVE","PITCH / OCTAVE","TONO / OCTAVA");
        add("EXPERIMENTAL","EXPÉRIMENTAL","EXPERIMENTAL","EXPERIMENTAL");
        add("NONE","AUCUN","NONE","NINGUNO");
        add("FOCUSED EDITOR","ÉDITEUR DE PÉDALE","FOCUSED EDITOR","EDITOR DE PEDAL");
        add("EMPLACEMENT LIBRE","EMPLACEMENT LIBRE","EMPTY STAGE","ESPACIO LIBRE");
        add("Interprétation sonore originale · aucune revendication de copie exacte de circuit.","Interprétation sonore originale. Aucune revendication de copie exacte de circuit.","Original sonic interpretation. No exact circuit-copy claim.","Interpretación sonora original. No es una copia exacta de ningún circuito.");
        add("Choisissez une pédale de la collection Liberty","Choisissez une pédale de la collection Liberty","Choose a pedal from the Liberty collection","Elige un pedal de la colección Liberty");
        add("●  PEDAL ON","PÉDALE ACTIVE","PEDAL ON","PEDAL ACTIVO");
        add("○  PEDAL OFF","PÉDALE INACTIVE","PEDAL OFF","PEDAL INACTIVO");
        add("Quatre paramètres propres au modèle, mix et niveau indépendants.","Quatre paramètres propres au modèle, mix et niveau indépendants.","Four model controls, with independent mix and level.","Cuatro parámetros del modelo, con mezcla y nivel independientes.");
        add("REPLACE PEDAL","REMPLACER","REPLACE PEDAL","SUSTITUIR");
        add("RESET MODEL","RÉINITIALISER","RESET MODEL","RESTABLECER");
        add("COPY","COPIER","COPY","COPIAR");
        add("PASTE","COLLER","PASTE","PEGAR");
        add("Le déplacement échange les deux étages DSP, pas seulement leurs illustrations.","Le déplacement échange les deux étages DSP, pas seulement leurs illustrations.","Moving swaps both DSP stages, not just their graphics.","Mover intercambia las etapas DSP, no solo sus imágenes.");
        add("‹ MOVE LEFT","< GAUCHE","< MOVE LEFT","< IZQUIERDA");
        add("MOVE RIGHT ›","DROITE >","MOVE RIGHT >","DERECHA >");
        add("LIBERTY MUSIC SOFTWARE · ENCYCLOPÉDIE","LIBERTY MUSIC SOFTWARE - ENCYCLOPÉDIE","LIBERTY MUSIC SOFTWARE - ENCYCLOPEDIA","LIBERTY MUSIC SOFTWARE - ENCICLOPEDIA");
        add("PEDAL HISTORY","HISTOIRE DES PÉDALES","PEDAL HISTORY","HISTORIA DE PEDALES");
        add("Discover the legends behind the sound","Découvrez les légendes derrière le son","Discover the legends behind the sound","Descubre las leyendas detrás del sonido");
        add("● LECTURE SEULE","CONSULTATION","READ-ONLY","SOLO LECTURA");
        add("Aucun réglage n'est appliqué","Aucun réglage n'est appliqué","No settings are applied","No se aplican ajustes");
        add("RETOUR À L'ÉDITEUR","RETOUR À L'ÉDITEUR","BACK TO EDITOR","VOLVER AL EDITOR");
        add("Illustration Liberty · lecture seule","Illustration Liberty - consultation","Liberty illustration - read-only","Ilustración Liberty - solo lectura");
        add("EXPLORER LES 80 FICHES","PARCOURIR LES 80 FICHES","BROWSE 80 ENTRIES","EXPLORAR 80 FICHAS");
        add("La consultation ne charge aucune pédale.","La consultation ne charge aucune pédale.","Browsing does not load a pedal.","Consultar no carga ningún pedal.");
        add("DANS VOTRE PÉDALIER","DANS VOTRE PÉDALIER","ON YOUR PEDALBOARD","EN TU PEDALERA");
        add("L'HISTOIRE","L'HISTOIRE","HISTORY","HISTORIA");
        add("LE SON","LE SON","SOUND","SONIDO");
        add("LES CONSEILS","LES CONSEILS","TIPS","CONSEJOS");
        add("PÉDALE DE L'EMPLACEMENT SÉLECTIONNÉ","PÉDALE DE L'EMPLACEMENT SÉLECTIONNÉ","SELECTED SLOT PEDAL","PEDAL DEL ESPACIO SELECCIONADO");
        add("INSPIRATIONS HISTORIQUES","INSPIRATIONS HISTORIQUES","HISTORICAL INFLUENCES","INSPIRACIONES HISTÓRICAS");
        add("Repères sonores et culturels, jamais une revendication de réplique officielle.","Repères sonores et culturels, jamais une revendication de réplique officielle.","Sonic and cultural references, not claims of official replication.","Referencias sonoras y culturales; no son réplicas oficiales.");
        add("CARACTÈRE SONORE","CARACTÈRE SONORE","TONAL CHARACTER","CARÁCTER SONORO");
        add("CONSEILS DE SOUND DESIGN","CONSEILS DE SOUND DESIGN","SOUND DESIGN TIPS","CONSEJOS DE DISEÑO SONORO");
        add("LA SIGNATURE SONORE","LA SIGNATURE SONORE","SONIC SIGNATURE","FIRMA SONORA");
        add("LE COMPORTEMENT AUDIO","LE COMPORTEMENT AUDIO","AUDIO BEHAVIOR","COMPORTAMIENTO DE AUDIO");
        add("RÉGLAGES DU MODÈLE","RÉGLAGES DU MODÈLE","MODEL CONTROLS","CONTROLES DEL MODELO");
        add("Les réglages affichés dans l'éditeur agissent sur les paramètres réels du DSP.","Les réglages affichés dans l'éditeur agissent sur les paramètres réels du DSP.","The editor controls change real DSP parameters.","Los controles del editor modifican parámetros DSP reales.");
        add("PREMIER RÉGLAGE","PREMIER RÉGLAGE","STARTING SETTINGS","AJUSTES INICIALES");
        add("CHAÎNE DE TRAITEMENT","CHAÎNE DE TRAITEMENT","SIGNAL CHAIN","CADENA DE EFECTOS");
        add("EXPÉRIENCES MUSICALES","EXPÉRIENCES MUSICALES","MUSICAL EXPERIMENTS","EXPERIMENTOS MUSICALES");
        add("80 fiches françaises  ·  références pédagogiques  ·  illustrations originales Liberty","80 fiches - références pédagogiques - illustrations originales Liberty","80 entries - educational references - original Liberty artwork","80 fichas - referencias educativas - ilustraciones Liberty originales");
        add("VINTAGE SOUL. MODERN POWER.","ÂME VINTAGE. PUISSANCE MODERNE.","VINTAGE SOUL. MODERN POWER.","ALMA VINTAGE. POTENCIA MODERNA.");
        add("BIBLIOTHÈQUE DE PRESETS · 100 PRESETS D'USINE","BIBLIOTHÈQUE DE PRESETS - 100 PRESETS D'USINE","PRESET LIBRARY - 100 FACTORY PRESETS","BIBLIOTECA DE PRESETS - 100 PRESETS DE FÁBRICA");
        add("LE SON POUR CHAQUE ÉPOQUE","LE SON POUR CHAQUE ÉPOQUE","A SOUND FOR EVERY ERA","UN SONIDO PARA CADA ÉPOCA");
        add("Rechercher parmi 10 familles musicales","Rechercher parmi 10 familles musicales","Search 10 musical categories","Busca en 10 estilos musicales");
        add("TOUTES","TOUTES","ALL","TODAS");
        add("FACTORY","USINE","FACTORY","FÁBRICA");
        add("résultats","résultats","results","resultados");
        add("PAGE","PAGE","PAGE","PÁGINA");
        add("PRÉCÉDENT","PRÉCÉDENT","PREVIOUS","ANTERIOR");
        add("SUIVANT","SUIVANT","NEXT","SIGUIENTE");
        add("OUVRIR UN PRESET UTILISATEUR  (.plegends)","OUVRIR UN PRESET UTILISATEUR (.plegends)","OPEN USER PRESET (.plegends)","ABRIR PRESET DE USUARIO (.plegends)");
        add("LA COLLECTION LIBERTY  ·  80 MODÈLES DSP","COLLECTION LIBERTY - 80 MODÈLES DSP","LIBERTY COLLECTION - 80 DSP MODELS","COLECCIÓN LIBERTY - 80 MODELOS DSP");
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
        add("DRIVE","DRIVE","DRIVE","DRIVE");
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
        add("BOOST","BOOST","BOOST","IMPULSO");
        add("DIST","DISTORSION","DIST","DISTORSIÓN");
        add("FILTER","FILTRE","FILTER","FILTRO");
        add("FREQ","FRÉQUENCE","FREQ","FRECUENCIA");
        add("SHAPE","FORME","SHAPE","FORMA");
        add("REPEATS","RÉPÉTITIONS","REPEATS","REPETICIONES");
        add("NOISE GATE","PORTE DE BRUIT","NOISE GATE","PUERTA DE RUIDO");
        add("Saturation progressive, harmonies chaleureuses et attaque de guitare préservée.","Saturation progressive, harmoniques chaleureuses et attaque de guitare préservée.","Progressive saturation, warm harmonics and preserved pick attack.","Saturación progresiva, armónicos cálidos y ataque de púa conservado.");
        add("Distorsion articulée, grain dense et réponse précise aux variations de jeu.","Distorsion articulée, grain dense et réponse précise aux variations de jeu.","Articulate distortion, dense texture and precise response to playing dynamics.","Distorsión articulada, textura densa y respuesta precisa a la dinámica.");
        add("Texture vintage riche en harmoniques, de la douceur aux sons les plus rugueux.","Texture vintage riche en harmoniques, de la douceur aux sons les plus rugueux.","Harmonic-rich vintage textures, from soft to aggressive.","Texturas vintage ricas en armónicos, desde suaves hasta agresivas.");
        add("Compression souple inspirée des grands compresseurs de pédalier et de studio.","Compression souple inspirée des grands compresseurs de pédalier et de studio.","Smooth compression inspired by classic pedal and studio compressors.","Compresión suave inspirada en compresores clásicos de pedal y estudio.");
        add("Sculpture tonale expressive avec fréquences et résonances musicales.","Sculpture tonale expressive avec fréquences et résonances musicales.","Expressive tone shaping with musical frequencies and resonances.","Modelado tonal expresivo con frecuencias y resonancias musicales.");
        add("Filtrage résonant vivant, idéal pour un jeu rythmique et expressif.","Filtrage résonant vivant, idéal pour un jeu rythmique et expressif.","Lively resonant filtering, ideal for rhythmic and expressive playing.","Filtrado resonante y vivo para una interpretación rítmica y expresiva.");
        add("Mouvement enveloppant, profondeur et caractère inspirés des modulations classiques.","Mouvement enveloppant, profondeur et caractère inspirés des modulations classiques.","Immersive movement, depth and classic modulation character.","Movimiento envolvente, profundidad y carácter de modulación clásico.");
        add("Variation rythmique et mouvement du signal, du vintage au contemporain.","Variation rythmique et mouvement du signal, du vintage au contemporain.","Rhythmic dynamics and signal motion from vintage to modern.","Dinámica rítmica y movimiento de señal, de lo vintage a lo actual.");
        add("Répétitions musicales aux couleurs analogiques et numériques distinctives.","Répétitions musicales aux couleurs analogiques et numériques distinctives.","Musical repeats with distinctive analog and digital colours.","Repeticiones musicales con colores analógicos y digitales distintivos.");
        add("Espace et profondeur, avec décroissance musicale et détail des transitoires.","Espace et profondeur, avec décroissance musicale et détail des transitoires.","Space and depth with musical decay and transient detail.","Espacio y profundidad, con decaimiento musical y detalle de transitorios.");
        add("Effet d'octave monophonique et nuances harmoniques pour guitare et basse.","Effet d'octave monophonique et nuances harmoniques pour guitare et basse.","Monophonic octave processing and harmonic colour for guitar and bass.","Efectos de octava monofónicos y color armónico para guitarra y bajo.");
        add("Textures évolutives et délais créatifs pour l'expérimentation sonore.","Textures évolutives et délais créatifs pour l'expérimentation sonore.","Evolving textures and creative delays for sonic experimentation.","Texturas cambiantes y delays creativos para la experimentación sonora.");
        add("Préamplification dynamique et couleur musicale pour votre guitare.","Préamplification dynamique et couleur musicale pour votre guitare.","Dynamic preamplification and musical character for your guitar.","Preamplificación dinámica y color musical para tu guitarra.");
        add("Ibanez Tube Screamer — inspiration de médiums et d'overdrive classique","Ibanez Tube Screamer : inspiration pour les médiums et l’overdrive classique","Ibanez Tube Screamer: classic mid-focused overdrive inspiration","Ibanez Tube Screamer: inspiración del overdrive clásico de medios");
        add("Klon Centaur — famille des overdrives transparentes et dynamiques","Klon Centaur : famille des overdrives transparentes et dynamiques","Klon Centaur: transparent and dynamic overdrive family","Klon Centaur: familia de overdrives transparentes y dinámicas");
        add("Marshall Bluesbreaker — inspiration du crunch blues britannique","Marshall Bluesbreaker : inspiration du crunch blues britannique","Marshall Bluesbreaker: British blues crunch inspiration","Marshall Bluesbreaker: inspiración del crunch blues británico");
        add("Pro Co RAT — famille des distorsions rock avec filtrage marqué","Pro Co RAT : famille des distorsions rock à filtrage marqué","Pro Co RAT: rock distortion family with distinctive filtering","Pro Co RAT: familia de distorsiones rock con filtrado característico");
        add("Boss DS-1 — grande tradition des distorsions compactes","Boss DS-1 : tradition des distorsions compactes","Boss DS-1: classic compact distortion tradition","Boss DS-1: tradición clásica de distorsiones compactas");
        add("Dallas-Arbiter Fuzz Face — tempérament germanium vintage","Dallas-Arbiter Fuzz Face : caractère germanium vintage","Dallas-Arbiter Fuzz Face: vintage germanium character","Dallas-Arbiter Fuzz Face: carácter vintage de germanio");
        add("Fuzz Face silicium — caractère plus mordant et affirmé","Fuzz Face silicium : caractère plus mordant et affirmé","Silicon Fuzz Face: biting, assertive character","Fuzz Face de silicio: carácter incisivo y definido");
        add("Diamond Compressor — compression optique douce, référence esthétique générale","Diamond Compressor : référence générale de compression optique douce","Diamond Compressor: general smooth optical compression reference","Diamond Compressor: referencia general de compresión óptica suave");
        add("Ross Compressor — esprit de compression vintage et sustain","Ross Compressor : esprit de compression vintage et sustain","Ross Compressor: vintage compression and sustain character","Ross Compressor: carácter vintage de compresión y sustain");
        add("Boss CE-2 — richesse du chorus analogique","Boss CE-2 : richesse du chorus analogique","Boss CE-2: rich analog chorus tradition","Boss CE-2: tradición rica del chorus analógico");
        add("MXR Phase 90 — phasing analogique classique","MXR Phase 90 : phasing analogique classique","MXR Phase 90: classic analog phaser reference","MXR Phase 90: referencia del phaser analógico clásico");
        add("Electro-Harmonix Electric Mistress — esprit du flanging vintage","Electro-Harmonix Electric Mistress : esprit du flanger vintage","Electro-Harmonix Electric Mistress: vintage flanger inspiration","Electro-Harmonix Electric Mistress: inspiración de flanger vintage");
        add("Maestro Echoplex — échos à bande et saturation du temps","Maestro Echoplex : échos à bande et saturation","Maestro Echoplex: tape echo and saturation inspiration","Maestro Echoplex: inspiración de eco de cinta y saturación");
        add("Electro-Harmonix Deluxe Memory Man — inspiration de délai à modulation","Electro-Harmonix Deluxe Memory Man : inspiration de délai modulé","Electro-Harmonix Deluxe Memory Man: modulated delay inspiration","Electro-Harmonix Deluxe Memory Man: inspiración de delay modulado");
        add("Réverbérations à ressort des amplificateurs vintage","Réverbérations à ressort des amplificateurs vintage","Vintage amplifier spring reverbs","Reverberaciones de muelles de amplificadores vintage");
        add("Réverbérations à plaque de studio des années 1970","Réverbérations à plaque de studio des années 1970","1970s studio plate reverbs","Reverberaciones de placa de estudio de los años 70");
        add("Égaliseurs de tonalité analogiques pour guitare","Égaliseurs analogiques pour guitare","Analog guitar tone-shaping EQs","Ecualizadores analógicos para guitarra");
        add("Vox et Cry Baby — familles de pédales wah emblématiques","Vox et Cry Baby : familles de pédales wah emblématiques","Vox and Cry Baby: landmark wah pedal families","Vox y Cry Baby: familias emblemáticas de pedales wah");
        add("intro_1","Les effets de la famille ","Effects in the ","Los efectos de la familia ");
        add("intro_2"," ont façonné le son de la guitare électrique. Depuis "," family have shaped the electric guitar sound. Since "," han marcado el sonido de la guitarra eléctrica. Desde ");
        add("intro_3",", les musiciens ont exploré leurs réponses et leurs couleurs. ",", players have explored their response and character. ",", los músicos han explorado su respuesta y carácter. ");
        add("intro_4"," propose une interprétation numérique originale, pensée pour le jeu et la création musicale."," offers an original DSP interpretation designed for performance and sound creation."," ofrece una interpretación DSP original para interpretar y crear música.");
        add("tone_suffix"," Les paramètres répondent au niveau d’entrée, au toucher et à l’ordre des effets. Chaque modèle possède sa signature."," Controls react to input level, touch and effect order. Each model has its own signature."," Los controles reaccionan al nivel de entrada, la pulsación y el orden. Cada modelo tiene su carácter.");
        add("behavior","Ce modèle traite réellement l’audio dans son slot, avec des réglages indépendants, une compensation de niveau et un bypass progressif. La chaîne suit l’ordre 1 à 8.","This model processes audio in its own slot, with independent controls, level trim and smooth bypass. The chain follows slots 1 to 8.","Este modelo procesa audio en su espacio, con controles independientes, nivel y bypass suave. La cadena sigue el orden del 1 al 8.");
        add("tip_start1","Chargez ","Load ","Carga ");
        add("tip_start2"," seul sur un emplacement, désactivez les autres effets et ajustez progressivement. Comparez les sons au même niveau de sortie."," alone in one slot, bypass other effects and adjust gradually. Compare at matched output levels."," solo en un espacio, desactiva otros efectos y ajusta gradualmente. Compara a igual nivel de salida.");
        add("tip_chain","Testez différentes positions avant et après les saturations, puis ajoutez modulation, délai ou réverbération selon votre intention musicale.","Try different positions before and after drive pedals, then add modulation, delay or reverb for your musical goal.","Prueba posiciones antes y después de las saturaciones; luego añade modulación, delay o reverb según tu objetivo.");
        add("tip_exp","Testez un son clair, un riff dynamique et des accords longs. Ajustez le mix par slot pour conserver la définition sans perdre la couleur.","Try clean tones, dynamic riffs and sustained chords. Adjust per-slot mix to preserve clarity and colour.","Prueba sonidos limpios, riffs dinámicos y acordes largos. Ajusta la mezcla por espacio para conservar claridad y color.");
        add("tip_sound","Commencez avec un mix modéré puis ajustez le gain et le caractère à votre guitare. Ces valeurs sont indicatives : aucun réglage n’est appliqué automatiquement.","Start with a moderate mix and tailor gain and tone to your guitar. Values are suggestions; nothing is applied automatically.","Empieza con mezcla moderada y adapta la ganancia y el carácter. Son sugerencias; nada se aplica automáticamente.");
        add("history_disclaimer","Influence de famille, sans revendication de copie officielle ni reproduction identique.","General influence, without any claim of official or identical replication.","Influencia general, sin afirmaciones de réplica oficial o idéntica.");
        add("choose_slot","CHOISIR LA PÉDALE DE L’EMPLACEMENT ","CHOOSE PEDAL FOR SLOT ","ELEGIR PEDAL DEL ESPACIO ");
        add("stage_label","EMPLACEMENT ","SLOT ","ESPACIO ");
        add("entry_label","FICHE ","ENTRY ","FICHA ");
        add("period_intro","Inspirations musicales : ","Musical inspirations: ","Inspiraciones musicales: ");
        add("history_help","L’interface historique est en lecture seule.","The history view is read-only.","La vista histórica es de solo lectura.");
        return result;
    }();
    return content;
}
inline juce::String translate(const juce::String& text,int language) {
    const auto& entries=dictionary();
    const auto it=entries.find(text.toStdString());
    return it==entries.end()?text:it->second[size_t(safeLanguage(language))];
}
// Special icons are painted as geometry or written using portable ASCII.
// Keep French/Spanish accents: JUCE's Unicode font fallback renders them.
inline juce::String safeTypography(juce::String text) {
    // Non-letter ornament glyphs frequently have missing character glyphs
    // in AU/VST3 hosts. Cache replacements to avoid per-frame allocations.
    static const std::array<std::pair<juce::String,juce::String>,14> replacements{{
        {decoded("•"),"-"}, {decoded("·"),"-"}, {decoded("—"),"-"},
        {decoded("–"),"-"}, {decoded("−"),"-"}, {decoded("→"),">"},
        {decoded("‹"),"<"}, {decoded("›"),">"}, {decoded("×"),"X"},
        {decoded("▾"),"v"}, {decoded("●"),""}, {decoded("○"),""},
        {decoded("▣"),"i"}, {decoded("©"),"(c)"}
    }};
    for(const auto& replacement:replacements)
        if(text.contains(replacement.first))text=text.replace(replacement.first,replacement.second);
    return text;
}
}
