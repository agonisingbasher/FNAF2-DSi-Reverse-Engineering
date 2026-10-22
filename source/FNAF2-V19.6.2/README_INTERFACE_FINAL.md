# V19.5 Fixed — integração das três imagens finais
Implementação sobre a base Fixed recuperada, sem substituir o projeto pelo código de uma versão anterior.

## Loading
source/loading.cpp é C++17, chamado pelo motor C/libnds através de loading.h (extern C).
loadingBegin(topVRAM,bottomVRAM) escolhe rand()%4 uma vez por transição.
Pares: 0/1 Helpy, 2/3 Toy Bonnie, 4/5 Toy Chica, 6/7 Freddy.
Cada pose dura 300 ms. loadingPump atualiza a 30 Hz, com relógio monotônico Calico.
Efeitos da linha 3 alternam a cada 2400 ms: confetes deslocados, brilho pulsante, CRT deslocado e halo pulsante. São compostos antes do personagem.
O PNG é opaco: o conversor remove por flood fill apenas o fundo escuro conectado de cada painel. Isso gera índice transparente, sem redesenhar os personagens; contornos escuros conectados podem exigir ajuste artístico futuro.
A estampa inferior tem três linhas de 864x192. Translação X positiva a 90 pixels/s: imagem move para a direita. A borda seguinte entra sem faixa preta, na ordem linha1 -> linha2 -> linha3 -> linha1, sem espelhamento dos ícones.
loadingRead divide leituras em blocos de até 8 KiB e chama loadingPump entre blocos. Integração em officeOpen e officeCutsceneBegin via enterOffice/beginDream.
loadingEnd libera ambos os atlas assim que o carregamento termina, sem duração mínima artificial.
Para outra transição: begin, carregue via loadingRead (ou pump entre tarefas CPU), end, desenhe a cena pronta. Operações indivisíveis externas não podem ser animadas durante seu bloqueio; a leitura inicial dos próprios atlas é síncrona.
Falha de atlas libera a memória e deixa o carregamento normal prosseguir.

## Game Over
Atlas 1280x768, vinte células de 256x192, paleta RGB555/index8.
Entrada: L1C1..5, 120 ms cada, toque bloqueado.
Idle: L2C1..5, 400 ms cada.
Faixa dos botões (y150..191): cópia parcial da L4C1, L4C2 (Retry) ou L4C3 (Menu), sobre o fundo. Nunca desenha L4C4/5.
Pressionar destaca; arrastar fora cancela; soltar confirma. Mantém seleção 200 ms.
Desligamento: L3C1..5, 90 ms cada, último quadro preto por 90 ms antes de trocar a cena. Inputs bloqueados e ação emitida uma única vez.
Retry mantém a noite; Menu retorna ao menu. Vitória mantém seu próprio timing sem herdar a nova entrada de Game Over.
Nenhuma alocação ou leitura de arquivo por quadro do Game Over.

## Memória
Loading: 590352 + 498192 = 1088544 bytes em atlas, liberados ao concluir.
Game Over: 983568 bytes em um atlas, liberados antes do Retry/Menu.
Source Rect é amostragem de atlas em software para o framebuffer 256x192; não depende de texturas 3D acima dos limites do hardware.
Os PNGs originais ficam em reference_assets/interface_final. Runtime lê apenas binários compactados por paleta.

## Verificação desta revisão
17 suítes C passaram, incluindo entrada/idle/seleção/saída nova e regressão da interação de vitória.
Compilação ARM9 C/C++ e link da ROM concluídos com devkitARM/libnds; finalização DSi preservada.
Não foi validado visualmente em emulador ou DSi real. Verificar fluidez, máscara de fundo e pico total de RAM no hardware.
Os scripts antigos test_fixed_render.py e test_gameover_render.py ainda descrevem a textura 3x3 anterior e não são validação desta revisão; consultar os novos testes de estados e conferir a arte no dispositivo.
README.md e previews antigos são históricos da base; esta nota prevalece para loading e Game Over.
