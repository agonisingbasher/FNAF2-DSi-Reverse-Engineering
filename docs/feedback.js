// Set a CORS-enabled HTTPS URL. Accept multipart fields name, email, title, description, repeated images, and video; validate limits and route mail to felipsempre10@gmail.com server-side.
const FEEDBACK_ENDPOINT = '';

const translations = {
  en: {
    backHome: 'Back to project',
    brandTag: 'SECURITY SYSTEM // 1987',
    feedbackEyebrow: 'FEEDBACK CENTER',
    feedbackHeading: 'Feedback & bug reports',
    feedbackIntro: 'Tell us what happened or share a suggestion. Fields marked * are required.',
    nameLabel: 'Name *',
    emailLabel: 'Contact (email) *',
    titleLabel: 'Title *',
    descriptionLabel: 'Description *',
    imagesLabel: 'Images (up to 5)',
    imagesHint: 'Select no more than 5 images.',
    videoLabel: 'Video (up to 1)',
    videoHint: 'Select one video file.',
    endpointMissing: 'Sending is not configured yet. The backend URL must be added before reports can be sent.',
    submitLabel: 'Send feedback',
    footerNote: 'UNOFFICIAL FAN PROJECT',
    privacyNote: 'Once configured, the backend will forward your information and files to felipsempre10@gmail.com.',
    backToTitle: 'Back to the title ↑',
    tooManyImages: 'Please select no more than 5 images.',
    invalidImage: 'Please select image files only.',
    invalidVideo: 'Please select a video file.',
    sending: 'Sending your feedback…',
    sent: 'Your feedback was sent successfully. Thank you!',
    failed: 'The feedback could not be sent. Please try again later.',
    serverFailed: 'The backend could not accept your feedback (HTTP {status}). Please try again later.'
  }
};

const originalText = new Map();
document.querySelectorAll('[data-i18n]').forEach(element => {
  originalText.set(element, element.textContent);
});

let language = 'pt';
const languageButton = document.getElementById('language');
languageButton.addEventListener('click', () => {
  language = language === 'pt' ? 'en' : 'pt';
  document.documentElement.lang = language === 'pt' ? 'pt-BR' : 'en';
  languageButton.textContent = language === 'pt' ? 'EN' : 'PT';
  languageButton.setAttribute('aria-pressed', String(language === 'en'));
  languageButton.setAttribute('aria-label', language === 'pt' ? 'Mudar idioma para inglês' : 'Switch language to Portuguese');
  document.querySelectorAll('[data-i18n]').forEach(element => {
    const key = element.dataset.i18n;
    element.textContent = language === 'pt' ? originalText.get(element) : (translations.en[key] ?? originalText.get(element));
  });
  if (statusMessage.dataset.messagePt) {
    statusMessage.textContent = language === 'pt' ? statusMessage.dataset.messagePt : statusMessage.dataset.messageEn;
  }
});

const form = document.getElementById('feedback-form');
const statusMessage = document.getElementById('feedback-status');
const submitButton = form.querySelector('[type="submit"]');

function showStatus(portuguese, english, state = 'error') {
  statusMessage.dataset.messagePt = portuguese;
  statusMessage.dataset.messageEn = english;
  statusMessage.textContent = language === 'pt' ? portuguese : english;
  statusMessage.dataset.state = state;
}

form.addEventListener('submit', async event => {
  event.preventDefault();
  const imageFiles = Array.from(form.elements.images.files);
  const videoFile = form.elements.video.files[0];

  if (imageFiles.length > 5) {
    showStatus('Selecione no máximo 5 imagens.', translations.en.tooManyImages);
    return;
  }
  if (imageFiles.some(file => !file.type.startsWith('image/'))) {
    showStatus('Selecione apenas arquivos de imagem.', translations.en.invalidImage);
    return;
  }
  if (videoFile && !videoFile.type.startsWith('video/')) {
    showStatus('Selecione um arquivo de vídeo.', translations.en.invalidVideo);
    return;
  }
  if (!FEEDBACK_ENDPOINT) {
    showStatus(
      'Não foi possível enviar: a URL do backend ainda precisa ser configurada em feedback.js.',
      'Could not send: the backend URL still needs to be configured in feedback.js.'
    );
    return;
  }

  const payload = new FormData();
  payload.append('name', form.elements.name.value.trim());
  payload.append('email', form.elements.email.value.trim());
  payload.append('title', form.elements.title.value.trim());
  payload.append('description', form.elements.description.value.trim());
  imageFiles.forEach(file => payload.append('images', file));
  if (videoFile) payload.append('video', videoFile);

  submitButton.disabled = true;
  showStatus('Enviando seu feedback…', translations.en.sending, 'info');
  try {
    const response = await fetch(FEEDBACK_ENDPOINT, {
      method: 'POST',
      body: payload,
      credentials: 'omit'
    });
    if (!response.ok) {
      showStatus(
        `O backend não aceitou o feedback (HTTP ${response.status}). Tente novamente mais tarde.`,
        translations.en.serverFailed.replace('{status}', response.status)
      );
      return;
    }
    form.reset();
    showStatus('Feedback enviado com sucesso. Obrigado!', translations.en.sent, 'success');
  } catch (error) {
    console.error('Feedback submission failed:', error);
    showStatus(
      'Não foi possível enviar o feedback. Verifique sua conexão e tente novamente.',
      translations.en.failed
    );
  } finally {
    submitButton.disabled = false;
  }
});
