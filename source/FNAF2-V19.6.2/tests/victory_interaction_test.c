#include "victory_interaction.h"
#include <assert.h>
int main(void){
 VictoryInteraction s;victoryInteractionReset(&s);
 GameOverInput idle={0},press={.held=true,.pressed=true,.x=35,.y=165};
 assert(victoryInteractionUpdate(&s,10399,false,press)==END_UI_NONE);
 assert(victoryInteractionSelected(&s)==END_UI_NONE);
 victoryInteractionUpdate(&s,10400,true,press);assert(victoryInteractionSelected(&s)==END_UI_NONE);
 victoryInteractionUpdate(&s,10410,true,idle);victoryInteractionUpdate(&s,10420,true,press);
 assert(victoryInteractionSelected(&s)==END_UI_PRIMARY);
 assert(victoryInteractionUpdate(&s,10430,true,idle)==END_UI_NONE);
 assert(victoryInteractionUpdate(&s,10629,true,idle)==END_UI_NONE);
 assert(victoryInteractionUpdate(&s,10630,true,idle)==END_UI_PRIMARY);
 assert(victoryInteractionUpdate(&s,10631,true,idle)==END_UI_NONE);
 victoryInteractionReset(&s);victoryInteractionUpdate(&s,10400,true,idle);
 press.x=180;victoryInteractionUpdate(&s,10401,true,press);
 assert(victoryInteractionSelected(&s)==END_UI_MENU);
 press.pressed=false;press.x=128;victoryInteractionUpdate(&s,10402,true,press);
 victoryInteractionUpdate(&s,10403,true,idle);assert(victoryInteractionSelected(&s)==END_UI_NONE);
 press.pressed=true;press.x=180;victoryInteractionUpdate(&s,10404,true,press);
 victoryInteractionUpdate(&s,10405,true,idle);assert(victoryInteractionUpdate(&s,10605,true,idle)==END_UI_MENU);
 return 0;
}
