#pragma once
#include <stdint.h>
#include "gameover_state.h"
typedef struct {GameOverState state;} VictoryInteraction;
void victoryInteractionReset(VictoryInteraction *s);
EndUIAction victoryInteractionUpdate(VictoryInteraction *s,uint32_t now,bool ready,GameOverInput input);
EndUIAction victoryInteractionSelected(const VictoryInteraction *s);
