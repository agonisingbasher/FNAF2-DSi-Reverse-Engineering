#include "camera_state.h"
#include <string.h>
#include <limits.h>
const CamPoint cameraBoxes[12]={{25,111},{116,111},{25,86},{116,86},{43,141},{109,141},{116,61},{25,40},{198,40},{163,91},{205,72},{205,120}};
void cameraReset(CameraState *c){memset(c,0,sizeof(*c));c->selected=9;c->noiseStep=6;}
unsigned cameraNeighbor(unsigned current,int dx,int dy){
 if(current<1 || current>12)return 9;
 int best=INT_MAX;unsigned id=current;
 for(unsigned pass=0;pass<2;pass++){
  for(unsigned i=0;i<12;i++){
   int x=cameraBoxes[i].x-cameraBoxes[current-1].x,y=cameraBoxes[i].y-cameraBoxes[current-1].y;
   int along=x*dx+y*dy,across=dx?y:x;if(across<0)across=-across;
   if(along<=0 || (!pass && across>along))continue;
   int score=x*x+y*y+3*(dx?y*y:x*x);if(score<best){best=score;id=i+1;}
  }
  if(id!=current)break;
 }
 return id;
}
unsigned cameraHit(int x,int y){for(unsigned i=0;i<12;i++)if(x>=cameraBoxes[i].x && x<cameraBoxes[i].x+29 && y>=cameraBoxes[i].y && y<cameraBoxes[i].y+16)return i+1;return 0;}
bool cameraTabletHit(int x,int y){return x>=210 && x<250 && y>=161 && y<189;}
bool cameraWindHit(int x,int y){return x>=145 && x<207 && y>=140 && y<159;}
bool cameraLightHit(int x,int y){return x>=215 && x<248 && y>=141 && y<162;}
void cameraSelect(CameraState *c,unsigned id,uint64_t now){if(id>=1 && id<=12 && id!=c->selected){c->selected=id;c->changedAt=now;}}
void cameraRequestClose(CameraState *c){if(!c->closeStage){c->closeStage=1;c->closeShown=false;}}
bool cameraAdvanceClose(CameraState *c,uint64_t now){if(!c->closeStage || !c->closeShown || now<c->closeUntil)return false;if(c->closeStage==1){c->closeStage=2;c->closeShown=false;return false;}c->closeStage=0;c->closeShown=false;return true;}
void cameraClosePresented(CameraState *c,uint64_t now){if(c->closeStage && !c->closeShown){c->closeShown=true;c->closeUntil=now+(c->closeStage==1?80:50);}}
unsigned cameraPan(unsigned id,uint64_t elapsed){
 if(id==1)return 48;
 if(id==2)return 96;
 if(id>=3 && id<=6)return 72;
 const unsigned hold=1000,travel=1800;unsigned t=elapsed%(hold*2+travel*2);
 if(t<hold)return 0;
 t-=hold;if(t<travel)return t*144/travel;
 t-=travel;if(t<hold)return 144;
 return 144-(t-hold)*144/travel;
}
