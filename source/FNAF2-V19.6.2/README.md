# FNAF2 DS/DSi — V19.5_Fixed

Release interno 0.19.5.3. Esta é a revisão da V19.5.2 CRT com a tela inferior
industrial, botões controlados pelo código e variação do ambiente ao fim da faixa.

## Arquivos
- `FNAF2-DS-Title-v0.19.5_Fixed-DSi.nds`: ROM normal.
- `FNAF2-DS-Title-v0.19.5_Fixed-DSi-LOG.nds`: mesma lógica, com diagnóstico.
- `source/`, `assets/`, `tests/`, `nitrofiles/`: projeto compilável completo.
- `reference_assets/victory_industrial/victory_sheet_original.png`: anexo intacto.
- `reference_assets/In_The_Depths_C.ogg`: áudio original intacto.
- `previews/victory_N_ESTADO.png`: saída real do renderer C, 256×192.
  N=1..7; ESTADO=0 Idle, 1 Continue selecionado, 2 Menu selecionado.
- `VALIDATION.json`: hashes das duas ROMs e verificações.
Este README substitui as instruções das versões anteriores; outros READMEs são
histórico. Não apague saves existentes. O devkitARM foi preservado.

## Vitória: interface inferior industrial em TODAS as noites
A interface genérica preta foi removida do caminho de execução E da ROM.
Os sete bancos antigos `v194_bottom_n*.bin` não são mais empacotados/carregados.
Uma textura 2×2, 512×384, com paleta RGB555 e pixels index8, substitui os bancos.
O arquivo de runtime ocupa 197136 bytes; só esta textura inferior é carregada.
A tela superior, seus bancos de relógio e poses A/B permanecem idênticos.

Mapeamento, linhas e colunas iniciadas em 1:
| Noite concluída | Quadro | Texto dinâmico |
| --- | --- | --- |
| 1–4 | L1C1 | NIGHT X SURVIVED no cabeçalho; NIGHT: X no papel |
| 5 | L1C2 | Não; mantém o cheque do PNG |
| 6 | L2C1 | Não; mantém o cheque OVERTIME do PNG |
| 7 | L2C2 | Não; mantém a demissão do PNG |

Nas noites 1–4, o renderer apaga só o algarismo pré-pintado e escreve a noite
concluída nas duas posições. No papel, recupera o fundo a partir de uma área
vizinha da mesma textura, evitando um retângulo preto. Não muda os demais textos.
Os documentos N5–7 são copiados da imagem, sem gerar textos/valores novos.
Observação de fidelidade: o novo PNG contém $101.05 no cheque N6, diferente dos
$20.10 definidos anteriormente. Foi preservado como o usuário pediu nesta
revisão (“não requerem alteração de texto dinâmico”). O documento N5 tem $100.50.

Timing preservado: 5 AM parado 0–4499 ms; rolagem 4500–8999 ms; celebração,
confetes/poses e fade industrial começam aos 9000 ms. O fade chega a 100% aos
10400 ms e então libera os controles. Até lá, nenhum toque antecipa a vitória.

## Botões unificados: vitória e Game Over
A arte pré-pintada deixa de ser a autoridade sobre a cor dos controles.
`source/touch_ui.c` desenha bordas e textos em cima da textura com estes estados:
- Idle: ambos os botões CINZA.
- Pressionado: somente o botão sob a stylus fica AZUL/ciano.
- Arraste: muda a seleção ao atravessar os botões; sair de ambos cancela.
- Confirmação: ao soltar a stylus, mantém o botão azul por 200 ms.
Depois desses 200 ms a vitória executa a ação. Game Over inicia a sequência CRT.
O renderer repinta toda a face dos botões; portanto, o ciano embutido em qualquer
quadro original nunca aparece sozinho durante Idle. O restante da arte industrial
permanece visível. A cor do outro botão continua cinza.

Hitboxes compartilhadas (origem no canto superior esquerdo, fim exclusivo):
- Botão esquerdo: x=[8,126), y=[150,190).
- GAME MENU: x=[132,250), y=[150,190).
É necessário soltar o toque herdado da gameplay antes de iniciar nova escolha.
A/START escolhe a ação esquerda; B escolhe MENU, com o mesmo feedback de 200 ms.
Entradas ficam bloqueadas durante a confirmação/transição; SELECT não a pula.

Na vitória, o botão esquerdo é TAP CONTINUE:
- N1–5: próxima noite, com a introdução existente.
- N6 E N7: abre a seleção existente da Custom Night.
GAME MENU sempre retorna à tela inicial, sem apagar progresso. Vitórias legítimas
continuam sendo salvas uma única vez antes dos botões; debug/mortes não concedem
prêmios. Nenhuma Noite 8 é criada. A nova UI/configuração Custom Night permanece
fora deste escopo; reutilizamos a tela de seleção existente.

## Game Over: Idle corrigido
O loop usa exclusivamente IDs 0,1,2 (L1C1, L1C2, L1C3), agora por 400 ms cada.
Um ciclo completo dura 1200 ms. L2C1/L2C2 não entram no array/cálculo de Idle.
A análise encontrou também uma causa no próprio PNG: L1C2 já continha RETRY
ciano. Por isso, limitar IDs sozinho não resolvia o “frame intruso” visual;
o overlay compartilhado agora força os dois controles a cinza em TODOS os três
quadros Idle. Os testes verificam isso pixel a pixel na área dos botões.

Seleção: RETRY=L2C1 (ID 3), MENU=L2C2 (ID 4), com overlay azul apenas no escolhido.
Mantém-se a sequência previamente solicitada após soltar +200 ms:
L3C1 → L3C2 → L2C3 → L3C3 → preto, 90 ms por etapa, inclusive o preto.
Só depois do preto acontece o reinício da mesma noite ou o retorno ao menu.
Retry preserva a noite e configuração e não passa por jornal/cutscene/intro.
A arte superior da máscara e a lógica superior anterior permanecem.
A mesma textura 3×3 é carregada uma vez (442896 bytes). Não há leitura/alocação
por frame. As partes da animação fora dos botões conservam o conteúdo das células
do PNG original; esta revisão não reorganiza/redesenha os quadros de desligamento.

## Ambiente: repetir OU trocar no fim da faixa
A faixa enviada dura aproximadamente 143,397 segundos. Foi convertida completa,
sem cortes, em mono PCM signed 8-bit a 8000 Hz (`ambience_depths.raw`, 1147180 bytes,
incluindo alinhamento de até 3 bytes de silêncio).
A faixa ambiente antiga permanece a 4000 Hz, sua taxa original de reprodução.
A gameplay começa com o ambiente antigo. Quando a faixa atual realmente chega
ao EOF, um PRNG independente faz UMA escolha:
- 50%: repetir a faixa atual.
- 50%: trocar para a outra.
Essa regra funciona nos dois sentidos, incluindo repetir a faixa nova.
Não troca ao abrir câmeras, acender lanterna, mudar de hora ou preencher o buffer.
Cutscenes, Game Over, chime/voz da vitória e música de título não foram substituídos.

A reprodução usa o mesmo ring buffer de 16384 bytes, sem carregar os 1,1 MB da
faixa nova na RAM. Uma atualização normal não toma decisões nem reinicia o canal.
Underrun recupera a posição da MESMA faixa, sem sorteio. Se o recurso opcional
novo estiver ausente/ilegível, retorna ao ambiente antigo. Fechar a gameplay
fecha a stream e interrompe o canal, evitando sons sobrepostos nos menus.

## Código principal
- `source/victory.c`: recorte industrial, noite dinâmica e fade.
- `source/victory_interaction.c`: toque/soltura/feedback antes da ação.
- `source/touch_ui.c` e `touch_labels.h`: hitboxes/cores/textos compartilhados.
- `source/gameover_state.c`: Idle de 400 ms, seleção e estados CRT.
- `source/gameover_render.c`: textura única com overlay dos controles.
- `source/ambience_state.c`: sorteio de permanência/troca.
- `source/camera_audio.inc`: streaming e decisão somente no EOF.
- `source/main.c`: cenas, Night 7 → seleção e integração das telas.
O motor continua em C/libnds.

## Compilar e testar
Use o devkitARM/libnds já instalado. Não o remova.
```
export DEVKITPRO=/caminho/devkitpro
export DEVKITARM="$DEVKITPRO/devkitARM"
export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
make BUILD=build-fixed
make BUILD=build-fixed-log DISABLE_STARTUP_LOG=0
python3 tests/run_tests.py
python3 tests/test_fixed_render.py
python3 tests/test_gameover_render.py
python3 tests/test_ambience_runtime.py
python3 tests/validate_rom.py
```
Pillow é usado na preparação dos gráficos. A PCM convertida está incluída;
para recriá-la, execute ffmpeg `-i In_The_Depths_C.ogg -ac 1 -ar 8000 -f s8` e
alinhe o tamanho a múltiplo de quatro com até três bytes zero no final.

## Validação desta release
17 suítes C passaram. O renderer real foi testado com ASan/UBSan nas sete noites,
textos numéricos, documentos por célula, fade, estados dos dois botões e os três
quadros Idle. A parte superior foi comparada aos bancos anteriores, sem mudanças.
O renderer de recorte 3×3 também passou nos testes de arquivo inválido, recortes,
cache e preto. Leitura do caminho Nitro foi removida durante a reprodução de teste,
confirmando que a animação não depende de novas leituras do sheet.

O teste de áudio compilou as funções reais de streaming com hardware simulado
e os dois arquivos PCM reais. Verificou 20 finais de faixa: 10 repetições e
10 trocas com a semente de teste; não é uma ordem fixa no jogo. Verificou também
recuperação de underrun sem sorteio e fallback por variante ausente. ASan/UBSan
passaram; o leak scanner não foi utilizado nesta sessão.
ROMs normal/LOG verificadas: CRCs, seções DSi, ARM7 preservado, entrypoint e
comparação integral dos 460 arquivos NitroFS. Hashes em VALIDATION.json.
Ainda não testado em DSi físico nem emulador nesta sessão.

Validação manual recomendada no console: concluir N1/N3/N4 (dois números iguais),
conferir N5/6/7, pressionar/arrastar/cancelar cada controle, conferir que N7 abre
Custom Night, morrer e deixar o Idle rodar sem azul, confirmar ambas as ações de
Game Over e ouvir mais de um fim de faixa ambiente durante gameplay.
