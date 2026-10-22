# FNAF 2 — Victory assets para DS/DSi

Pacote de gráficos de origem: 29 PNGs RGBA, nas dimensões combinadas. As telas de destino são 256 × 192. Imagens com transparência devem ser desenhadas sobre preto. Este pacote não contém ROM, áudio, nem integração compilada com libnds.


## IMPORTANTE: implementar as duas telas

Leia primeiro INTEGRACAO_DUAS_TELAS.md. A tela inferior é obrigatória em todas as noites. Fica preta somente antes de 9000 ms; depois mostra caixa, texto e botão. Noites 5–7 também mostram o documento. Chame o renderizador inferior em cada quadro do estado de vitória, mesmo sem cheque. Os PNGs existem; o ZIP não altera uma ROM automaticamente.

Esta revisão preserva todos os 29 PNGs do pacote anterior e detalha o diagnóstico de tela inferior preta. Aos 10400 ms do áudio, os documentos de 5–7 devem estar totalmente nítidos.

## Tempo e áudio

Inicie o áudio completo de vitória uma vez em t=0. Use o início efetivo da reprodução como referência do contador monotônico.

| Tempo absoluto | Estado | Superior | Inferior |
|---|---|---|---|
| 0–4499 ms | ShowFive | 5 AM imóvel | Preto |
| 4500–8999 ms | RollClock | 5 sobe; 6 entra; AM imóvel | Preto |
| >=9000 ms | Celebrate | 6 AM, letreiro, confetes, A/B | Fade começa em zero |

`celebrationMs = elapsedMs - 9000`. Não inserir CharactersEnter antes dos 9000 ms. Não disparar outro Yay se já estiver no áudio. Precisão de apresentação limitada ao próximo quadro de vídeo; confirmar sincronia no console.

## Layout

| Elemento | x,y | Tamanho |
|---|---|---|
| top/digit_5 ou digit_6 | 64,40 | 32×48 |
| top/am | 96,40 | 96×48 |
| top/survived | 16,8 | 224×32 |
| Quadro de top/confetti | 0,0 | 256×192 |
| Grupo A/B | 0,88 | 256×104 |
| bottom/night_complete | 16,12 | 224×24 |
| bottom/night_X_survived | 16,40 | 224×16 |
| Documento | 16,60 | 224×104 |
| bottom/tap_to_continue | 16,168 | 224×16 |

ShowFive e RollClock: desenhe o numeral e AM em y=72 para manter 5 AM centralizado. Aos 9000 ms, posicione o relógio estável em y=40 para reservar espaço aos personagens. A tabela acima e as prévias mostram a composição da comemoração.

## Folhas de animação

clock_roll.png: 32×384, oito quadros verticais de 32×48. Retângulo do quadro i: (0,i*48,32,48). Quadro 0 = 5; quadro 7 = 6. O numeral fica recortado à janela de 48 px. Índice: min(7,floor(clamp((elapsedMs-4500)/4500,0,1)*8)). A exportação tem oito passos; não equivale a uma rolagem de 60 quadros por segundo. Para rolagem contínua, use digit_5 e digit_6 empilhados, deslocados e recortados na mesma janela.

confetti.png: 256×1536, oito quadros verticais de 256×192. Retângulo do quadro i: (0,i*192,256,192). Índice: floor(celebrationMs/100)%8. O último quadro se conecta ao primeiro por deslocamento modular; repete a cada 800 ms.

## Personagens

Todos os grupos têm 256×104 e poses A/B independentes, sem texto ou confetes incorporados.

| Noite | Pose A / Pose B |
|---|---|
| 1–4 | toys_a / toys_b: Toy Freddy, Toy Bonnie, Toy Chica com cupcake e Mangle |
| 5 | withered_light_a / withered_light_b: Freddy, Bonnie, Chica e Foxy menos destruídos |
| 6 | withered_full_a / withered_full_b: Bonnie, Chica e Foxy destruídos; Freddy ausente; sombra discreta no canto |
| 7 | shadow_golden_a / shadow_golden_b: Shadow Freddy e Golden Freddy miniaturas |

Índice A/B: floor(celebrationMs/300)%2. Não recortar automaticamente cada pose em runtime: preserve o canvas e o ponto de apoio. As poses foram produzidas como sprites originais por geração de imagem e exportadas com amostragem nearest-neighbor; não são sprites extraídos do jogo original.

## Documentos e fade inferior

Noites 1–4: não há cheque; apenas caixa de conclusão, texto da noite e botão.

Noite 5: Jeremy Fitzgerald, $100.50, assinatura padrão Fazbear Entertainment, sem carimbo especial.

Noite 6: Jeremy Fitzgerald, $20.10, carimbo vermelho OVERTIME.

Noite 7: NOTICE OF TERMINATION, Fritz Smith, motivos “Tampering with the animatronics” e “Odor”, carimbo FIRED, sem valor monetário.

| celebrationMs | Revelação |
|---|---|
| 0 | Preto |
| 0–400 | Caixa amarela: 0→100% |
| 300–800 | Texto da noite: 0→100% |
| 500–1400 | Documento: borrado→nítido e escuro→visível |
| >=1400 | Documento nítido; botão alterna a cada 500 ms |

Use as imagens *_blur.png somente no começo da revelação e faça crossfade para as versões nítidas. O documento já contém seus textos e carimbo. Não renderize o mesmo texto outra vez por cima. O botão apagado não desativa sua área de toque.

## Integração e memória

Ative vitória uma única vez no cruzamento lógico para 6 AM. Capture a noite concluída; suspenda IA, ataques e controles do Office. Preserve progressão/save existentes. Ao aceitar continuar, exija toque novo após soltura do anterior e celebrationMs>=1400.

Prepare gráficos antes da transição. PNG é formato de origem, não formato diretamente carregável na VRAM. Converter pelo pipeline do projeto. Grupos de 256×104 não são um único OBJ nativo: dividir em sprites, tiles, BG ou usar composição. Não alocar a folha inteira de confetes na VRAM sem orçamento. Um quadro RGBA 256×192 ocupa 196608 bytes; a folha de oito quadros ocupa 1572864 bytes antes de conversão. Carregar apenas o grupo da noite relevante.

Não há leitura de SD durante o loop A/B. Usar tempo decorrido, nunca quantidade de frames, como relógio. Verificar emulador e DSi real.

## Conferência

manifest.json contém dimensões, modo, SHA-256 e quantidade de quadros. preview.png mostra pares A/B e composição dos documentos. validation.json registra verificação de dimensões, PNG/RGBA/transparência e diferença entre poses. O pacote foi validado como arquivo gráfico; não houve teste de ROM ou áudio no console.
