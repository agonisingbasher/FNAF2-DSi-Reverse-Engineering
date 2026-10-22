#include "office_state.h"
#include <string.h>
static unsigned flashlightCapacity(unsigned night) {
 if(night<=1)return 7000;
 if(night==2)return 6000;
 if(night==3)return 5000;
 if(night==4)return 4000;
 return 3000;
}
void officeReset(Office *o,unsigned night) {memset(o,0,sizeof(*o));o->night=night;o->pan=72;o->batteryMilli=100000;}
unsigned officeFrame(const Office *o) {
 if(o->invasion>0){static const unsigned f[]={2,0,12,13};return f[o->invasion<=3?o->invasion:0];}
 if(o->leftLight){static const unsigned f[]={4,1,16};return f[o->leftVent<=2?o->leftVent:0];}
 if(o->rightLight){static const unsigned f[]={5,6,17};return f[o->rightVent<=2?o->rightVent:0];}
 if(o->centerLight){static const unsigned f[]={3,7,8,9,10,11,14,15,18,19,20};return f[o->corridor<=10?o->corridor:0];}
 return 2;
}
void officeToggleMask(Office *o) {if(!o->monitorTarget && o->monitorFrame==0)o->maskTarget=!o->maskTarget;}
void officeToggleMonitor(Office *o) {if(!o->maskTarget && o->maskFrame==0)o->monitorTarget=!o->monitorTarget;}
void officeTick(Office *o,bool x,bool left,bool right,bool l,bool r) {
 if(o->dead || o->complete)return;
 ++o->ticks;
 if(o->maskTarget || o->monitorTarget)o->touchLight=0;
 x=x || o->touchLight==1;l=l || o->touchLight==3;r=r || o->touchLight==2;
 x=x && !o->hallLightLockActive;
 if(o->maskTarget && o->maskFrame<10)o->maskFrame=o->maskFrame+2>10?10:o->maskFrame+2;
 if(!o->maskTarget && o->maskFrame>0)o->maskFrame=o->maskFrame<2?0:o->maskFrame-2;
 if(o->monitorTarget && o->monitorFrame<11)o->monitorFrame=o->monitorFrame+2>11?11:o->monitorFrame+2;
 if(!o->monitorTarget && o->monitorFrame>0)o->monitorFrame=o->monitorFrame<2?0:o->monitorFrame-2;
 if(!o->monitorFrame) {
  if(left && o->pan>0)o->pan-=6;
  if(right && o->pan<144)o->pan+=6;
  if(o->pan<0)o->pan=0;
  if(o->pan>144)o->pan=144;
 }
 if(o->moving && x)o->flashlightNeedsRelease=true;
 if(!x)o->flashlightNeedsRelease=false;
 bool available=o->batteryMilli>0 && !o->maskFrame && !o->monitorFrame;
 o->centerLight=available && x && !o->moving && !o->flashlightNeedsRelease;
 o->leftLight=available && l;o->rightLight=available && r;
 if(o->batteryMilli<=0){o->batteryMilli=0;o->centerLight=o->leftLight=o->rightLight=o->cameraLight=false;}
 // Visual preview has no deaths or victories; invasion deadline remains ready for AI.
 if(!o->aiManaged && o->invasion && o->preview==0 && !o->maskTarget) {
  if(++o->invasionTicks>=90)o->dead=true;
 } else o->invasionTicks=0;
}
void officeHallLightLockStart(Office *o,uint64_t now,bool inputHeld) {
 o->hallLightLockUntil=now+4000;o->hallLightLockActive=true;o->hallLightLockHeld=inputHeld;
}
bool officeHallLightLockUpdate(Office *o,uint64_t now,bool inputHeld,bool *playFeedback) {
 if(playFeedback)*playFeedback=false;
 if(!o->hallLightLockActive)return false;
 if(inputHeld){
  if(!o->hallLightLockHeld && playFeedback)*playFeedback=true;
  o->hallLightLockHeld=true;o->hallLightLockUntil=now+4000;
  return true;
 }
 o->hallLightLockHeld=false;
 if(now>=o->hallLightLockUntil){o->hallLightLockActive=false;return false;}
 return true;
}
void officePreview(Office *o) {
 o->previewUsed=true;
 o->preview=(o->preview+1)%19;o->invasion=o->corridor=o->leftVent=o->rightVent=0;
 if(o->preview<=10)o->corridor=o->preview;
 else if(o->preview<=13)o->invasion=o->preview-10;
 else if(o->preview<=15)o->leftVent=o->preview-13;
 else if(o->preview<=17)o->rightVent=o->preview-15;
 else if(o->preview==18)o->moving=true;
 if(o->preview!=18)o->moving=false;
}

void officeAdvanceClock(Office *o,uint64_t elapsedMilliseconds) {
 if(o->dead || o->complete)return;
 int64_t effective=(int64_t)elapsedMilliseconds+o->debugClockOffsetMilliseconds;
 uint64_t hour=effective>0?(uint64_t)effective/89000:0;
 o->hour=hour>=6?6:(unsigned)hour;
 if(o->hour==6)o->complete=true;
}

uint64_t officeCounterMilliseconds(uint32_t elapsedTicks,uint32_t busClock) {
 return busClock?((uint64_t)elapsedTicks*1024000)/busClock:0;
}

/* Integrate actual lit time, not number of rendered frames. Capacity retains
   the old 60 Hz equivalent (7000 lit frames on N1 = 116.667 seconds). */
void officeAdvanceBattery(Office *o,uint64_t now) {
 if(!o->batteryClockSet){o->batteryClockSet=true;o->batteryLastMilliseconds=now;}
 uint64_t dt=now>=o->batteryLastMilliseconds?now-o->batteryLastMilliseconds:0;
 o->batteryLastMilliseconds=now;
 if(o->batteryLighting && !o->dead && !o->complete && o->batteryMilli>0) {
  uint64_t denominator=(uint64_t)flashlightCapacity(o->night)*1000;
  uint64_t numerator=dt*6000000+(unsigned)o->batteryDrainRemainder;
  uint64_t drain=numerator/denominator;
  o->batteryDrainRemainder=(int)(numerator%denominator);
  o->batteryMilli=drain>=(unsigned)o->batteryMilli?0:o->batteryMilli-(int)drain;
 }
 if(o->batteryMilli<=0){o->batteryMilli=0;o->centerLight=o->leftLight=o->rightLight=o->cameraLight=false;}
 o->batteryLighting=o->batteryMilli>0 && (o->centerLight || o->leftLight || o->rightLight || o->cameraLight);
}
unsigned officeBatteryIcon(const Office *o) {
 return o->batteryMilli>75000?0:o->batteryMilli>50000?1:o->batteryMilli>25000?2:o->batteryMilli>0?3:4;
}
bool officeDebugSetHour(Office *o,unsigned hour,uint64_t now) {
 if(hour>5 || o->dead || o->complete)return false;
 o->debugUsed=true;o->debugClockOffsetMilliseconds=(int64_t)hour*89000-(int64_t)now;
 officeAdvanceClock(o,now);return true;
}
unsigned officeDebugNextNight(unsigned current,int direction) {
 if(current<1 || current>7)current=1;
 return direction<0?(current==1?7:current-1):(current==7?1:current+1);
}
