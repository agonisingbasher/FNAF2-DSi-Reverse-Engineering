(() => {
  const translations = {
    back: '← Back to project',
    portfolioHome: 'Main portfolio ↗',
    title: 'Player quick guide',
    releaseNotice: 'The published ROM is identified as V19.7. The supplied source is V19.6.2; controls below were checked against that source and may differ from the published ROM. The published recording was made in an emulator on a computer; this guide does not confirm V19.7 testing on physical hardware.',
    downloadTitle: 'Download and start',
    download1: 'Visit the official project page and choose a download.',
    gamejoltLink: 'Game Jolt', or: 'or download the published ROM from the', romLink: 'GitHub repository',
    download2: 'The published recording shows the game running in an emulator on a computer. V19.6.2 source notes document emulator or homebrew DSi use, but do not confirm that ROM V19.7 was tested on hardware. No other console or method is documented.',
    download3: 'On first startup, the game prepares progress files in the available storage. If writing is unavailable, progress may not be saved.',
    savesTitle: 'Preserve your save files',
    savesText: 'Do not delete, overwrite, or move the save folder between versions without making a backup. The V19.6.2 source initializes storage and records the selected directory; names/paths depend on the variant and environment. Back up the save folder before changing ROMs.',
    controlsTitle: 'Controls checked in V19.6.2 source',
    controlsCaveat: 'These commands are implemented in the supplied source, not a guarantee of a complete match with ROM V19.7.',
    cameraInput: 'ABXY (monitor open)', tabletInput: 'Down / B (monitor closed)', lightInput: 'X / Y / A (monitor closed)', cameraLightInput: 'R (monitor open)', windInput: 'L (CAM 11)', endInput: 'A / START / B (Game Over and victory)',
    officeDpadInput: '← / → (monitor closed)', officeDpad: 'Move/pan across the office.', customInput: 'Custom Night', customControls: 'D-pad selects/focuses; A increases and B decreases AI (0–20); X changes focus; touchscreen can select presets/controls; START begins Night 7.',
    input: 'Input', action: 'Action',
    touchMap: 'Tap a room on the lower-screen map to choose a camera; use the lower panel for tablet, mask and lights.',
    cameraButtons: 'A/X/Y/B move between neighboring cameras in different directions.',
    tabletMask: 'Down opens/closes the tablet; B uses the mask.',
    lights: 'X: center hallway light; Y: left vent light; A: right vent light. The center light can also be blocked by game state.',
    cameraLight: 'Hold for the camera light while battery remains; the touchscreen panel can also toggle it.',
    windBox: 'Hold to wind the Music Box when the selected camera and game state allow it.',
    upAction: 'Horn in the office when allowed; in other states, mutes the phone call.',
    endButtons: 'A or START confirms Retry/Continue; B returns to the menu. A touch held while entering the screen must be released before activation.',
    feedbackTitle: 'Report an issue',
    feedbackLead: 'Send feedback or report a bug through the', feedbackLink: 'project feedback page',
    feedbackDetails: '. Include the version, device/emulator, night, reproduction steps and a log if available. You can attach up to 5 images and 1 short video. Do not include personal data or public save files.',
    versionNote: 'Published ROM: V19.7. Supplied source: V19.6.2.',
    footer: 'Independent, non-commercial project. Characters and assets belong to their respective owners.',
    pageTitle: 'Player guide | FNAF 2 DSi',
    pageDescription: 'Quick guide to play FNAF 2 DSi and send feedback.'
  };
translations.es = {"back":"← Volver al proyecto","portfolioHome":"Portafolio principal ↗","title":"Guía rápida del jugador","releaseNotice":"La ROM publicada se identifica como V19.7. El código fuente disponible es V19.6.2; los controles de abajo se verificaron en ese código fuente y pueden diferir de la ROM publicada. La grabación publicada se hizo en un emulador en la computadora; esta guía no confirma pruebas de la ROM V19.7 en hardware físico.","downloadTitle":"Descargar e iniciar","download1":"Abre la página oficial del proyecto y elige una descarga.","gamejoltLink":"Game Jolt","or":"o descarga la ROM publicada en el","romLink":"repositorio GitHub","download2":"La grabación publicada muestra el juego ejecutándose en un emulador en la computadora. El código fuente V19.6.2 documenta la ejecución en un emulador o en una DSi con homebrew, pero no confirma que la ROM V19.7 se haya probado en hardware. No está documentada ninguna otra consola o método.","download3":"En el primer arranque el juego prepara los archivos de progreso en el almacenamiento disponible. Si no se puede escribir, es posible que el progreso no se guarde.","savesTitle":"Protege tus partidas guardadas","savesText":"No borres, sobrescribas ni muevas la carpeta de partidas guardadas entre versiones sin hacer antes una copia de seguridad. El código fuente V19.6.2 inicializa el almacenamiento y registra la carpeta elegida; los nombres/rutas dependen de la variante y del entorno. Si vas a cambiar la ROM, copia primero la carpeta de partidas guardadas.","controlsTitle":"Controles verificados en el código fuente V19.6.2","controlsCaveat":"Tabla de los comandos implementados en el código fuente publicado, no una garantía de coincidencia total con la ROM V19.7.","input":"Entrada","action":"Acción","touchMap":"Toca el mapa de la pantalla inferior para elegir una cámara; el panel inferior activa la tablet, la máscara y las luces.","cameraInput":"ABXY (monitor abierto)","cameraButtons":"A/X/Y/B recorren las cámaras vecinas en distintas direcciones.","tabletInput":"↓ / B (monitor cerrado)","tabletMask":"↓ abre/cierra la tablet; B activa la máscara.","lightInput":"X / Y / A (monitor cerrado)","lights":"X: luz central; Y: luz izquierda; A: luz derecha. La luz central también puede quedar bloqueada por el estado del juego.","cameraLightInput":"R (monitor abierto)","cameraLight":"Mantén la luz de la cámara mientras quede batería; un toque en el panel también puede alternarla.","windInput":"L (CAM 11)","windBox":"Mantén el comando para dar cuerda a la Music Box cuando la cámara y el estado lo permiten.","upAction":"Horn en la oficina cuando está permitido; en otros estados, silencia la llamada.","endInput":"A / START / B (Game Over y victoria)","endButtons":"A o START confirma Retry/Continue; B vuelve al menú. Hay que soltar los toques que mantengas al entrar en la pantalla antes de que se activen.","officeDpadInput":"← / → (monitor cerrado)","officeDpad":"Mueve o desplaza la vista por la oficina.","customInput":"Custom Night","customControls":"El D-pad selecciona y enfoca; A sube y B baja la IA (0–20); X cambia el enfoque; la pantalla táctil permite elegir presets y controles; START comienza Night 7.","feedbackTitle":"Reportar un problema","feedbackLead":"Envía comentarios o reporta un bug desde la","feedbackLink":"página de comentarios del proyecto","feedbackDetails":". Incluye la versión, el dispositivo o emulador, la noche, los pasos para reproducirlo y el log si está disponible. Puedes adjuntar hasta 5 imágenes y 1 video corto. No incluyas datos personales ni archivos de save públicos.","versionNote":"ROM publicada: V19.7. Código fuente publicado: V19.6.2.","footer":"Proyecto independiente, sin fines comerciales. Los derechos de los personajes y los assets pertenecen a sus titulares.","pageTitle":"Guía del jugador | FNAF 2 DSi","pageDescription":"Guía rápida para jugar a FNAF 2 DSi y enviar comentarios."};
translations.fr = {"back":"← Retour au projet","portfolioHome":"Portefolio principal ↗","title":"Guide rapide du joueur","releaseNotice":"La ROM publiée est identifiée comme V19.7. La source disponible est la V19.6.2 ; les contrôles ci-dessous ont été vérifiés sur cette source et peuvent différer de la ROM publiée. La capture publiée a été réalisée dans un émulateur sur ordinateur ; ce guide ne confirme aucun test de la ROM V19.7 sur matériel physique.","downloadTitle":"Télécharger et démarrer","download1":"Ouvrez la page officielle du projet et choisissez un téléchargement.","gamejoltLink":"Game Jolt","or":"ou téléchargez la ROM publiée depuis le","romLink":"dépôt GitHub","download2":"La capture publiée montre une exécution dans un émulateur sur ordinateur. La source V19.6.2 documente une exécution en émulateur ou sur DSi avec du homebrew, mais ne confirme pas que la ROM V19.7 ait été testée sur le matériel. Aucune autre console ni aucune autre méthode n'est documentée.","download3":"Au premier lancement, le jeu prépare les fichiers de progression dans le stockage disponible. Si l'écriture est impossible, la progression risque de ne pas être enregistrée.","savesTitle":"Protégez vos sauvegardes","savesText":"Ne supprimez pas, n'écrasez pas et ne déplacez pas le dossier de sauvegardes d'une version à l'autre sans en faire une copie de sécurité. La source V19.6.2 initialise le stockage et enregistre le répertoire choisi ; les noms/chemins dépendent de la variante et de l'environnement. Avant de changer de ROM, copiez le dossier de sauvegardes.","controlsTitle":"Contrôles vérifiés sur la source V19.6.2","controlsCaveat":"Table des commandes implémentées dans la source mise à disposition, sans garantie de correspondance intégrale avec la ROM V19.7.","input":"Entrée","action":"Action","touchMap":"Touchez la carte de l'écran du bas pour choisir une caméra ; le panneau du bas actionne la tablette, le masque et les lumières.","cameraInput":"ABXY (moniteur ouvert)","cameraButtons":"A/X/Y/B parcourent les caméras voisines dans des directions différentes.","tabletInput":"↓ / B (moniteur fermé)","tabletMask":"↓ ouvre/ferme la tablette ; B actionne le masque.","lightInput":"X / Y / A (moniteur fermé)","lights":"X : lumière centrale ; Y : lumière gauche ; A : lumière droite. La lumière centrale peut aussi être bloquée par l'état du jeu.","cameraLightInput":"R (moniteur ouvert)","cameraLight":"Maintenez la lumière de la caméra tant qu'il reste de la batterie ; un toucher sur le panneau peut aussi l'alterner.","windInput":"L (CAM 11)","windBox":"Maintenez la commande pour remonter la Music Box lorsque la caméra et l'état du jeu le permettent.","upAction":"Horn dans le bureau quand c'est autorisé ; dans les autres états, il met l'appel en sourdine.","endInput":"A / START / B (Game Over et victoire)","endButtons":"A ou START confirme Retry/Continue ; B revient au menu. Les appuis maintenus en arrivant sur l'écran doivent être relâchés avant de s'activer.","officeDpadInput":"← / → (moniteur fermé)","officeDpad":"Déplacez la vue et balayez le bureau.","customInput":"Custom Night","customControls":"Le D-pad sélectionne et déplace le focus ; A augmente et B diminue l'IA (0–20) ; X change le focus ; l'écran tactile permet de choisir les préréglages et les commandes ; START lance Night 7.","feedbackTitle":"Signaler un problème","feedbackLead":"Envoyez un retour ou signalez un bug depuis la","feedbackLink":"page de retours du projet","feedbackDetails":". Indiquez la version, l'appareil ou l'émulateur, la nuit, les étapes pour reproduire le problème et le log si possible. Vous pouvez joindre jusqu'à 5 images et 1 vidéo courte. N'incluez pas de données personnelles ni de fichiers de save publics.","versionNote":"ROM publiée : V19.7. Source mise à disposition : V19.6.2.","footer":"Projet indépendant et non commercial. Les droits sur les personnages et les assets appartiennent à leurs titulaires.","pageTitle":"Guide du joueur | FNAF 2 DSi","pageDescription":"Guide rapide pour jouer à FNAF 2 DSi et envoyer vos retours."};
  const LANGUAGES = ['pt-BR', 'en', 'es', 'fr'];
  const languageButtons = document.querySelectorAll('[data-lang-option]');
  const languageStorageKey = 'fnaf-guide-language';
  const original = new Map([...document.querySelectorAll('[data-i18n]')].map(el => [el, el.textContent]));
  const pageMetaDescription = document.querySelector('meta[name="description"]');
  const pageDefaults = { title: document.title, description: pageMetaDescription ? pageMetaDescription.content : '' };
  let lang = 'pt-BR';

  /* O ingles fica no proprio `translations`; es/fr sao dicts aninhados. Em portugues
   nao ha dict: o texto do HTML e a fonte. */
  function dictFor(code) {
    if (code === 'pt-BR') return null;
    return code === 'en' ? translations : translations[code];
  }

  function applyLanguage(code) {
    if (LANGUAGES.includes(code)) lang = code;
    const dict = dictFor(lang);
    document.documentElement.lang = lang === 'pt-BR' ? 'pt-BR' : lang;
    const pick = (key, fallback) => (dict && dict[key] !== undefined ? dict[key] : fallback);
    document.title = pick('pageTitle', pageDefaults.title);
    if (pageMetaDescription) pageMetaDescription.content = pick('pageDescription', pageDefaults.description);
    for (const [el, text] of original) {
      const key = el.dataset.i18n;
      const value = dict && dict[key] !== undefined ? dict[key] : text;
      el.textContent = value;
    }
    languageButtons.forEach((button) => {
      button.setAttribute('aria-pressed', String(button.dataset.langOption === lang));
    });
    try {
      localStorage.setItem(languageStorageKey, lang);
    } catch (error) {
      if (error.name !== 'SecurityError') throw error;
    }
    const pageUrl = new URL(window.location.href);
    pageUrl.searchParams.set('lang', lang);
    history.replaceState(null, '', pageUrl);
  }

  /* Sem ?lang= na URL e sem escolha gravada, o navegador decide: quem abre de Buenos
     Aires ve espanhol, quem abre de Paris ve frances. Um pt de qualquer pais continua
     em pt-BR, que e a unica variante portuguesa que temos. */
  function detectLanguage() {
    const preferred = navigator.languages && navigator.languages.length ? navigator.languages : [navigator.language];
    for (const tag of preferred) {
      if (!tag) continue;
      const base = String(tag).toLowerCase().split('-')[0];
      if (base === 'pt') return 'pt-BR';
      const match = LANGUAGES.find((code) => code.toLowerCase() === base);
      if (match) return match;
    }
    return 'pt-BR';
  }

  function readStoredLanguage() {
    try {
      const stored = localStorage.getItem(languageStorageKey);
      return LANGUAGES.includes(stored) ? stored : null;
    } catch (error) {
      if (error.name !== 'SecurityError') throw error;
      return null;
    }
  }

  languageButtons.forEach((button) => {
    button.addEventListener('click', () => applyLanguage(button.dataset.langOption));
  });

  const requestedLanguage = new URLSearchParams(window.location.search).get('lang');
  applyLanguage(LANGUAGES.includes(requestedLanguage) ? requestedLanguage : readStoredLanguage() || detectLanguage());
})();
