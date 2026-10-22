#include "night_ai.h"
#include <assert.h>
#include <string.h>
static int drainAtFPS(unsigned fps,bool camera,unsigned duration){
 Office o;officeReset(&o,1);
 if(camera){o.monitorTarget=true;o.monitorFrame=11;o.cameraLight=true;}
 else officeTick(&o,true,false,false,false,false);
 officeAdvanceBattery(&o,0);
 for(unsigned n=1;n<=fps*duration/1000;n++)officeAdvanceBattery(&o,(uint64_t)n*1000/fps);
 officeAdvanceBattery(&o,duration);return o.batteryMilli;
}
int main(void){
 /* Slow and fast frame rates must charge exactly the same lit exposure. */
 for(unsigned fps=1;fps<=120;fps++){
  assert(drainAtFPS(fps,false,30000)==74286);
  assert(drainAtFPS(fps,true,30000)==74286);
 }
 Office o;officeReset(&o,1);
 officeTick(&o,true,false,false,false,false);officeAdvanceBattery(&o,0);
 officeAdvanceBattery(&o,30000);assert(officeBatteryIcon(&o)==1);
 officeAdvanceBattery(&o,60000);assert(officeBatteryIcon(&o)==2);
 officeAdvanceBattery(&o,90000);assert(officeBatteryIcon(&o)==3);
 officeAdvanceBattery(&o,116667);assert(officeBatteryIcon(&o)==4 && !o.centerLight);
 o.batteryMilli=75001;assert(officeBatteryIcon(&o)==0);o.batteryMilli=75000;assert(officeBatteryIcon(&o)==1);
 o.batteryMilli=50000;assert(officeBatteryIcon(&o)==2);o.batteryMilli=25000;assert(officeBatteryIcon(&o)==3);
 officeReset(&o,1);officeTick(&o,true,false,false,false,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,1000);
 officeTick(&o,false,false,false,false,false);officeAdvanceBattery(&o,1000);int before=o.batteryMilli;
 officeAdvanceBattery(&o,89000);assert(o.batteryMilli==before); /* Idle never drains. */
 officeReset(&o,1);officeTick(&o,false,false,false,true,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,30000);assert(o.batteryMilli==74286);
 officeReset(&o,1);officeTick(&o,false,false,false,false,true);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,30000);assert(o.batteryMilli==74286);

 /* Skip clock only: keep battery, AI gameplay and monotonic time intact. */
 officeReset(&o,1);o.batteryMilli=67890;officeAdvanceClock(&o,12345);
 assert(officeDebugSetHour(&o,5,12345));assert(o.hour==5 && !o.complete && o.debugUsed && !o.previewUsed && o.batteryMilli==67890);
 officeAdvanceClock(&o,12345+88999);assert(o.hour==5 && !o.complete);
 officeAdvanceClock(&o,12345+89000);assert(o.hour==6 && o.complete);
 assert(!officeDebugSetHour(&o,5,0));
 officeReset(&o,3);assert(!officeDebugSetHour(&o,6,0));assert(!o.debugUsed);
 assert(officeDebugNextNight(1,1)==2 && officeDebugNextNight(7,1)==1 && officeDebugNextNight(1,-1)==7);
 for(unsigned n=1;n<=7;n++){officeReset(&o,n);assert(o.hour==0 && o.batteryMilli==100000);}

 uint8_t levels[10]={0},out[10];unsigned puppet;
 for(unsigned hour=0;hour<6;hour++){
  nightLevels(1,hour,levels,out,&puppet);
  for(unsigned i=0;i<10;i++)assert(out[i]==(i==TB && hour>=2?1:0));
 }
 for(unsigned dt=1;dt<=1000;dt=dt==1?17:dt==17?100:dt==100?1000:1001){
  NightAI a;officeReset(&o,1);nightAIReset(&a,1,levels,false,111);
  for(unsigned t=dt;t<534000;t+=dt){o.hour=t/89000;nightAIUpdate(&a,&o,t,9,false,false);}
  o.hour=5;nightAIUpdate(&a,&o,534000,9,false,false);
  assert(a.music==86650 && a.puppetStage==0 && !o.dead);
  assert(a.loc[TF]==9 && a.loc[TC]==9 && a.loc[MG]==12 && a.loc[WF]==8 && a.loc[WB]==8 && a.loc[WC]==8 && a.loc[FX]==8);
 }
 for(unsigned n=2;n<=7;n++){
  NightAI a;officeReset(&o,n);nightAIReset(&a,n,levels,false,111);nightAIUpdate(&a,&o,1000,9,false,false);
  assert(a.music==100000-(3+(int)n)*100); /* Preserve later-night rates. */
 }
 NightAI a;officeReset(&o,3);nightAIReset(&a,3,levels,false,111);
 assert(!nightHallwayAudioActive(&a,&o));
 for(unsigned who=0;who<10;who++){
  unsigned old=a.loc[who];a.loc[who]=HALL;
  o.centerLight=o.cameraLight=false;assert(nightHallwayAudioActive(&a,&o));
  o.centerLight=true;assert(nightHallwayAudioActive(&a,&o));
  o.centerLight=false;o.maskFrame=10;o.monitorFrame=11;o.monitorTarget=true;assert(nightHallwayAudioActive(&a,&o));
  a.loc[who]=old;assert(!nightHallwayAudioActive(&a,&o));
 }
 a.loc[WF]=a.loc[FX]=HALL;a.loc[WF]=8;assert(nightHallwayAudioActive(&a,&o));
 o.dead=true;assert(!nightHallwayAudioActive(&a,&o));o.dead=false;o.complete=true;assert(!nightHallwayAudioActive(&a,&o));
 return 0;
}
