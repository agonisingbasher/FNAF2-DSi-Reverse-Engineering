(() => {
  const translations = {
    back: '← Back to portfolio',
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
    feedbackText: 'Send feedback on the project page at', orOpen: 'or open an issue on',
    feedbackDetails: '. Include the version, device/emulator, night, reproduction steps and a log if available. Do not post personal data or public save files.',
    versionNote: 'Published ROM: V19.7. Supplied source: V19.6.2.',
    footer: 'Independent, non-commercial project. Characters and assets belong to their respective owners.'
  };
  const button = document.getElementById('language');
  let lang = 'pt';
  const original = new Map([...document.querySelectorAll('[data-i18n]')].map(el => [el, el.textContent]));
  button.addEventListener('click', () => {
    lang = lang === 'pt' ? 'en' : 'pt';
    document.documentElement.lang = lang === 'pt' ? 'pt-BR' : 'en';
    button.textContent = lang === 'pt' ? 'EN' : 'PT';
    button.setAttribute('aria-pressed', String(lang === 'en'));
    button.setAttribute('aria-label', lang === 'pt' ? 'Mudar idioma para inglês' : 'Switch language to Portuguese');
    for (const [el, text] of original) {
      const key = el.dataset.i18n;
      el.textContent = lang === 'pt' ? text : (translations[key] ?? text);
    }
  });
})();
