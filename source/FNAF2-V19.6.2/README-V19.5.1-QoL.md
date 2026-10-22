# FNAF2 DS/DSi — V19.5.1 QoL

Correção da V19.5 dedicada às duas interfaces inferiores. A reformulação da
configuração visual de Custom Night continua reservada para a V19.6.

## Arquivos para jogar
- `FNAF2-DS-Title-v0.19.5.1-DSi.nds`: build normal, sem log de arranque.
- `FNAF2-DS-Title-v0.19.5.1-DSi-LOG.nds`: mesma lógica, com diagnóstico.
Use a versão normal primeiro. Os saves existentes são preservados; não os apague.

## Vitória: todas as noites 1–7
A tela superior e seus bancos gráficos são idênticos byte a byte à V19.5.
5 AM parado por 4500 ms; rolagem por mais 4500 ms; comemoração/voz e início
do fade inferior aos 9000 ms. O fade termina aos 10400 ms, liberando as ações.
A tela inferior exibe NIGHT COMPLETE!, NIGHT X SURVIVED e os documentos de
N5/N6/N7. Os documentos foram compostos em 224×86 para acomodar os controles.
Os valores, nomes e carimbos permanecem os mesmos dos PNGs fornecidos.

Dois botões permanentes, lado a lado, em TODAS as sete noites:
- Esquerdo ciano: [ TAP CONTINUE ].
- Direito cinza: [ GAME MENU ].
Ambos medem 116×36 pixels. Não desaparecem com o antigo piscar do texto.
N1–5: continuar avança para a introdução da noite seguinte.
N6: continuar abre a configuração de Custom Night existente (N7).
N7: continuar repete a Custom Night com os mesmos níveis e opções.
GAME MENU sempre retorna à tela inicial; nunca existe uma Noite 8.

A vitória legítima é registrada/salva uma vez, ao terminar a noite, antes dos
botões. O retorno ao menu não altera nem apaga o progresso. Noites de debug,
prévia e mortes mantêm as regras anteriores: não concedem prêmios/progresso.
Uma falha real de escrita do save é registrada na build LOG; não se promete
salvamento quando o cartão/ambiente não permite gravar.

## Game Over
A arte superior da máscara e sua lógica visual foram preservadas.
A tela inferior usa fundo preto, GAME OVER vermelho grande e a frase completa:
“Fazbear Entertainment is not responsible for death or dismemberment.”
O texto dos PNGs usa maiúsculas para legibilidade. A composição segue a imagem
JPG enviada: título amplo, frase menor e botões grandes lado a lado.
- Esquerdo ciano: [ RETRY NIGHT ].
- Direito cinza: [ GAME MENU ].
O Game Over permanece aberto até a escolha, sem retorno automático após 4 s.
RETRY NIGHT fecha o áudio anterior e reinicializa diretamente a mesma noite,
sem jornal, inscrição, cutscene ou tela de introdução extra. A leitura física
necessária dos recursos da gameplay continua ocorrendo durante a inicialização.
A noite, configuração da IA, cheats e marca de debug são mantidos.
GAME MENU fecha os sons anteriores e restaura o menu e sua música.

## Controles e segurança de toque
Nos dois menus finais: A/START ativa o botão esquerdo; B ativa GAME MENU.
Hitboxes DS (origem no canto superior esquerdo, limites finais exclusivos):
- Primário: x=[8,124), y=[150,186).
- Menu: x=[132,248), y=[150,186).
O toque precisa ser solto depois de entrar na tela. Toques mantidos na gameplay
não acionam os botões. Toques fora dos retângulos não executam ações.
Na vitória, tocar antes dos 10400 ms não antecipa o relógio/fade.

## Integração e assets
`reference_assets/ui_update/` inclui os quatro PNGs originais, ZIP intacto e JPG
de referência. Os textos dos botões foram extraídos dos PNGs, sem comprimir a
fonte, e montados em molduras grandes com as cores da referência. O título e a
frase do Game Over também usam os pixels do PNG fornecido.
As prévias 256×192 efetivamente usadas estão em `previews/`.
Os adapters C++ do ZIP são referência; não fazem parte do motor C compilado.
A integração real está em `source/main.c` e `source/end_ui.c/.h`.
`assets/build_qol_ui.py` pré-compõe o Game Over e os controles.
`assets/build_v194_victory.py` monta os bancos inferiores das sete noites.
PNG/fontes não são decodificados em tempo real; a vitória continua pré-carregada,
sem leitura de arquivos durante a animação. Game Over é carregado uma vez.
Em falha de carregamento, existem dois controles textuais funcionais nas mesmas
hitboxes, em vez de uma tela sem saída.
Nenhuma mudança foi feita no balanço, bateria, câmeras ou áudio do corredor.

## Compilação e validação
Use o toolchain devkitARM/libnds já adotado na V19.5:
```
export DEVKITPRO=/caminho/para/devkitpro
export DEVKITARM="$DEVKITPRO/devkitARM"
export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
make BUILD=build-qol
make BUILD=build-qol-log DISABLE_STARTUP_LOG=0
python3 tests/run_tests.py
python3 tests/test_v194_render.py
python3 tests/validate_rom.py
```
Pillow é usado somente na preparação dos gráficos.
14 suítes de testes passaram, incluindo bordas das hitboxes, trava de toque,
timing, estado e balanceamento anteriores. Renderer das sete noites validado
com ASan/UBSan e comparação dos dois framebuffers aos bancos. ROMs normal/LOG
validadas: CRC, seções DSi, ARM7 preservado, entrada e 465 arquivos NitroFS.
Arte superior da vitória verificada byte a byte contra a build V19.5.
`VALIDATION.json` inclui hashes e resultados. `QOL_ASSET_AUDIT.json` registra
os PNGs originais e os bancos superiores preservados.

Ainda não executado em DSi físico ou emulador nesta sessão. Verificação manual:
1. Concluir cada noite e confirmar ambos os botões após o fade.
2. N5/N6/N7: conferir documentos, valores e carimbos.
3. GAME MENU após vitória legítima: reabrir o jogo e confirmar o save.
4. Morrer com toque mantido: nenhuma ação até soltar e tocar novamente.
5. RETRY NIGHT: mesma noite diretamente e sem sons da tela anterior.
6. GAME MENU no Game Over: menu normal e sem alteração do progresso.
7. Repetir N7: continuar usa a mesma configuração, sem inventar N8.
