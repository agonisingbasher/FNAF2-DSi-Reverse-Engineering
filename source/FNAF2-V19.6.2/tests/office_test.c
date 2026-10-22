#include "office_state.h"
#include "game_state.h"
#include <assert.h>
int main(void) {
 Office o;officeReset(&o,1);assert(officeFrame(&o)==2);
 unsigned corridor[]={3,7,8,9,10,11,14,15,18,19,20};
 for(unsigned i=0;i<11;i++){o.corridor=i;o.centerLight=true;assert(officeFrame(&o)==corridor[i]);}
 o.leftLight=true;o.leftVent=1;assert(officeFrame(&o)==1);
 o.leftVent=2;assert(officeFrame(&o)==16);o.leftVent=0;assert(officeFrame(&o)==4);
 o.leftLight=false;o.rightLight=true;o.rightVent=1;assert(officeFrame(&o)==6);
 o.rightVent=2;assert(officeFrame(&o)==17);o.rightVent=0;assert(officeFrame(&o)==5);
 for(unsigned i=1;i<=3;i++){o.invasion=i;unsigned f[]={0,0,12,13};assert(officeFrame(&o)==f[i]);}
 officeReset(&o,1);officeToggleMask(&o);officeToggleMonitor(&o);assert(o.maskTarget && !o.monitorTarget);
 for(unsigned i=0;i<10;i++)officeTick(&o,true,false,false,false,false);
 assert(o.maskFrame==10 && !o.centerLight);officeToggleMask(&o);
 for(unsigned i=0;i<10;i++)officeTick(&o,false,false,false,false,false);
 officeToggleMonitor(&o);officeToggleMask(&o);assert(o.monitorTarget && !o.maskTarget);
 officeReset(&o,1);o.moving=true;officeTick(&o,true,false,false,false,false);assert(!o.centerLight && o.flashlightNeedsRelease);
 o.moving=false;officeTick(&o,true,false,false,false,false);assert(!o.centerLight);
 officeTick(&o,false,false,false,false,false);officeTick(&o,true,false,false,false,false);assert(o.centerLight);
 o.batteryMilli=1;officeTick(&o,true,false,false,false,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,16);assert(o.batteryMilli==0 && !o.centerLight);
 officeReset(&o,1);officeTick(&o,true,false,false,false,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,116666);assert(o.batteryMilli>0);officeAdvanceBattery(&o,116667);assert(o.batteryMilli==0 && !o.centerLight);
 officeReset(&o,5);officeTick(&o,true,false,false,false,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,49999);assert(o.batteryMilli>0);officeAdvanceBattery(&o,50000);assert(o.batteryMilli==0 && !o.centerLight);
 officeReset(&o,1);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,100000);assert(o.batteryMilli==100000);
 officeReset(&o,1);officeTick(&o,false,false,false,true,false);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,10000);assert(o.batteryMilli<100000);
 officeReset(&o,1);officeTick(&o,false,false,false,false,true);officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,10000);assert(o.batteryMilli<100000);
 officeReset(&o,1);o.cameraLight=true;officeAdvanceBattery(&o,0);officeAdvanceBattery(&o,12000);assert(o.batteryMilli<90000);
 officeReset(&o,1);officeHallLightLockStart(&o,100,false);bool feedback=false;
 assert(officeHallLightLockUpdate(&o,200,true,&feedback) && feedback && o.hallLightLockUntil==4200);
 assert(officeHallLightLockUpdate(&o,1000,false,&feedback) && !feedback);
 assert(officeHallLightLockUpdate(&o,4199,false,&feedback));
 assert(!officeHallLightLockUpdate(&o,4200,false,&feedback));
 officeHallLightLockStart(&o,5000,true);officeTick(&o,true,false,false,false,false);assert(!o.centerLight);
 officeReset(&o,1);for(unsigned i=0;i<100;i++)officeTick(&o,false,true,false,false,false);assert(o.pan==0);
 for(unsigned i=0;i<100;i++){officeTick(&o,false,false,true,false,false);}
 assert(o.pan==144);
 Progress p;progressReset(&p);assert(!progressCanContinue(&p));assert(progressVictory(&p,1,0,false));assert(progressCanContinue(&p));
 progressReset(&p);assert(!progressCanContinue(&p) && p.rewards==0);
 return 0;
}
