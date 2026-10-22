# V19.6.2 — somente loading e reação dos Withereds
Base: V19.6.1, preservando Help Wanted, assinatura inferior com L, dez personagens no nível 20, Custom Night e separação DEV/NORMAL.

Loading inferior: após inicialização da semente, sorteia row em 0..2 e offsetX em 0..863 antes do primeiro desenho. Mantém a velocidade, sentido e continuidade anteriores. Sorteios podem se repetir legitimamente.

Withered Freddy, Bonnie e Chica: janela mínima de 1500ms para máscara completa (Noite 1 mantém 1667ms). O runtime aguarda confirmação de apresentação do quadro do escritório antes de iniciar o prazo; a leitura de assets e o fechamento do monitor não consomem a janela. Toy Freddy, Toys, Foxy e Golden Freddy mantêm suas regras anteriores. Após responder corretamente, continua sendo necessário manter a máscara até a retirada.

18 suítes passaram. Novos cenários para cada um dos três Withereds verificam espera de renderização lenta, defesa antes do limite, retirada e morte quando o prazo termina sem máscara.
ROMs DEV/NORMAL compiladas; CRC, ARM7, seções DSi e 462 arquivos NitroFS validados. Não testadas em DSi/emulador nesta entrega.
