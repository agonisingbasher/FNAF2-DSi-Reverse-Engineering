const translations={en:{navInteractions:'Controls',interactionTitle:'Not just adapted. Redesigned for play.',interactionIntro:'The top display shows the action, while the touchscreen hosts a dedicated security center. Interaction is designed for the handheld format, not simply a PC game squeezed onto one display.',interaction1:'Direct camera selection',interaction1Text:'Tap rooms on the lower-screen map to change the monitored camera on the upper screen.',interaction2:'Flashlights and mask',interaction2Text:'Access the hallway and vent lights and equip the mask using the security controls.',interaction3:'Tablet and surveillance',interaction3Text:'Open or close the tablet, inspect cameras, and manage surveillance across both screens.',interaction4:'Continue to the next night',interaction4Text:'Custom victory flow, shift report, and Continue / Game Menu controls.',interaction5:'Custom Night',interaction5Text:'A configuration interface designed to adjust animatronic AI levels directly on DSi. Functionality must be verified against the published build.',interaction6:'Custom loading screens',interaction6Text:'Loading animations, boot terminal, Game Over, victory and menu states designed for this version.',comparisonTitle:'From PC to DSi: fundamentally different architectures.',comparisonIntro:'A comparison between the PC original’s published minimum requirements and the console’s hardware. These are not directly comparable benchmarks.',reqProcessor:'Processor',reqMemory:'RAM',reqGraphics:'Published graphics requirement',reqStorage:'Storage',reqPlatform:'Platform',reqSource:'Source: official Steam requirements ↗',dsScreens:'Displays',dsTouch:'Input',dsTouchValue:'Touchscreen + buttons',dsExecution:'Runtime',dsSource:'Source: official Nintendo DSi manual (displays) ↗',approachTitle:'Behavior reverse engineering + system reimplementation + original UX',approachText:'This is not Windows software running on the console, nor simply downscaled visuals. Logic and interfaces were rebuilt for homebrew, with navigation, loading, presentation and dual-screen interactions designed for the device.',approachCaveat:'Technical note: 1 GB and 16 MB describe different hardware platforms; this comparison is not a measurement of the game’s actual memory usage.',demoResolutionValue:'250 × 370 (cropped)',navGallery:'Gallery',navDemo:'Video',demoTitle:'Watch the project in action.',demoIntro:'Gameplay recorded on a computer using a Nintendo DSi emulator. See the dual-screen interface, menus, and the experience developed.',videoFallback:'Your browser does not support HTML5 video.',videoTag:'REAL PROJECT FOOTAGE · PC EMULATOR',videoDownload:'DOWNLOAD MP4 ↗',demoAsideTitle:'One console. Two screens. One challenge.',demoAsideText:'This demonstration was recorded on a computer emulator, not on physical Nintendo DSi hardware.',demoPlatform:'Target platform',demoCapture:'Recording environment',demoCaptureValue:'Emulator / computer',demoResolution:'Video resolution',demoLength:'Duration',demoNote:'On-demand playback: the video does not autoplay and loads only when requested.',galleryTitle:'Real project screenshots.',galleryIntro:'Screenshots provided by the developer, preserving the two vertically stacked DSi displays.',shot1:'Main menu',shot2:'System boot',shot3:'Office',shot4:'Camera map',shot5:'6 AM victory',navProject:'Project',navEngineering:'Engineering',navTimeline:'Timeline',navContact:'Contact',heroLead:'A reverse-engineered reimplementation with a bespoke interface, adapted mechanics, and native interaction across both Nintendo DSi XL screens.',source:'VIEW SOURCE ON GITHUB ↗',explore:'WATCH GAMEPLAY ▶',metricRelease:'RELEASE',metricScreens:'INTEGRATED SCREENS',metricStack:'PROGRAMMING',metricTarget:'TARGET HARDWARE',projectTitle:'A challenge beyond the game.',projectText:'Beyond reproducing the original, we reconstructed behaviors and designed a new DSi experience: menus, transitions, custom loading screens, and controls built around two displays.',feature1:'Camera system',feature1Text:'Interactive tablet, room switching, lighting, and a dual-screen interface.',feature2:'Animatronic AI',feature2Text:'Behaviors, movement routes, reaction windows, and night balancing.',feature3:'UI and state systems',feature3Text:'Menus, introduction, cutscenes, loading, Game Over, and victory.',engineeringTitle:'Not a direct port. A reconstruction.',challengeTitle:'Engineering challenges',challengeText:'We studied the original PC gameplay, rebuilt systems in C/C++, and created custom interfaces, loading transitions and interaction flows for a handheld with limited memory and two displays.',timelineTitle:'Iterative evolution.',t1:'Earlier stability milestone.',t2:'Transition fixes, interfaces, audio, and result screens.',t3:'Intro flow, Custom Night, loading, and reaction-time adjustments.',t4:'Release prepared for public project presentation.',contactTitle:'From project to career.',contactText:'This project showcases continuous learning, logical reasoning, debugging, optimization, and the ability to turn complex requirements into working software.',footer:'Independent, non-commercial project. Characters and assets belong to their respective owners.'}};translations.en.socialInvite='Enjoyed the project? Follow for new videos and updates.';translations.en.portfolioHome='MAIN PORTFOLIO ?';translations.en.socialYoutube='FOLLOW ON YOUTUBE ↗';translations.en.socialInstagram='FOLLOW ON INSTAGRAM ↗';const original={};document.querySelectorAll('[data-i18n]').forEach(el=>original[el.dataset.i18n]=el.textContent);let lang='pt';document.getElementById('language').addEventListener('click',()=>{lang=lang==='pt'?'en':'pt';document.documentElement.lang=lang==='pt'?'pt-BR':'en';const languageButton=document.getElementById('language');languageButton.textContent=lang==='pt'?'EN':'PT';languageButton.setAttribute('aria-pressed',String(lang==='en'));languageButton.setAttribute('aria-label',lang==='pt'?'Mudar idioma para inglês':'Switch language to Portuguese');document.querySelectorAll('[data-i18n]').forEach(el=>{const key=el.dataset.i18n;el.textContent=lang==='pt'?(original[key]??el.textContent):(translations.en[key]??original[key]??el.textContent);});});

Object.assign(translations.en, {
  gamejoltHero: '🎮 GAME JOLT · VIEW GAME ↗',
  presentation: 'WATCH THE PROJECT PRESENTATION ON YOUTUBE ↗',
  releaseStatus: 'NOW ON GAME JOLT',
  releaseTitle: 'Discover FNAF 2 DSi on Game Jolt.',
  releaseIntro: 'Visit the published V19.7 ROM page, follow updates and check download options. The browsable source is V19.6.2; no equivalence between them is established.',
  releaseAction: '🎮 OPEN GAME JOLT PAGE ↗',
  releaseSource: 'BROWSE V19.6.2 SOURCE ↗',
  releaseNote: 'Unofficial fan project. File availability is shown on the release page.'
});

// Subtle reveal effects. Content remains visible without JavaScript or when reduced motion is preferred.
if ('IntersectionObserver' in window && !window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
  const revealItems = document.querySelectorAll('.hero-grid > div, .metrics > div, .section:not(.demo-section) > h2, .section-intro, .feature-grid > article, .engineering-grid > *, .timeline.detailed > div, .demo-section .reveal, .interaction-section .reveal:not(.interaction-shot), .comparison-section .reveal');
  const observer = new IntersectionObserver((entries, obs) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) { entry.target.classList.add('is-visible'); obs.unobserve(entry.target); }
    });
  }, { threshold: 0.08, rootMargin: '0px 0px -20px 0px' });
  revealItems.forEach((el, i) => { el.classList.add('reveal-on-scroll'); el.style.setProperty('--reveal-delay', `${(i % 4) * 65}ms`); observer.observe(el); });
}

const visitorCounter = document.createElement('span');
visitorCounter.style.cssText = 'display:inline-flex;align-items:center;gap:8px;white-space:nowrap';

const visitorCounterImage = document.createElement('img');
visitorCounterImage.alt = lang === 'pt' ? 'Contador de visualizações do site' : 'Site view counter';
visitorCounterImage.referrerPolicy = 'no-referrer';
visitorCounterImage.style.cssText = 'opacity:.65;filter:grayscale(1)';
visitorCounterImage.src = 'https://visitor-badge.laobi.icu/badge?page_id=agonisingbasher.FNAF2-DSi-Reverse-Engineering&left_text=Views&left_color=090d13&right_color=090d13';

const visitorCounterFallback = document.createElement('span');
visitorCounterFallback.hidden = true;
visitorCounterImage.addEventListener('error', () => {
  console.error('Failed to load the visitor counter.');
  visitorCounterImage.hidden = true;
  visitorCounterFallback.hidden = false;
  visitorCounterFallback.textContent = lang === 'pt' ? 'Contador de visualizações indisponível' : 'View counter unavailable';
});

visitorCounter.append(visitorCounterImage, visitorCounterFallback);
document.querySelector('footer').append(visitorCounter);

document.getElementById('language').addEventListener('click', () => {
  visitorCounterImage.alt = lang === 'pt' ? 'Contador de visualizações do site' : 'Site view counter';
  if (visitorCounterFallback.hidden === false) {
    visitorCounterFallback.textContent = lang === 'pt' ? 'Contador de visualizações indisponível' : 'View counter unavailable';
  }
});

Object.assign(translations.en, {
  source: 'BROWSE V19.6.2 SOURCE ↗', sourceZip: 'DOWNLOAD SOURCE ZIP ↧', playerGuide: 'PLAYER GUIDE ↗', navValidation: 'Validation',
  nitroReadLink: 'loadNitroRaw() / main.c ↗',
  projectText: 'This independent project studies observable FNAF 2 behavior and reimplements systems in C/C++, with an original interface for both DSi screens. We did not have access to the original source code; the supplied V19.6.2 source and V19.7 ROM are distinct versions.',
  engineeringIntro: 'Short case studies based on the V19.6.2 source. “Implemented in code” does not mean the complete ROM was validated or tested on hardware.',
  problemLabel: 'Problem', investigationLabel: 'Investigation', decisionLabel: 'Technical decision', resultLabel: 'Verifiable result',
  caseMemoryTitle: 'Enter Night 1 without exhausting RAM', caseMemoryProblem: 'Transitioning into the first night could hang while preparing images and audio.', caseMemoryInvestigation: 'V19.3 notes describe large permanent arrays and title music retained beyond the title screen.', caseMemoryDecision: 'Title, WARNING and newspaper assets were moved to NitroFS; temporary title music is stopped and released before cutscenes/gameplay. Asset-open failures are checked before data is used.', caseMemoryResult: 'The structure is described in the V19.3 history and visible in the supplied source; this does not prove the same implementation in the V19.7 ROM or physical testing.', skillsDebugMemory: 'Skills: debugging · memory management · resource lifecycle', sourceFilesLink: 'Browse source/ ↗',
  caseRenderTitle: 'Balance memory, asset reads and rendering', caseRenderProblem: 'The development account reports memory and stability regressions after a more complex preload attempt; the audited source package contains no independent record of this episode.', caseRenderInvestigation: 'According to the account, the V18 loading/rendering structure was restored while preserving other changes. This is not corroborated by version history in the supplied package.', caseRenderDecision: 'The described approach limits simultaneous resources and accepts reads during loading rather than assuming everything must stay resident.', caseRenderResult: 'This is a process account, not a measured conclusion. No comparative benchmark or published smoothness data is available.', skillsRender: 'Skills: debugging · regression analysis · resource trade-offs',
  caseCameraTitle: 'Separate presence, light and frame', caseCameraProblem: 'Mangle’s presence, flashlight state and camera image are separate facts.', caseCameraInvestigation: 'On CAM 12, the logic selects a dark state with the light off; with the light on, it selects an empty or Mangle frame according to Mangle’s logical location.', caseCameraDecision: 'Frame selection checks light and logical location independently.', caseCameraResult: 'All three conditions are explicit in nightCameraFrame() in the source. Automated tests cover camera/AI logic but do not replace visual in-game validation.', skillsState: 'Skills: state machines · gameplay logic · testing',
  caseAudioTitle: 'Hallway warning follows the threat state', caseAudioProblem: 'The warning should reflect logical animatronic presence in the hallway, not whether the flashlight is on or the monitor is open.', caseAudioInvestigation: 'nightHallwayAudioActive() checks animatronic positions in the hallway and disables the warning in loss/win states.', caseAudioDecision: 'hall.raw is loaded into a buffer and played as a hardware loop when an animatronic is in the hallway; this does not depend on light/monitor state. With no threat, the channel stops. Separately, when AI signals a hallway exit while the center light is on, the light is locked for 4 s after release; holding the control extends the lock.', caseAudioResult: 'Visible in V19.6.2 source; this does not confirm audio mix or operation in V19.7/on hardware.', skillsAudio: 'Skills: state machines · audio resource lifecycle',
  caseBalanceTitle: 'Reaction windows and difficulty', caseBalanceProblem: 'Feedback identified near-instant attacks and too little reaction time, especially for Withereds.', caseBalanceInvestigation: 'Rules use routes, per-night AI levels and mask windows; V19.6.2 records a 1500 ms minimum for Withereds and 1667 ms for Night 1.', caseBalanceDecision: 'Tune deadlines by threat/night and start the window after the office frame is presented, so asset/render delays do not consume reaction time.', caseBalanceResult: 'The report lists tests for render wait, defense and timeout scenarios. This is not a claim of perfect balance or emulator/hardware gameplay testing.', skillsBalance: 'Skills: gameplay logic · iterative balancing · testing',
  validationTitle: 'Evidence, version and limits', validationIntro: 'Records from VALIDATION.json in the V19.6.2 source. This historical report was read, not rerun during this site revision.', validationSuites: 'automated suites recorded', validationSuitesText: 'Includes state, office, camera, AI, cutscene, contract/signature, victory timing, Game Over, ambience and Custom Night.', nitroFiles: 'NitroFS files verified', nitroFilesText: 'Packaged-content integrity recorded for V19.6.2 NORMAL and DEV ROMs.', romIntegrity: 'ROM integrity', romIntegrityText: 'The report records header/banner CRC, DSi sections, preserved ARM7, legacy trampoline and SHA-256 hashes for both variants.', notTested: 'Not tested', validationLimits: 'Emulator and physical hardware', validationLimitsText: 'VALIDATION.json sets hardware_tested=false. V19.6.2 notes also say the build was not tested in an emulator or on a physical DSi. This is not evidence that published ROM V19.7 was validated.', memoryNote: 'A separate historical V19.3 record reports 1,075,172 bytes (~1.03 MiB) for ARM9 text+data+BSS. It excludes all gameplay-time allocations and must not be extrapolated to V19.7.',
  tRecent1Title: 'Startup and memory', tRecent1Text: 'Historical notes describe moving title/newspaper assets to NitroFS and releasing temporary title music. The memory figure is specific to V19.3.', tRecent2Title: 'Victory and clock transition', tRecent2Text: 'Version documentation details the 5→6 AM transition and two-screen celebration.', tRecent3Title: 'Game Over and loading', tRecent3Text: 'Available notes describe retry/menu flows, victory controls and updates to interface/loading resources.', tRecent4Title: 'Balancing and Custom Night', tRecent4Text: 'The source includes reaction-window adjustments, tests and Custom Night with AI levels, presets and entry to Night 7.', tRecent5Title: 'Publication', tRecent5Text: 'The repository’s published ROM is identified as V19.7. No supplied source or evidence establishes equivalence with the V19.6.2 source.', historySource: 'Development notes; does not validate later ROMs', versionDifference: 'Published ROM: V19.7 · Supplied source: V19.6.2',
  interaction5Text: 'Custom Night is implemented in the V19.6.2 source: ten AI settings from 0–20, presets, touch/button navigation and starting Night 7. This does not confirm complete validation in ROM V19.7 or on hardware; preset names do not prove faithful original rules.',
  caseMemoryDecision: 'Title, WARNING and newspaper assets were moved to NitroFS; temporary title music is stopped and released before cutscenes/gameplay. loadNitroRaw() returns failure if a file cannot be opened or the read is incomplete; the caller chooses how to respond.',
  interaction5: 'Custom Night',
  contactTitle: 'My contribution to the project', developerName: 'Felipe Alexandre — requirements definition and refinement; interface and interaction decisions across both screens; analysis of expected behavior; testing, issue identification and correction direction; version tracking and delivery validation.', aiDisclosure: 'AI-assisted development: AI tools support investigation and implementation. My contribution includes defining requirements, making decisions, critically reviewing results and validating deliveries. I do not claim to have manually authored all code or assert an undocumented division of work.',
  sourceMismatch: 'Published ROM: V19.7. Source provided: V19.6.2.', releaseSource: 'BROWSE V19.6.2 SOURCE ↗',
  heroAlt: 'Two-screen DSi office capture', featuresLabel: 'Project features', promoLinkLabel: 'Open the FNAF 2 DSi Game Jolt page', promoAlt: 'FNAF 2 for DSi and DSi XL V19.7 promotional art on Game Jolt', videoLabel: 'FNAF 2 DSi gameplay demonstration recorded in an emulator on a computer',
  zoom1: 'Enlarge main menu', zoom2: 'Enlarge boot sequence', zoom3: 'Enlarge office', zoom4: 'Enlarge camera map', zoom5: 'Enlarge 6 AM victory', shotAlt1: 'Two DSi screens: main menu', shotAlt2: 'Two DSi screens: boot sequence', shotAlt3: 'Two DSi screens: office', shotAlt4: 'Two DSi screens: cameras', shotAlt5: 'Two DSi screens: 6 AM victory', interactionAlt: 'Two screens showing a monitored room and interactive camera map',
  heroLead: 'A behavior-based reverse-engineered reimplementation with an original interface and interaction across both Nintendo DSi XL screens.',
  experienceContractTitle: 'Interactive contract', experienceContract: 'Stylus input is accepted only inside the SIGN HERE box; CLEAR resets it and APPLY is enabled only after a signature. The source also implements signing with L as an alternative.',
  experienceProgressTitle: 'Progress and cutscenes', experienceProgress: 'Saved progress feeds Continue and unlocks; cutscenes use a lower-screen terminal. Storage can fail, so saving is not guaranteed when writes are unavailable.',
  experienceMomentsTitle: 'A two-screen gameplay loop', experienceMoments: 'The source implements the animated 5-to-6 AM transition, celebrations and per-night documents, Game Over Retry/Menu, and animated loading. Cameras, mask, lights, audio and battery respond to game state.',
  evidenceMemory: 'Evidence: README-V19.4.md (V19.3 history) + main.c (V19.6.2 source).', evidenceRender: 'Evidence: supplied process account; not corroborated in the audited source package.', evidenceCamera: 'Evidence: night_ai.c and V19.6.2 camera/AI suites.', evidenceAudio: 'Evidence: night_ai.c, office.c and office_state.c; V19.6.2 source.', evidenceBalance: 'Evidence: README_V19.6.2.md, night_ai.c and V19.6.2 tests.',
  tV7Title: 'Title prototype', tV7Text: 'Warning sequence and menu under test; diagnostic messages found in the binary.', tV9Title: 'Opening menu', tV9Text: 'Title structure and Night 1/Custom Night selection identifiable.', tV11Title: 'Menu expansion', tV11Text: 'More embedded content and named animatronic references.', tV12Title: 'Office and masks', tV12Text: 'References to office scenes, mask frames and camera feed.', tV13Title: 'Audio and end states', tV13Text: 'Audio-loading records and Game Over/Night Complete references.', tV14Title: 'Logic and debugging', tV14Text: 'Clock/timing diagnostics and references to office components.', tV15Title: 'Feature expansion', tV15Text: 'Significant binary-size increase and new phone-call data; exact content requires source code.', tV16Title: 'Cameras and controls', tV16Text: 'Diagnostics for camera open/selection and Music Box availability.', tV171Title: 'Incremental changes', tV171Text: 'Embedded content grew; specific changes cannot be determined from the ROM alone.', tV18Title: 'Introductions and cutscenes', tV18Text: 'References to night_intro, dream_mask and dream_puppet; described as a stable milestone.', tV181Title: 'Startup fix', tV181Text: 'Build named BootFix, with additional static-camera references.', tV190Title: 'Victory screen', tV190Text: 'NIGHT SURVIVED and TAP CONTINUE strings found in the binary.',
  archiveSize1: '1.00 MiB · archived ROM', archiveSize2: '0.94 MiB · archived ROM', archiveSize3: '2.44 MiB · archived ROM', archiveSize4: '5.27 MiB · archived ROM', archiveSize5: '5.54 MiB · archived ROM', archiveSize6: '10.51 MiB · archived ROM', archiveSize7: '22.04 MiB · archived ROM', archiveSize8: '22.67 MiB · archived ROM', archiveSize9: '25.60 MiB · archived ROM', archiveSize10: '25.65 MiB · archived ROM', archiveSize11: '25.17 MiB · archived ROM'
});

const staticTextTranslations = [
  ['.section-label', [
    ['sectionOverview', '01 / VISÃO GERAL DO PROJETO', '01 / PROJECT OVERVIEW'],
    ['sectionRelease', '// LANÇAMENTO PÚBLICO · COMUNIDADE', '// PUBLIC RELEASE · COMMUNITY HUB'],
    ['sectionDemo', '02 / DEMONSTRAÇÃO DE JOGABILIDADE', '02 / GAMEPLAY DEMONSTRATION'],
    ['sectionGallery', '03 / CAPTURAS REAIS DO JOGO', '03 / ACTUAL GAMEPLAY SCREENSHOTS'],
    ['sectionEngineering', '04 / DIÁRIO DE ENGENHARIA', '04 / ENGINEERING LOG'],
    ['sectionInteractions', '05 / PROJETADO PARA DUAS TELAS', '05 / DESIGNED FOR TWO SCREENS'],
    ['sectionConstraints', '06 / ENGENHARIA SOB RESTRIÇÕES', '06 / ENGINEERING UNDER CONSTRAINTS'],
    ['sectionValidation', '07 / TESTES E VALIDAÇÃO', '07 / TESTS & VALIDATION'],
    ['sectionHistory', '08 / HISTÓRICO DE DESENVOLVIMENTO', '08 / DEVELOPMENT HISTORY'],
    ['sectionDeveloper', '09 / PERFIL DO DESENVOLVEDOR', '09 / DEVELOPER PROFILE']
  ]],
  ['.case-card > span', [
    ['caseLabelMemory', '01 / MEMÓRIA E TRANSIÇÕES', '01 / MEMORY & TRANSITIONS'],
    ['caseLabelRender', '02 / REGRESSÃO DE RENDERIZAÇÃO', '02 / RENDERING REGRESSION'],
    ['caseLabelCamera', '03 / LÓGICA → IMAGEM', '03 / LOGIC → IMAGE'],
    ['caseLabelAudio', '04 / ÁUDIO GUIADO POR ESTADO', '04 / STATE-DRIVEN AUDIO'],
    ['caseLabelBalance', '05 / EQUILÍBRIO E REAÇÃO', '05 / BALANCE & REACTION']
  ]],
  ['.interaction-grid article > span', [
    ['interactionLabelTouch', '01 / TOQUE', '01 / TOUCH'],
    ['interactionLabelDefense', '02 / DEFESA', '02 / DEFENSE'],
    ['interactionLabelMonitoring', '03 / MONITORAMENTO', '03 / MONITORING'],
    ['interactionLabelFlow', '04 / PROGRESSÃO', '04 / FLOW'],
    ['interactionLabelCustomization', '05 / PERSONALIZAÇÃO', '05 / CUSTOMIZATION'],
    ['interactionLabelExperience', '06 / EXPERIÊNCIA', '06 / EXPERIENCE']
  ]],
  ['.experience-grid article > span', [
    ['experienceLabelContract', '07 / INTERAÇÃO E ACESSIBILIDADE', '07 / CONTRACT & ACCESSIBILITY'],
    ['experienceLabelProgression', '08 / PROGRESSÃO E CENAS', '08 / PROGRESSION & CUTSCENES'],
    ['experienceLabelMoments', '09 / MOMENTOS E SISTEMAS', '09 / MOMENTS & SYSTEMS']
  ]],
  ['.compare-card .compare-tag', [
    ['compareOriginal', 'ORIGINAL / PC', 'ORIGINAL / PC'],
    ['compareReimplementation', 'REIMPLEMENTAÇÃO / DSi XL', 'REIMPLEMENTATION / DSi XL']
  ]],
  ['.release-tags span', [
    ['releaseRomTag', 'ROM V19.7', 'V19.7 ROM'],
    ['releaseSourceTag', 'FONTE V19.6.2', 'V19.6.2 SOURCE'],
    ['releaseTargetTag', 'DSi + DSi XL', 'DSi + DSi XL'],
    ['releaseStackTag', 'C/C++', 'C/C++']
  ]],
  ['.demo-status', [['demoStatus', 'V19.7 / JOGABILIDADE', 'V19.7 / GAMEPLAY']]],
  ['.shot-note', [['shotNote', 'CAPTURA EM EMULADOR DSi // ESCRITÓRIO', 'DSi EMULATOR CAPTURE // OFFICE']]],
  ['.brand small', [['brandTag', 'SISTEMA DE SEGURANÇA // 1987', 'SECURITY SYSTEM // 1987']]],
  ['footer > span:first-child', [['footerProject', 'FNAF 2 DSi XL / PROJETO DE FÃ NÃO OFICIAL', 'FNAF 2 DSi XL / UNOFFICIAL FAN PROJECT']]],
  ['.contact .actions a', [
    ['linkedinLink', 'LINKEDIN ↗', 'LINKEDIN ↗'],
    ['githubLink', 'GITHUB ↗', 'GITHUB ↗']
  ]]
];

for (const [selector, entries] of staticTextTranslations) {
  document.querySelectorAll(selector).forEach((element, index) => {
    const entry = entries[index];
    if (!entry) return;
    const [key, portuguese, english] = entry;
    element.dataset.i18n = key;
    original[key] = portuguese;
    translations.en[key] = english;
    element.textContent = lang === 'pt' ? portuguese : english;
  });
}

const heroEyebrow = document.querySelector('.hero .eyebrow');
const heroEyebrowText = Array.from(heroEyebrow.childNodes).find(node => node.nodeType === Node.TEXT_NODE);
if (heroEyebrowText) {
  original.heroEyebrow = 'PROJETO HOMEBREW / NINTENDO DSi XL / V19.7';
  translations.en.heroEyebrow = 'HOMEBREW PROJECT / NINTENDO DSi XL / V19.7';
  heroEyebrowText.nodeValue = ` ${lang === 'pt' ? original.heroEyebrow : translations.en.heroEyebrow} `;
  heroEyebrow.dataset.i18n = 'heroEyebrow';
}

const feedbackLink = document.createElement('a');
feedbackLink.className = 'feedback-cta';
feedbackLink.href = 'feedback.html';
feedbackLink.dataset.i18n = 'feedbackLink';
feedbackLink.textContent = 'Feedback e bugs →';
original.feedbackLink = feedbackLink.textContent;
translations.en.feedbackLink = 'Feedback & bug reports →';
document.querySelector('footer').append(feedbackLink);

const originalAttributes = new Map();
document.querySelectorAll('[data-i18n-alt],[data-i18n-aria-label]').forEach(el => {
  originalAttributes.set(el, { alt: el.getAttribute('alt'), ariaLabel: el.getAttribute('aria-label') });
});
document.getElementById('language').addEventListener('click', () => {
  for (const [el, originalValue] of originalAttributes) {
    const altKey = el.dataset.i18nAlt;
    const labelKey = el.dataset.i18nAriaLabel;
    if (altKey) el.setAttribute('alt', lang === 'pt' ? originalValue.alt : (translations.en[altKey] ?? originalValue.alt));
    if (labelKey) {
      const value = lang === 'pt' ? originalValue.ariaLabel : (translations.en[labelKey] ?? originalValue.ariaLabel);
      if (value) el.setAttribute('aria-label', value);
    }
  }
});
