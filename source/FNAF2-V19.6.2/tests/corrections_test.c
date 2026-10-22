#include "office_state.h"
#include "office_audio.h"
#include "game_state.h"
#include <assert.h>
int main(void) {
 Office o;officeReset(&o,1);
 officeAdvanceClock(&o,88999);assert(o.hour==0 && !o.complete);
 for(unsigned h=1;h<=5;h++){officeAdvanceClock(&o,h*89000);assert(o.hour==h && !o.complete);}
 officeAdvanceClock(&o,533999);assert(o.hour==5 && !o.complete);
 officeAdvanceClock(&o,534000);assert(o.hour==6 && o.complete);
 assert(officeAudioPlan(false,false,&o).stopAll);
 Progress p;progressReset(&p);assert(!progressCanContinue(&p));assert(progressVictory(&p,1,0,false));assert(progressCanContinue(&p) && p.night==2);
 officeReset(&o,1);officeToggleMask(&o);
 OfficeAudioPlan a=officeAudioPlan(false,false,&o);assert(a.maskOn && !a.maskOff && !a.monitorOn);
 officeTick(&o,false,true,false,false,false);assert(o.pan==66 && o.maskFrame==2);
 for(unsigned i=0;i<9;i++)officeTick(&o,false,false,false,false,false);
 int pan=o.pan;officeTick(&o,false,false,true,false,false);assert(o.pan==pan+6 && o.maskFrame==10);
 officeToggleMask(&o);a=officeAudioPlan(true,false,&o);assert(a.maskOff && !a.maskOn);
 for(unsigned i=0;i<10;i++)officeTick(&o,false,false,false,false,false);
 officeToggleMonitor(&o);a=officeAudioPlan(false,false,&o);assert(a.monitorOn && !a.monitorOff);
 officeTick(&o,false,true,false,false,false);assert(o.pan==pan+6);
 officeToggleMonitor(&o);a=officeAudioPlan(false,true,&o);assert(a.monitorOff);
 officeReset(&o,1);officeTick(&o,true,false,false,false,false);assert(officeAudioPlan(false,false,&o).buzz);
 officeTick(&o,false,false,false,false,false);assert(!officeAudioPlan(false,false,&o).buzz);
 officeTick(&o,false,false,false,true,false);assert(officeAudioPlan(false,false,&o).buzz);
 officeTick(&o,false,false,false,false,false);assert(!officeAudioPlan(false,false,&o).buzz);
 o.dead=true;assert(officeAudioPlan(false,false,&o).stopAll);
 return 0;
}
