#include "cutscene_state.h"
#include <assert.h>
int main(void){Cutscene c;for(unsigned k=1;k<=4;k++){cutsceneReset(&c,k);assert(cutscenePan(&c)==72);for(unsigned i=0;i<180;i++)cutsceneTick(&c,true,false);assert(cutscenePan(&c)==0);for(unsigned i=0;i<360;i++)cutsceneTick(&c,false,true);assert(cutscenePan(&c)==144);for(unsigned i=0;i<50;i++)cutsceneTick(&c,true,true);assert(cutscenePan(&c)==144);assert(!cutsceneGoldenVisible(&c));while(!cutsceneFinished(&c))cutsceneTick(&c,false,false);assert(c.frames==960);}cutsceneReset(&c,3);assert(cutsceneGoldenVisible(&c) && cutsceneRoomFrame(&c)==2);cutsceneReset(&c,2);assert(cutsceneRoomFrame(&c)==0);for(unsigned i=0;i<480;i++)cutsceneTick(&c,false,false);assert(cutsceneRoomFrame(&c)==1);
 assert(cutsceneLogMode(1,0,false)==CUTSCENE_LOG_BOOT&&cutsceneLogMode(1,480,false)==CUTSCENE_LOG_ERRORS);
 assert(cutsceneLogMode(2,479,false)==CUTSCENE_LOG_BOOT&&cutsceneLogMode(2,480,false)==CUTSCENE_LOG_RECOGNITION&&cutsceneLogMode(2,780,false)==CUTSCENE_LOG_ITS_ME);
 assert(cutsceneLogMode(3,300,false)==CUTSCENE_LOG_BOOT&&cutsceneLogMode(3,300,true)==CUTSCENE_LOG_UNKNOWN);
 assert(cutsceneLogMode(4,839,false)==CUTSCENE_LOG_BOOT&&cutsceneLogMode(4,840,false)==CUTSCENE_LOG_OVERRIDE&&cutsceneLogMode(4,900,false)==CUTSCENE_LOG_BLACK);
 assert(cutsceneLogMode(6,100,false)==CUTSCENE_LOG_BOOT);return 0;}
