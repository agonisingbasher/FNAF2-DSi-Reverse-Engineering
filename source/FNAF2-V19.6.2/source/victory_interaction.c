#include "victory_interaction.h"
void victoryInteractionReset(VictoryInteraction *s){gameOverReset(&s->state,0);s->state.phase=GO_IDLE;}
EndUIAction victoryInteractionSelected(const VictoryInteraction *s){return s->state.selected;}
EndUIAction victoryInteractionUpdate(VictoryInteraction *s,uint32_t now,bool ready,GameOverInput input){
 GameOverState *v=&s->state;
 if(!ready){gameOverReset(v,now);v->phase=GO_IDLE;return END_UI_NONE;}
 if(v->phase==GO_CONFIRM && (uint32_t)(now-v->phaseStart)>=GO_SELECTED_MS){
  v->phase=GO_DONE;if(!v->delivered){v->delivered=true;return v->selected;}
 }
 if(v->phase==GO_DONE)return END_UI_NONE;
 /* Reuse the same press/drag/release and confirmation policy as Game Over. */
 return gameOverUpdate(v,now,input);
}
