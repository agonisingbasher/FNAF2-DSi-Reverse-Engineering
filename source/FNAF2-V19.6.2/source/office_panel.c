#include "office_panel.h"
unsigned officePanelFrame(const Office *o,uint64_t now) {
 if(o->muteBluePending || o->muteFeedbackUntil>now)return 1;
 if(o->muteRestPending || o->muteResetUntil>now)return 0;
 unsigned selected=PANEL_NONE;
 if(o->maskFrame || o->maskTarget)selected=PANEL_MASK;
 else if(o->monitorFrame || o->monitorTarget)selected=PANEL_TABLET;
 else if(o->bbBlocked && o->blockedLight)selected=o->blockedLight;
 else if(o->leftLight)selected=PANEL_LEFT;
 else if(o->rightLight)selected=PANEL_RIGHT;
 else if(o->centerLight)selected=PANEL_HALL;
 return o->callActive?(selected?1+selected:0):7+selected;
}
typedef struct {int x0,y0,x1,y1;PanelAction action;} Hit;
PanelAction officePanelHit(unsigned frame,int x,int y) {
 static const Hit idle[]={
  {146,20,231,70,PANEL_HALL},{31,73,95,119,PANEL_LEFT},{176,73,239,119,PANEL_RIGHT},
  {29,122,105,173,PANEL_TABLET},{158,122,241,173,PANEL_MASK},{107,134,153,173,PANEL_MUTE}};
 static const Hit call[]={
  {138,17,225,65,PANEL_HALL},{27,70,89,114,PANEL_LEFT},{173,70,238,114,PANEL_RIGHT},
  {25,117,99,165,PANEL_TABLET},{155,117,238,165,PANEL_MASK},{102,124,152,165,PANEL_MUTE}};
 static const Hit quiet[]={
  {140,17,220,66,PANEL_HALL},{32,70,89,115,PANEL_LEFT},{169,70,227,115,PANEL_RIGHT},
  {34,117,114,165,PANEL_TABLET},{140,117,222,165,PANEL_MASK}};
 const Hit *hits=frame<2?idle:frame<7?call:quiet;unsigned n=frame<7?6:5;
 for(unsigned i=0;i<n;i++)if(x>=hits[i].x0 && x<hits[i].x1 && y>=hits[i].y0 && y<hits[i].y1)return hits[i].action;
 return PANEL_NONE;
}
void officePanelApply(Office *o,PanelAction action,uint64_t now) {
 if(o->dead || o->complete)return;
 if(action==PANEL_MUTE) {
  if(o->callActive){o->callActive=false;o->muted=true;o->muteFeedbackUntil=now+150;o->muteResetUntil=now+225;o->muteBluePending=o->muteRestPending=true;}
 } else if(action>=PANEL_HALL && action<=PANEL_LEFT) {
  if(!o->maskTarget && !o->maskFrame && !o->monitorTarget && !o->monitorFrame)
   o->touchLight=o->touchLight==(unsigned)action?PANEL_NONE:(unsigned)action;
 } else if(action==PANEL_MASK) {
  if(!o->monitorTarget && !o->monitorFrame){o->touchLight=PANEL_NONE;officeToggleMask(o);}
 } else if(action==PANEL_TABLET) {
  if(!o->maskTarget && !o->maskFrame){o->touchLight=PANEL_NONE;officeToggleMonitor(o);}
 }
}
bool officeUpIsHorn(const Office *o) {return o->pan==0;}

void officePanelPresented(Office *o,unsigned frame,uint64_t now) {
 /* Start each short feedback interval only after its frame is visible.
    Slow SD reads must not skip the blue or the return-to-idle frame. */
 if(frame==1 && o->muteBluePending) {
  o->muteBluePending=false;o->muteFeedbackUntil=now+150;
 } else if(frame==0 && o->muteRestPending && !o->muteBluePending && now>=o->muteFeedbackUntil) {
  o->muteRestPending=false;o->muteResetUntil=now+75;
 }
}
