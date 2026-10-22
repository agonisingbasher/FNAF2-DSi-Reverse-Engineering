# Auditoria das versões arquivadas — FNAF 2 DSi

Inspecionadas 12 ROMs distintas do arquivo `Fnaf Versões.zip` (somente builds sem sufixo `-LOG`).

**Limitação:** binários `.nds` não são equivalentes a um histórico Git. As descrições indicam vestígios identificáveis e contexto, não um changelog exato e comprovado por código-fonte.

| Versão | Tamanho (MiB) | Evidência / interpretação |
|---|---:|---|
| V7 | 1.00 | Protótipo do título: Sequência de aviso e menu em teste; mensagens de diagnóstico presentes no binário. |
| V9 | 0.94 | Menu inicial: Estrutura de título e seleção de Night 1/Custom Night identificável. |
| V11 | 2.44 | Expansão do menu: Mais conteúdo embarcado e referências nominais a animatrônicos. |
| V12 | 5.27 | Escritório e máscaras: Referências a cenários do escritório, frames de máscara e feed de câmera. |
| V13 | 5.54 | Áudio e estados de término: Registros de carregamento de áudio, Game Over e Night Complete. |
| V14 | 5.54 | Lógica e depuração: Diagnósticos de relógio/tempo e referências a componentes do escritório. |
| V15 | 10.51 | Ampliação dos recursos: Aumento significativo do binário e novos dados de chamada noturna; conteúdo exato requer código-fonte. |
| V16 | 22.04 | Câmeras e controles: Diagnósticos de câmera aberta, seleção e disponibilidade de Music Box. |
| V17.1 | 22.67 | Ajustes incrementais: Ampliação do conteúdo embarcado; alterações específicas não determináveis só pela ROM. |
| V18 | 25.60 | Introduções e cutscenes: Referências a night_intro, dream_mask e dream_puppet; marco relatado como estável. |
| V18.1 | 25.65 | Correção de inicialização: Build BootFix nomeada; referências adicionais a câmera estática. |
| V19.0 | 25.17 | Tela de vitória: Textos NIGHT SURVIVED e TAP CONTINUE presentes no binário. |

## Integridade: arquivos, bytes e SHA-256

| ROM | Bytes | SHA-256 |
|---|---:|---|
| `FNAF2-DS-Title-v0.7-HB.nds` | 1046080 | `e259526de5c8ea8cc5a299f5c2c7df254eabdfaa310340f1ace8d54ffddc4d83` |
| `FNAF2-DS-Title-v0.9-DSi.nds` | 985088 | `63470d8ea23c34a903ce9cdac9e724baad0c70be216a4b4955ba6d1d7ff18941` |
| `FNAF2-DS-Title-v0.11-DSi.nds` | 2560000 | `a36797397d0d34f10729fab13b5829fa952cdfec01caaee64b792a7e3d9c311f` |
| `FNAF2-DS-Title-v0.12-DSi.nds` | 5527552 | `76abc1929101231bc916fe8694380f80a685dcb150b16d23295f6d83b988ed7c` |
| `FNAF2-DS-Title-v0.13-DSi.nds` | 5804032 | `8b189622b98c7c3aa9c8c1bce400c4cb83f9a401556852ae3937915317c5f25b` |
| `FNAF2-DS-Title-v0.14-DSi.nds` | 5805056 | `179f3f04d7413af6254ad9bc53095f099f861e848cfe8bb9af4b36786b86e1cd` |
| `FNAF2-DS-Title-v0.15-DSi.nds` | 11018240 | `b7da9e7892a1d0bc2c592d834057f530cf36abced8b1f8ae6767ba1f90ea01ca` |
| `FNAF2-DS-Title-v0.16-DSi.nds` | 23113728 | `6ad41ad7fe04fe686f0545a1af338725a4412d279b93c9cc77aa143203203897` |
| `FNAF2-DS-Title-v0.17.1-DSi.nds` | 23776256 | `0885093307b401ed32fe002069bdadcdf5aac8bff9d2acdcccdd7bd17f9b26f5` |
| `FNAF2-DS-Title-v0.18-DSi.nds` | 26839040 | `749b6e2ffed9374db811f7285e7dfd1c4ae3cf9e529d13ad06bd598001a4e0a4` |
| `FNAF2-DS-Title-v0.18.1-BootFix-DSi.nds` | 26895360 | `a275fdd8809e0a91e4143710e7cb8240601272df38ff964fe5d078c8e9f9d903` |
| `FNAF2-DS-Title-v0.19.0-DSi.nds` | 26391552 | `d11ce1170262e165050132bd286dbeb0daa47ded450223858d177f3493a489e4` |

## Critérios

- Inspeção não executou as ROMs em hardware ou emulador.
- Strings presentes no arquivo executável são indícios de componentes programados, não prova de acesso funcional.
- A alteração de tamanho pode resultar de inclusão, remoção, compressão ou realocação de assets. Não permite identificar alterações exatas.
- As builds `-LOG` são variantes da mesma numeração e não foram analisadas como releases distintos.
- As versões V19.1 a V19.7 não foram fornecidas no ZIP histórico.
- Para um changelog preciso por implementação são necessários código-fonte, commits ou documentação das builds.

## Correspondência dos arquivos atuais do repositório

O workspace também contém `Rom/FNAF2-V19.7.nds` e o pacote `Rom/FNAF2-V19.6.2-Codigo-Fonte.zip`, cujo Makefile e `VALIDATION.json` identificam a fonte como V19.6.2. Não foi encontrada documentação que demonstre equivalência entre essa fonte e a ROM V19.7. Portanto: **ROM publicada: V19.7. Fonte disponibilizada: V19.6.2.** A diferença de rótulo não foi eliminada por renomeação. Os 18 testes, 462 arquivos NitroFS e verificações de integridade constantes no relatório pertencem às builds NORMAL/DEV V19.6.2, não à ROM V19.7.
