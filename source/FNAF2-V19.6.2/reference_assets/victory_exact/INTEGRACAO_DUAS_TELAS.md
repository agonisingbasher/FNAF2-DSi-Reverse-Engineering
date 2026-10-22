# Integração obrigatória das DUAS telas — conclusão às 6 AM

Este guia complementa README.md. A pasta assets/victory/bottom/ contém os gráficos da tela de toque: ela NÃO deve ficar preta durante toda a comemoração. Importar apenas top/ é uma integração incompleta.

O pacote fornece imagens e especificações. Não contém a correção de uma ROM. Sem examinar o código atual, não é possível atribuir o problema a uma linha específica ou confirmar a causa.

## 1. Estado e início

No cruzamento do contador lógico para 6 AM, entre uma única vez no estado de vitória. Capture a noite concluída antes de avançar o save. Suspenda IA, ataques, consumo e controles do Office. Prepare previamente os recursos das duas telas e o áudio, evitando leituras do SD durante a sequência.

Inicie o áudio completo uma vez. t=0 corresponde ao início efetivo da reprodução. Use posição do áudio quando disponível, ou relógio monotônico sincronizado. Não use contador de frames nem reinicie o cronômetro em cada fase.

| elapsedMs | Fase | Tela superior | Tela inferior |
|---|---|---|---|
| 0–4499 | ShowFive | Apenas 5 AM | Preto |
| 4500–8999 | RollClock | 5 sobe, 6 entra, AM fixo | Preto |
| >=9000 | Celebrate | 6 AM + SURVIVED! + confetes + personagens A/B | Revelação obrigatória |

Não há CharactersEnter antes de Celebrate. Aos 9000 ms entram personagens e confetes e começa o fade inferior. Não tocar outro Yay se ele já está no áudio. A apresentação acontece no primeiro quadro que alcançar o limite.

## 2. Chamar os DOIS renderizadores no loop

Em todo quadro do estado de vitória:

1. Calcular elapsedMs a partir da mesma origem de tempo do áudio.
2. Calcular fase absoluta: <4500 ShowFive; <9000 RollClock; restante Celebrate.
3. Atualizar renderização da tela superior.
4. Atualizar renderização da tela inferior, inclusive nas Noites 1–4.
5. Publicar/transferir as alterações das duas telas pelo mecanismo do projeto.
6. Processar entrada somente quando permitido.

Não colocar renderBottom apenas dentro de uma condição night>=5. Só a presença do DOCUMENTO depende da noite. Caixa, texto e botão pertencem a TODAS as noites.

Pseudocódigo de referência, funções abstratas a adaptar ao projeto (não copiar como APIs libnds):

```cpp
void updateVictory(uint32_t elapsedMs, int completedNight) {
    if (elapsedMs < 4500) {
        drawTopFiveAM();
        drawBottomBlack();
    } else if (elapsedMs < 9000) {
        const float roll = (elapsedMs - 4500) / 4500.0f;
        drawTopClockRoll(roll);
        drawBottomBlack();
    } else {
        const uint32_t celebrationMs = elapsedMs - 9000;
        drawTopCelebration(completedNight, celebrationMs);
        drawBottomCompletion(completedNight, celebrationMs);
    }
    presentBothScreens();
}
```

Calcular celebrationMs apenas após elapsedMs>=9000 evita underflow com inteiros sem sinal. Não definir celebrationStart=now em todos os quadros: isso mantém o fade eternamente em zero.

## 3. Camadas inferiores e coordenadas

Limpar/compor preto ANTES dos elementos; não limpar novamente depois deles.

| Camada (ordem de desenho) | Arquivo | Posição | Dimensões |
|---|---|---|---|
| Fundo | Preenchimento preto, sem PNG | 0,0 | 256×192 |
| Caixa amarela | bottom/night_complete.png | 16,12 | 224×24 |
| Texto da noite | bottom/night_N_survived.png, N=1..7 | 16,40 | 224×16 |
| Documento borrado/nítido | Arquivos por noite abaixo | 16,60 | 224×104 |
| Botão | bottom/tap_to_continue.png | 16,168 | 224×16 |

Usar os nomes e maiúsculas exatamente como estão no ZIP. Todos os caminhos da tabela são relativos a assets/victory/. Não desenhar cheque e demissão simultaneamente; carregar só o documento escolhido.

## 4. Conteúdo por noite

| Noite | Texto | Documento nítido / borrado | Dados |
|---|---|---|---|
| 1 | NIGHT 1 SURVIVED | Nenhum | Caixa + texto + botão |
| 2 | NIGHT 2 SURVIVED | Nenhum | Caixa + texto + botão |
| 3 | NIGHT 3 SURVIVED | Nenhum | Caixa + texto + botão |
| 4 | NIGHT 4 SURVIVED | Nenhum | Caixa + texto + botão |
| 5 | NIGHT 5 SURVIVED | paycheck_5.png / paycheck_5_blur.png | Jeremy Fitzgerald; $100.50; assinatura; sem carimbo especial |
| 6 | NIGHT 6 SURVIVED | paycheck_6.png / paycheck_6_blur.png | Jeremy Fitzgerald; $20.10; OVERTIME |
| 7 | NIGHT 7 SURVIVED | termination_7.png / termination_7_blur.png | Fritz Smith; NOTICE OF TERMINATION; FIRED |

Os documentos da tabela ficam em bottom/. A demissão contém “Tampering with the animatronics” e “Odor”, sem valor monetário. Não inventar cheque para Noites 1–4. A Noite 6 é Extra Night; a 7 é Custom Night.

## 5. Fade-in inferior

t = celebrationMs, contado do zero aos 9000 ms do áudio.

Defina ramp(t,a,b)=clamp((t-a)/(b-a),0,1) usando ponto flutuante ou cálculo fixo equivalente; não usar divisão inteira inadvertidamente.

- Caixa: opacidade ramp(t,0,400).
- Texto: opacidade ramp(t,300,800).
- Documento: opacidade ramp(t,500,900).
- Nitidez: crossfade borrado→nítido por ramp(t,800,1400).
- Em t>=1400: documento nítido com opacidade 100%.
- Botão: só em t>=1400; visível quando floor((t-1400)/500)%2==0.

Assim, aos 9,0 s exatos a tela inferior ainda pode estar preta (opacidade zero); ela deve começar a aparecer imediatamente depois. Aos 9,4 s a caixa está plenamente visível, aos 9,8 s o texto também, e aos 10,4 s o documento está nítido e o botão já funciona. Nas Noites 1–4, a área reservada ao documento permanece preta, mas os outros elementos aparecem normalmente.

Composição ideal do documento sobre preto: primeiro interpolar as versões borrada e nítida pelo fator de nitidez, depois aplicar a opacidade geral uma vez. Evitar duas passagens que somem brilho. Adaptar ao suporte real do renderizador; uma tabela de intensidade pré-calculada também é aceitável. O PNG RGBA não garante fade automático no hardware.

## 6. DS/DSi: conferências de renderização

Identificar o engine realmente associado à tela inferior. Não presumir que basta preparar o engine superior. Respeitar qualquer screen swap já usado pelo projeto.

Verificar configuração do modo de vídeo inferior, banco de VRAM correspondente, BG/OBJ habilitados, prioridade das camadas, tamanho/formato dos buffers, paleta, alpha/transparência e transferência no momento correto. Se o renderizador usa buffers em RAM, respeitar suas regras de publicação/cache existentes. Não compartilhar acidentalmente ponteiros de buffers mutáveis entre as duas telas.

Ao sair de Office/câmeras, restaurar brilho/blending da tela inferior: brilho máximo negativo, overlay opaco ou alpha permanentemente zero podem esconder os elementos. Um overlay antigo não deve cobrir o documento. Não usar o PNG inteiro 224×104 como um único OBJ: dividir ou compor conforme o pipeline.

Converter/empacotar os PNGs para o formato aceito pelo projeto e incluir os arquivos no sistema de recursos real (NitroFS, assets compilados ou equivalente). Arquivos presentes no ZIP mas ausentes da build não estarão disponíveis à ROM.

## 7. Diagnóstico quando a tela inferior permanece preta

Sem diagnosticar por suposição, registrar e verificar nesta ordem:

1. O estado de vitória chama a função inferior em TODAS as noites?
2. elapsedMs ultrapassa 9000 e celebrationMs cresce? Registrar valores aos 9000, 9400, 9800 e 10400 ms.
3. completedNight está entre 1 e 7 e não foi incrementado antes de escolher os arquivos?
4. night_complete, night_N_survived e botão foram carregados? Registrar falhas e caminhos exatos.
5. Para 5–7, os dois arquivos do documento existem na build e têm buffers válidos?
6. A composição é publicada no engine/tela correto? Em uma build de diagnóstico, desenhar um retângulo amarelo opaco na tela inferior para distinguir problema de vídeo de problema de recurso/fade; remover depois.
7. O desenho ocorre antes de um clear final ou sob overlay opaco?
8. Brilho e alpha estão liberados? Evitar divisão inteira mantendo alpha zero.
9. O controlador avança/fecha a vitória antes de a revelação ocorrer?

Se apenas a tela superior aparece durante toda a comemoração, o comportamento não cumpre esta especificação. A causa deve ser confirmada no código ou pelos registros; este guia não afirma que houve um erro específico do outro ChatGPT.

## 8. Toque e saída

Não aceitar durante ShowFive/RollClock. Aceitar novo toque após t>=1400 na área (16,168,224,16), mesmo quando o botão está apagado pela piscada. Exigir soltura do toque antigo antes de armar o novo. Não concluir save/progressão duas vezes. Respeitar o fluxo de cutscenes/menu do projeto.

## 9. Critérios de aceite

- Nas Noites 1–4: caixa, texto correto e botão aparecem após 9 s, mesmo sem cheque.
- Noite 5: Jeremy e $100.50 nítidos após 10,4 s.
- Noite 6: Jeremy, $20.10 e OVERTIME nítidos após 10,4 s.
- Noite 7: Fritz, motivos e FIRED nítidos após 10,4 s.
- 5→6 nunca se inverte; A/B e confetes começam aos 9000 ms.
- Brilho/alpha permitem a tela inferior; buffers permanecem válidos.
- Áudio não duplica; toque mantido não pula a tela; sem leitura de SD durante animação.
- Compilar uma build nova e identificar versão; testar o código integrado no emulador e no DSi real.

O ZIP foi verificado como pacote de gráficos e documentação. Não declarar a ROM corrigida sem compilar e testar a integração.
