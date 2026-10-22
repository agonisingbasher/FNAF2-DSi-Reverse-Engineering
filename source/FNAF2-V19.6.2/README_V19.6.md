# FNAF2 DSi — V19.6.1 — Custom Night e acessibilidade
Base: V19.5 Fixed + Interface Final + Loading Hotfix (thread VBlank preservada).
Esta nota prevalece sobre os READMEs históricos.

## ROM NORMAL
Jogo regular, sem aba L+R e sem os antigos atalhos SELECT de teste.
Custom Night continua com desbloqueio pelo progresso normal.
Cheats ON impede mortes pela IA; não concede energia ou caixinha infinitas.
Correção: New Game agora preserva Cheats ON/OFF ao reiniciar o progresso.
Partidas com Cheats ON não concedem estrelas, desbloqueios ou avanço salvo.

## ROM DEV
Segure L+R durante gameplay ativa para abrir a aba de testes.
Solte os dois antes de escolher uma ação.
D-Pad esquerda/direita: noite 1..7, circular.
D-Pad cima/baixo: campo de noite / carregar noite / ver comemoração / voltar.
A confirma, B volta. Toque nos botões também funciona.
X abre a Custom Night para teste sem desbloqueio.
VER COMEMORACAO mostra a vitória da noite escolhida, já na fase de celebração (9000ms); não é uma vitória legítima.
CARREGAR NOITE reinicia a noite selecionada sem contrato/cutscene.
O relógio/IA ficam pausados na aba; o painel da gameplay é redesenhado ao retornar.
Nenhuma partida da ROM DEV concede recompensas.
Save separado: saves/FNAF2-DEV-PROGRESS.dat e .bak; cutscenes usam FNAF2-DEV-DREAM-SEEN.dat.
A ROM NORMAL mantém seus nomes de save anteriores.

## Contrato / New Game (ambas ROMs)
Jornal Help Wanted original na tela superior; assinatura e Apply/Clear na inferior.
L desenha uma assinatura padrão válida e aplica imediatamente.
A indicação L aparece apenas na tela inferior, preservando o jornal. O caminho de assinatura por toque continua funcional.
O atalho segue para a cutscene da Noite 1 pelo fluxo já existente; não pula a cutscene.

## Custom Night (ambas ROMs)
Topo: grade 5x2 com dez retratos derivados das texturas que já estavam no jogo, cursor laranja, nome e dica.
Nomes abreviados na grade para caberem em 256x192.
Inferior: seletor de preset, AI 00..20, escala de dificuldade, READY verde e OPTIONS azul.
D-Pad: move entre personagens. A +1, B -1. Limites 0 e 20.
X alterna foco entre personagens e controles inferiores.
No foco inferior, cima/baixo escolhem controle; esquerda/direita ajustam; A ativa.
START inicia a Noite 7. Não concede prêmio apenas por iniciar 20/20.
OPTIONS mostra comandos e estado dos cheats; A/B voltam, Y retorna ao menu principal.
Toque: setas do preset/dificuldade, READY, OPTIONS e botões de retorno.
A tela superior do DSi não é tátil.
Presets locais implementados:
- CUSTOM: mantém ajustes manuais.
- CUPCAKE CHALLENGE: todos 5.
- NEW AND SHINY: cinco Toys/Mangle/BB 10, demais 0.
- 20 20 20 20: todos os dez animatrônicos 20, conforme solicitado.
- GOLDEN FREDDY: todos 20.
A edição manual retorna o nome para CUSTOM.
Esta entrega implementa a interface no jogo; não inclui fotografia/mockup do console em mãos.

## Compilar
Preservar SDK devkitARM/libnds/Calico instalado.
make BUILD=build-normal DEV_BUILD=0
make BUILD=build-dev DEV_BUILD=1
Usar pastas de build distintas evita misturar objetos DEV/NORMAL.
Fontes C do motor preservados; loading continua C++17.

## Verificação
18 suítes C passaram, incluindo teste novo de navegação 5x2, limites, presets, cheats impedindo morte e preservação de Cheats ON no New Game.
Renderização real da Custom Night verificada em host com AddressSanitizer/UBSan; prévia em previews/custom_night_v196.png.
NORMAL e DEV compiladas, ARM7/CRC/banner/seções DSi/NitroFS validados.
Validador confirma que texto do menu DEV e prefixo do save DEV não existem na ROM NORMAL.
Não testado em hardware/emulador nesta entrega. Validar L no contrato, A/B/X e L+R no DSi.
