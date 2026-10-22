#include "office_panel.h"
#include "office_audio.h"
#include <assert.h>
static void tick(Office *o){officeTick(o,false,false,false,false,false);}
int main(void) {
 Office o;officeReset(&o,1);o.callActive=true;
 assert(officePanelFrame(&o,0)==0);
 const int centers[6][2]={{183,45},{204,94},{61,94},{199,147},{67,147},{130,151}};
 const PanelAction actions[]={PANEL_HALL,PANEL_RIGHT,PANEL_LEFT,PANEL_MASK,PANEL_TABLET,PANEL_MUTE};
 for(unsigned f=0;f<13;f++) {
  for(unsigned i=0;i<6;i++)assert(officePanelHit(f,centers[i][0],centers[i][1])==(f>=7 && i==5?PANEL_NONE:actions[i]));
  assert(officePanelHit(f,0,0)==PANEL_NONE);assert(officePanelHit(f,125,80)==PANEL_NONE);
 }
 for(unsigned light=1;light<=3;light++) {
  officePanelApply(&o,(PanelAction)light,0);tick(&o);
  assert(o.touchLight==light);assert(officePanelFrame(&o,0)==light+1);
  for(unsigned i=0;i<20;i++)tick(&o); // Finger released; light remains latched.
  assert(officeAudioPlan(false,false,&o).buzz);
  officePanelApply(&o,(PanelAction)light,0);tick(&o);
  assert(o.touchLight==0 && officePanelFrame(&o,0)==0 && !officeAudioPlan(false,false,&o).buzz);
 }
 officePanelApply(&o,PANEL_HALL,0);tick(&o);officePanelApply(&o,PANEL_RIGHT,0);tick(&o);
 assert(o.rightLight && !o.centerLight && !o.leftLight); // One touch light selected at a time.
 officePanelApply(&o,PANEL_MASK,0);tick(&o);assert(o.maskTarget && !o.rightLight && o.touchLight==0 && officePanelFrame(&o,0)==5);
 officePanelApply(&o,PANEL_TABLET,0);assert(!o.monitorTarget);
 officePanelApply(&o,PANEL_MASK,0);for(unsigned i=0;i<5;i++)tick(&o);assert(!o.maskFrame);
 officePanelApply(&o,PANEL_TABLET,0);tick(&o);assert(o.monitorTarget && officePanelFrame(&o,0)==6);
 officePanelApply(&o,PANEL_TABLET,0);for(unsigned i=0;i<6;i++)tick(&o);assert(!o.monitorFrame);
 officePanelApply(&o,PANEL_HALL,0);tick(&o);officePanelApply(&o,PANEL_MUTE,1000);
 assert(o.muted && !o.callActive && o.centerLight && officePanelFrame(&o,1000)==1);
 officePanelPresented(&o,1,1000);
 assert(officePanelFrame(&o,1149)==1 && officePanelFrame(&o,1150)==0);
 officePanelPresented(&o,0,1150);
 assert(officePanelFrame(&o,1224)==0 && officePanelFrame(&o,1225)==8); // Mute ends only call, preserving lamp.
 officePanelApply(&o,PANEL_HALL,1225);tick(&o);assert(officePanelFrame(&o,1225)==7);
 officeReset(&o,6);o.callActive=true;assert(!o.muted && !o.touchLight);
 o.pan=0;assert(officeUpIsHorn(&o));o.pan=6;assert(!officeUpIsHorn(&o));
 o.batteryMilli=0;officePanelApply(&o,PANEL_LEFT,0);tick(&o);assert(!o.leftLight && officePanelFrame(&o,0)==0);
 officeReset(&o,1);o.callActive=true;o.callActive=false;assert(officePanelFrame(&o,0)==7); // Natural completion hides mute.
 officeReset(&o,1);o.callActive=true;officePanelApply(&o,PANEL_MUTE,1000);
 assert(officePanelFrame(&o,5000)==1); // Slow reads cannot skip blue feedback.
 officePanelPresented(&o,1,5000);assert(officePanelFrame(&o,5149)==1);
 assert(officePanelFrame(&o,9000)==0); // Nor skip the requested idle return.
 officePanelPresented(&o,0,9000);assert(officePanelFrame(&o,9074)==0 && officePanelFrame(&o,9075)==7);
 return 0;
}
