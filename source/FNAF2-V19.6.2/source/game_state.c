#include "game_state.h"
#include <string.h>
void progressReset(Progress *p) { memset(p,0,sizeof(*p));p->night=1; }
bool progressValid(const Progress *p) {
 if(p->night<1 || p->night>7 || p->stars>7 || p->cheats>1 || p->rewards>511) return false;
 for(unsigned i=0;i<AI_COUNT;i++) if(p->ai[i]>20)return false;
 return true;
}
unsigned menuCount(const Progress *p) { return p->night>=6 ? 5 : 3; }
unsigned menuFrame(const Progress *p,unsigned selected) {
 unsigned c=selected;
 if(p->night<=5) {
  if(c==2)c+=p->cheats;
  return p->night<=4 ? (p->night-1)*4+c : 16+c;
 }
 if(c==4)c+=p->cheats;
 return 20+(p->night==7?6:0)+c;
}
bool progressVictory(Progress *p,unsigned night,const uint8_t *ai,bool cheatsUsed) {
 if(cheatsUsed || night<1 || night>7)return false;
 if(night==7) {
  if(!ai || p->night<6)return false;
  for(unsigned i=0;i<AI_COUNT;i++)if(ai[i]!=20)return false;
  p->stars|=4;p->night=7;return true;
 }
 if(night==6) { if(p->night<6)return false;p->stars|=2;return true; }
 if(night!=p->night)return false;
 if(night==5)p->stars|=1;
 p->night=night+1;return true;
}

bool progressCanContinue(const Progress *p) { return progressValid(p) && p->night>=2; }

void progressNewGame(Progress *p){uint8_t cheats=p->cheats;progressReset(p);p->cheats=cheats?1:0;}
