#include "gameover_state.h"
#include <assert.h>
static GameOverInput clear;
static void entrance(GameOverState *s){
 gameOverReset(s,0);
 for(unsigned i=0;i<5;i++){
  assert(gameOverFrame(s,i*120)==i);
  assert(gameOverUpdate(s,(i+1)*120,clear)==END_UI_NONE);
 }
 assert(s->phase==GO_IDLE);
 for(unsigned i=0;i<15;i++)assert(gameOverFrame(s,600+i*400)==5+i%5);
}
int main(void){
 GameOverState s;
 for(unsigned menu=0;menu<2;menu++){
  entrance(&s);
  GameOverInput t={.held=true,.pressed=true,.x=menu?190:30,.y=165};
  gameOverUpdate(&s,601,t);assert(s.phase==GO_PRESS);
  assert(gameOverFrame(&s,601)==(menu?17:16));
  gameOverUpdate(&s,602,clear);
  for(unsigned i=0;i<5;i++){
   assert(gameOverUpdate(&s,802+i*90,t)==END_UI_NONE);
   assert(gameOverFrame(&s,802+i*90)==10+i);
  }
  assert(gameOverUpdate(&s,1252,t)==(menu?END_UI_MENU:END_UI_PRIMARY));
  assert(gameOverUpdate(&s,1253,t)==END_UI_NONE);
 }
 entrance(&s);
 GameOverInput t={.held=true,.pressed=true,.x=30,.y=165};
 gameOverUpdate(&s,601,t);
 t.x=128;t.pressed=false;gameOverUpdate(&s,602,t);gameOverUpdate(&s,603,clear);
 assert(s.phase==GO_IDLE && s.selected==END_UI_NONE);
 return 0;
}
