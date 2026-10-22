#include "gameover_state.h"
#include "touch_ui.h"
/* 4x5 row-major source rectangles. Inactive orange cells are unreachable. */
static const unsigned offFrames[]={10,11,12,13};
void gameOverReset(GameOverState *s,uint32_t now){
 *s=(GameOverState){.phase=GO_ENTRY,.phaseStart=now,.idleStart=now};
}
bool gameOverLocked(const GameOverState *s){return s->phase>=GO_CONFIRM;}
EndUIAction gameOverUpdate(GameOverState *s,uint32_t now,GameOverInput in){
 if(s->phase==GO_ENTRY){
  if((uint32_t)(now-s->phaseStart)>=GO_ENTRY_MS){s->phaseStart=now;if(++s->shutdownStep==5){s->phase=GO_IDLE;s->idleStart=now;s->armed=!in.held;}}
 }else if(s->phase==GO_IDLE){
  if(!in.held)s->armed=true;
  if(in.retryKey || in.menuKey){s->selected=in.menuKey?END_UI_MENU:END_UI_PRIMARY;s->phase=GO_CONFIRM;s->phaseStart=now;}
  else if(in.held && in.pressed && s->armed){
   s->selected=touchUIHit(in.x,in.y);s->armed=false;
   if(s->selected!=END_UI_NONE)s->phase=GO_PRESS;
  }
 }else if(s->phase==GO_PRESS){
  if(in.held){
   s->selected=touchUIHit(in.x,in.y); /* Drag outside cancels; drag across changes button. */
  }else if(s->selected!=END_UI_NONE){s->phase=GO_CONFIRM;s->phaseStart=now;}
  else{s->phase=GO_IDLE;s->armed=true;}
 }else if(s->phase==GO_CONFIRM){
  if((uint32_t)(now-s->phaseStart)>=GO_SELECTED_MS){s->phase=GO_SHUTDOWN;s->shutdownStep=0;s->phaseStart=now;}
 }else if(s->phase==GO_SHUTDOWN){
  /* One step at a time: even after a slow frame every CRT cell is presented. */
  if((uint32_t)(now-s->phaseStart)>=GO_OFF_MS){
   s->phaseStart=now;
   if(++s->shutdownStep==4)s->phase=GO_BLACK;
  }
 }else if(s->phase==GO_BLACK){
  /* The black frame must reach the display before loading a new scene. */
  if((uint32_t)(now-s->phaseStart)>=GO_OFF_MS){s->phase=GO_DONE;}
 }
 if(s->phase==GO_DONE && !s->delivered){s->delivered=true;return s->selected;}
 return END_UI_NONE;
}
unsigned gameOverFrame(const GameOverState *s,uint32_t now){
 if(s->phase==GO_ENTRY)return s->shutdownStep;
 if(s->phase==GO_IDLE || (s->phase==GO_PRESS && s->selected==END_UI_NONE))return ((uint32_t)(now-s->idleStart)/GO_IDLE_MS)%5+5;
 if(s->phase==GO_PRESS || s->phase==GO_CONFIRM)return s->selected==END_UI_MENU?17:16;
 if(s->phase==GO_SHUTDOWN)return offFrames[s->shutdownStep];
 return GO_BLACK_FRAME;
}
