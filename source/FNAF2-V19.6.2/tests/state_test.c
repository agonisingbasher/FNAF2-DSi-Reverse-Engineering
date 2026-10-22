#include "game_state.h"
#include <assert.h>
int main(void) {
 Progress p;progressReset(&p);assert(progressValid(&p));assert(menuCount(&p)==3);
 for(unsigned night=1;night<=4;night++) {
  p.night=night;p.cheats=0;
  assert(menuFrame(&p,0)==(night-1)*4);
  assert(menuFrame(&p,1)==(night-1)*4+1);
  assert(menuFrame(&p,2)==(night-1)*4+2);
  p.cheats=1;assert(menuFrame(&p,2)==(night-1)*4+3);
 }
 p.night=5;p.cheats=0;assert(menuFrame(&p,0)==16);assert(menuFrame(&p,2)==18);
 p.cheats=1;assert(menuFrame(&p,2)==19);
 for(unsigned night=6;night<=7;night++) {
  p.night=night;assert(menuCount(&p)==5);
  for(unsigned i=0;i<4;i++)assert(menuFrame(&p,i)==20+(night==7?6:0)+i);
  p.cheats=0;assert(menuFrame(&p,4)==24+(night==7?6:0));
  p.cheats=1;assert(menuFrame(&p,4)==25+(night==7?6:0));
 }
 progressReset(&p);
 assert(!progressVictory(&p,2,0,false));assert(!progressVictory(&p,1,0,true));
 for(unsigned n=1;n<=5;n++)assert(progressVictory(&p,n,0,false));
 assert(p.night==6 && p.stars==1);
 assert(progressVictory(&p,6,0,false));assert(p.night==6 && p.stars==3);
 uint8_t ai[AI_COUNT];for(unsigned i=0;i<AI_COUNT;i++)ai[i]=20;
 assert(!progressVictory(&p,7,ai,true));ai[9]=19;assert(!progressVictory(&p,7,ai,false));
 assert(p.night==6 && p.stars==3);ai[9]=20;
 assert(progressVictory(&p,7,ai,false));assert(p.night==7 && p.stars==7);
 progressReset(&p);assert(p.night==1 && p.stars==0 && p.cheats==0);
 p.ai[0]=21;assert(!progressValid(&p));return 0;
}
