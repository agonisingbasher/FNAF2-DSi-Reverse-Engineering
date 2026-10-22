# FNAF 2 DS/DSi — V19.5

Build derivada da V19.4, com correções de tutorial, debug, bateria e áudio. A interface e a configuração da Custom Night continuam as anteriores, para trabalho na V19.6.

## 1. Noite 1 — tutorial

- 12–1 AM: todos os níveis de movimento são zero.
- 2–5 AM: somente Toy Bonnie no nível 1. Toy Freddy, Toy Chica, Mangle, Balloon Boy, Withereds, Foxy e Golden Freddy permanecem inativos.
- Toy Bonnie tem oportunidade de movimento a cada 15 segundos, em vez de 5; probabilidade de sucesso de 1/20 por oportunidade. Não é um ataque garantido e não é invulnerabilidade.
- Caixinha da Puppet: 25 unidades/s de uma barra de 100000. Isso é 16 vezes mais lento que a drenagem anterior da Noite 1. Sem dar corda durante os 534 s da noite, resta aproximadamente 86,65%. A recarga continua disponível na CAM11.
- Drenagem da caixinha e tabelas das Noites 2–7 preservadas. Este é um balanceamento de tutorial solicitado para o port, não uma afirmação de reprodução exata do executável original.

## 2. Debug — teclado I / SELECT

O executável NDS recebe botões do DS, não teclas ASCII de um computador. No emulador, configure **I como o botão SELECT** no mapeamento de controles. Não existe uma tecla I física no DSi.

Durante gameplay, com o guarda vivo:

| Comando DS | Ação |
|---|---|
| SELECT sozinho (I no emulador) | Relógio passa imediatamente a 5 AM |
| Segurar L e pressionar SELECT | Reinicia na próxima noite: 1→2→3→4→5→6→7→1 |
| Segurar R e pressionar SELECT | Reinicia na noite anterior, com volta 1→7 |

Para pular de 1 para 3, use L+SELECT duas vezes. Cada mudança inicia a noite escolhida em 12 AM, com bateria/IA/caixinha novas. Depois use SELECT sozinho para testar a passagem 5→6 AM. Solte L/R para usar o salto de horário.

O salto para 5 AM altera só o deslocamento do relógio; não avança 445 segundos na simulação da IA, bateria ou áudio. Após o salto, restam 89 segundos reais até 6 AM. A transição e a comemoração continuam com os tempos da V19.4, incluindo 9000 ms até a festa.

O antigo SELECT de preview visual foi substituído por estes comandos. O indicador DEBUG aparece no HUD. Usar debug marca a sessão de teste: **sem estrelas, desbloqueios ou avanço de save por vitória**, mesmo ao continuar para outra noite. A IA permanece ativa; debug não usa o flag previewUsed, que pausava a IA.

Para voltar a uma sessão normal, saia ao título e escolha New Game/Continue/Extra Night ou inicie a Custom Night pelo fluxo normal. A entrada em Custom Night por debug usa os níveis já existentes no save; a configuração dela não foi alterada nesta versão.

## 3. Bateria e ícones

A carga agora integra milissegundos de luz efetivamente ligada. Cair o FPS não aumenta a autonomia. Drenam: lanterna do corredor, luz dos dois dutos e lanterna da câmera. Apagadas, não drenam. Várias luzes no mesmo intervalo contam como um único intervalo de exposição, preservando a regra anterior.

Mantida a capacidade equivalente anterior a 60 FPS:

| Noite | Autonomia com luz continuamente ligada |
|---|---|
| 1 | ~116,67 s |
| 2 | 100 s |
| 3 | ~83,33 s |
| 4 | ~66,67 s |
| 5–7 | 50 s |

Os valores não são o tempo total da noite: uso em pulsos consome apenas o tempo acumulado aceso.

Ícones: acima de 75% = quatro traços; até 75% = três; até 50% = dois; até 25% = um; zero = vazio. A UI continua mostrando bateria no mapa das câmeras, conforme o layout do projeto. Ao voltar ao mapa, exibe a carga atual. Ao zerar, todas as luzes se desligam; não há carga negativa.

Na Noite 1, manter luz ligada por 30 s deixa 74,286% de carga: o primeiro traço já deve ter desaparecido. Esse resultado foi igual em testes de 1 a 120 FPS.

## 4. Som do corredor

hall.raw foi comparado byte a byte com a conversão existente de **With_S2 (1).wav** (mono, PCM8, 8000 Hz, loudnorm I=-20/TP=-2/LRA=7). SHA-256: 1249f4c609b49a2f99f82d0f8b2d5e5586db60af5f00687f007a73f0c0e3074e.

O áudio começa assim que qualquer localização da IA é HALL, independentemente da lanterna, máscara ou monitor. Toca como sample completo em loop de hardware, sem reiniciar a cada quadro e sem padding de blocos de streaming entre repetições. Continua se um personagem sair mas outro permanecer. Para quando o último sai, ao morrer, concluir a noite ou deixar gameplay.

O sample de 48500 bytes é preparado ao entrar na noite e liberado ao sair. A confirmação auditiva no DSi real ainda é necessária.

## 5. Testes e compilação

13 suítes nativas passaram: state, office, corrections, v14, panel, call_stream, camera, ai, v19, cutscene, contract, victory_timing, v195. O teste de renderização das duas telas também passou para as Noites 1–7, com ASan/UBSan (detector de leaks desativado por limitação do sandbox).

v195_test.c verifica:
- Drenagem igual em 1–120 FPS, luz de câmera e dutos, idle e bateria vazia.
- Limites de todos os ícones.
- Salto para 5 AM e conclusão após 89 s, sem mexer na bateria/preview.
- Ciclo válido de noites 1–7.
- Caixinha da Noite 1 em diferentes passos de tempo e níveis das Noites 2–7.
- Áudio permitido no corredor escuro, com máscara/monitor, vários ocupantes e saída do último; desativado ao morrer/vencer.

VALIDATION.json verifica as duas ROMs: CRCs, seções DSi, ARM7 original, entrada legada e todos os arquivos NitroFS. O finalizador usa a reserva explícita rom_alignment_padding via endereço físico LOAD do ELF, sem depender de sobra aleatória entre ARM9 e ARM7. Load lists e tamanho do ARM9 são preservados.

Não houve execução em emulador ou hardware real nesta entrega. Os testes nativos validam lógica e bancos gráficos, não a reprodução física de áudio, o mapeamento do teclado do seu emulador ou a interação no touch.

```sh
# Com DevkitARM existente na máquina, exportar DEVKITPRO/DEVKITARM e PATH.
make BUILD=build-v195-normal
make BUILD=build-v195-log DISABLE_STARTUP_LOG=0
python3 tests/run_tests.py
python3 tests/test_v194_render.py
python3 tests/validate_rom.py
```

O nome test_v194_render.py foi mantido porque verifica o renderizador de vitória preservado da V19.4.

## Teste recomendado no DSi/emulador

1. Preservar os saves. Usar a versão LOG no primeiro teste; ela grava FNAF2-STARTUP.LOG.
2. Noite 1: observar palco/câmeras até 2–3 AM e a lenta caixinha.
3. Segurar luz por 30 s e abrir o mapa: confirmar queda de um traço na N1.
4. SELECT sozinho: ver 5 AM imediatamente; aguardar 89 s e confirmar ambas as telas de vitória.
5. L+SELECT/R+SELECT: testar Noites 3–7. DEBUG deve aparecer e vitórias não devem conceder progressão.
6. Encontrar alguém no corredor em noite posterior: som continua ao apagar a luz e termina ao esvaziar o corredor.

ROMs possuem todos os recursos internamente; não é necessário extrair PNGs no cartão. O DevkitARM desta sessão foi instalado em uma pasta dentro do workspace, com pacotes oficiais verificados e preservados, para poder ser reutilizado sem apagá-lo ao limpar builds.
