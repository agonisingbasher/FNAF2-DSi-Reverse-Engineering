#include "cutscene_state.h"
void cutsceneReset(Cutscene *c,unsigned kind){c->kind=kind;c->frames=0;c->panFifths=72*5;}
void cutsceneTick(Cutscene *c,bool left,bool right){
 c->frames++;
 /* DSi adaptation: 24 px/s at 60 Hz, compared with office 360 px/s. */
 if(left!=right)c->panFifths+=left?-2:2;
 if(c->panFifths<0)c->panFifths=0;
 if(c->panFifths>144*5)c->panFifths=144*5;
}
unsigned cutsceneRoomFrame(const Cutscene *c){return c->kind==1?0:c->kind==2?(c->frames>=480?1:0):c->kind==3?2:1;}
int cutscenePan(const Cutscene *c){return c->panFifths/5;}
bool cutsceneGoldenVisible(const Cutscene *c){int p=cutscenePan(c);return c->kind==3 && p>=42 && p<=102;}
bool cutsceneFinished(const Cutscene *c){return c->frames>=960;}
unsigned cutsceneLogMode(unsigned kind,unsigned frames,bool goldenVisible){
 if(kind==1)return frames<480?CUTSCENE_LOG_BOOT:CUTSCENE_LOG_ERRORS;
 if(kind==2)return frames<480?CUTSCENE_LOG_BOOT:frames<780?CUTSCENE_LOG_RECOGNITION:CUTSCENE_LOG_ITS_ME;
 if(kind==3)return goldenVisible?CUTSCENE_LOG_UNKNOWN:CUTSCENE_LOG_BOOT;
 if(kind==4)return frames<840?CUTSCENE_LOG_BOOT:frames<900?CUTSCENE_LOG_OVERRIDE:CUTSCENE_LOG_BLACK;
 return CUTSCENE_LOG_BOOT;
}
