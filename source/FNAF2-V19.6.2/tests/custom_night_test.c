#include "custom_night.h"
#include "night_ai.h"
#include "game_state.h"
#include <assert.h>
int main(void){
 unsigned n;
 uint8_t levels[10]={0};
 assert(customMove(0,-1,0)==4);assert(customMove(4,1,0)==0);
 assert(customMove(2,0,1)==7);assert(customMove(7,0,1)==2);
 customAdjust(levels,0,-1);assert(levels[0]==0);
 for(n=0;n<30;n++)customAdjust(levels,0,1);
 assert(levels[0]==20);customAdjust(levels,0,-1);assert(levels[0]==19);
 for(n=1;n<CUSTOM_PRESETS;n++){customPreset(levels,n);for(unsigned i=0;i<10;i++)assert(levels[i]<=20);}
 customPreset(levels,3);for(n=0;n<10;n++)assert(levels[n]==20);
 customPreset(levels,1);for(n=0;n<10;n++)assert(levels[n]==5);
 for(unsigned cheat=0;cheat<2;cheat++){
  Office o;NightAI ai;officeReset(&o,7);nightAIReset(&ai,7,levels,cheat,123);
  ai.loc[GF]=INSIDE;ai.level[GF]=20;o.centerLight=true;
  nightAIUpdate(&ai,&o,101,9,false,false);assert(o.dead==!cheat);
 }
 for(unsigned who=WF;who<=WC;who++){
  Office o;NightAI ai;officeReset(&o,7);nightAIReset(&ai,7,levels,false,123);
  ai.presentationTracking=true;ai.loc[who]=INSIDE;ai.level[who]=20;ai.entered[who]=1;
  nightAIUpdate(&ai,&o,5000,9,false,false);assert(!o.dead && ai.awaitingOfficeFrame[who]);
  nightAIUpdate(&ai,&o,8000,9,false,false);assert(!o.dead);
  nightOfficePresented(&ai,8000);
  nightAIUpdate(&ai,&o,9499,9,false,false);assert(!o.dead);
  o.maskFrame=10;nightAIUpdate(&ai,&o,9499,9,false,false);assert(!o.dead && ai.answered[who]);
  nightAIUpdate(&ai,&o,13000,9,false,false);assert(!o.dead && ai.loc[who]!=INSIDE);
  officeReset(&o,7);nightAIReset(&ai,7,levels,false,123);
  ai.presentationTracking=true;ai.loc[who]=INSIDE;ai.level[who]=20;ai.entered[who]=1;
  nightAIUpdate(&ai,&o,5000,9,false,false);nightOfficePresented(&ai,8000);
  nightAIUpdate(&ai,&o,9500,9,false,false);assert(o.dead);
 }
 Progress p;progressReset(&p);p.cheats=1;p.night=6;p.stars=3;progressNewGame(&p);assert(p.cheats==1 && p.night==1 && p.stars==0);assert(!progressVictory(&p,1,levels,true));assert(p.night==1);
}
