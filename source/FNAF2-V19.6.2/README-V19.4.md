# FNAF 2 DS/DSi — V19.4

## Mudanças solicitadas

1. New Game → assinatura → Apply: removida a espera programada no jornal. Ao aceitar Apply, a Noite 1 entra diretamente em beginDream(1,true), com terminal embaixo e visão pela máscara do Freddy acima. O tempo de carregamento físico dos recursos continua dependendo do cartão/loader; não há espera de 300 frames nesse caminho.
2. Saída do monitor: a mudança monitorTarget=true→false arma a mesma sequência inferior de seis frames noiseRender(...,false) usada na entrada. Retorna ao Security Center depois. O caminho de entrada não foi alterado. camera_state.c (seleção/vizinhos) e camera_render.inc (renderização superior) são idênticos à base disponível; a nova sequência é exclusivamente de officeRenderPanel. Tanto fechamento por tablet quanto KEY_DOWN passam pela mesma detecção de saída.
3. Vitória: as imagens são exatamente os 29 PNGs do ZIP FNAF2_Victory_Assets_DSi(2).zip. Os hashes originais são conferidos pelo gerador. O antigo compositor de vitória foi substituído por victory.c e bancos indexados preparados antes do áudio, para que as duas telas sejam renderizadas sem leituras durante a animação.

## Tempo e comportamento

- 0–4499 ms: 5 AM centralizado; inferior preta.
- 4500–8999 ms: oito passos de rolagem 5→6; inferior preta.
- Aos 9000 ms: 6 AM, SURVIVED!, confetes, personagens A/B e início do fade inferior. O áudio existente usa chime.raw e crowd.raw; crowd é disparado no primeiro update >=9000 ms, em vez dos 10000 ms antigos. A sincronia audível deve ser confirmada no console.
- Aos 9400 ms: caixa NIGHT COMPLETE! plenamente visível.
- Aos 9800 ms: texto NIGHT X SURVIVED plenamente visível.
- Aos 10400 ms: documento nítido e botão TAP TO CONTINUE; botão pisca a cada 500 ms. Novo toque na área inferior, A ou START continua. Toque já mantido desde o gameplay exige soltura antes de funcionar.
- Noites 1–4: caixa, texto e botão, sem cheque. Noite 5: Jeremy Fitzgerald/$100.50. Noite 6: Jeremy/$20.10/OVERTIME. Noite 7: Fritz Smith/NOTICE OF TERMINATION/motivos/FIRED.
- Continuar nas Noites 1–5 leva à próxima noite. Nas Noites 6–7 retorna ao título, preservando o fluxo existente.

## Memória e origem do código

O ZIP disponível da V19.3 RAMFix estava truncado antes da pasta source. Foram recuperadas 546 entradas com CRC verificado. A estrutura de código foi restaurada a partir do último fonte íntegro V19.2 UI-Fix disponível; as correções RAMFix documentadas da V19.3 foram reaplicadas: título, menus, WARNING e jornal em NitroFS; música temporária do título em heap, encerrada e liberada antes de cutscene/escritório. Recursos V19.3 recuperados e ARM7 preservados. Esta build não é uma ROM antiga renomeada.

ARM9 normal text+data+BSS: 1075172 bytes (~1,03 MiB). Bancos de vitória carregados por noite: (9+16+16)*49664 = 2036224 bytes (~1,94 MiB), com liberação ao continuar. Somente o grupo e documento da noite corrente são carregados. Sem todos os grupos simultâneos na RAM. Não há grandes arrays de título compilados no ARM9.

DevkitARM foi instalado em /opt/devkitpro e permanece instalado. Pacotes oficiais verificados por SHA-256 e assinatura PGP, com cópia local em /opt/devkitpro/package-backup. GCC 16.1.0, newlib 4.6, libnds 2.0.2, Calico 1.2.0 e ndstool 2.3.1. Não executar make clean fora do projeto; o clean deste Makefile remove apenas outputs de build.

## Arquivos e testes

- ROM normal: FNAF2-DS-Title-v0.19.4-DSi.nds.
- ROM de diagnóstico: FNAF2-DS-Title-v0.19.4-DSi-LOG.nds. Gera FNAF2-STARTUP.LOG na raiz do cartão.
- VALIDATION.json: SHA-256, CRCs, ARM7, cabeçalhos e seções DSi, trampoline legado e comparação byte a byte de todos os 464 arquivos NitroFS.
- V19.4-Preview.png: prévia decodificada dos bancos realmente incluídos na ROM; não é captura de emulador.

Passaram as 11 suítes existentes e o teste de limites victory_timing_test.c. O teste adicional test_v194_render.py comparou as duas telas com os bancos para as Noites 1–7, testou gate de toque e renderizou com o acesso a NitroFS removido após preload. ASan e UBSan passaram; o detector de leaks do sanitizer foi desativado porque não funciona neste sandbox. Não houve execução em emulador ou teste no DSi real.

Compilação com Pillow (gerador usa get_flattened_data):

```sh
source /etc/profile.d/devkit-env.sh
make BUILD=build-v194-normal
make BUILD=build-v194-log DISABLE_STARTUP_LOG=0
python3 tests/run_tests.py
cc -Wall -Wextra -Werror -Isource tests/victory_timing_test.c -o /tmp/victory_timing_test
/tmp/victory_timing_test
python3 tests/test_v194_render.py
python3 tests/validate_rom.py
```

Teste no DSi: preservar saves existentes; conferir Apply, abrir/fechar monitor sem trocar câmeras e conclusão nas sete noites. Preferir a ROM LOG para o primeiro teste e enviar o log se houver travamento ou falha de carregamento. As duas ROMs contêm os recursos internamente; não é necessário copiar o ZIP de PNGs para o cartão.
