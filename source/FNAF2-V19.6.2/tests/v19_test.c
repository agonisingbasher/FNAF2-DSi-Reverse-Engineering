#include "night_ai.h"
#include <assert.h>
#include <string.h>
static Office o;static NightAI a;static uint8_t levels[10];
static void reset(void){officeReset(&o,7);memset(levels,0,10);nightAIReset(&a,7,levels,true,123);a.music=100000;}
int main(void){
 assert(nightBonnieMaskX(0)==256 && nightBonnieMaskX(2500)==64 && nightBonnieMaskX(5000)==-128 && nightBonnieMaskX(6000)==-128);
 reset();assert(!nightHallwayAudioActive(&a,&o));
 a.loc[WF]=HALL;assert(nightHallwayAudioActive(&a,&o));
 o.centerLight=false;o.monitorTarget=true;o.monitorFrame=11;assert(nightHallwayAudioActive(&a,&o));
 a.loc[WF]=8;assert(!nightHallwayAudioActive(&a,&o));
 assert(nightFootstepsAudible(true,1) && nightFootstepsAudible(true,4));
 assert(!nightFootstepsAudible(true,7) && !nightFootstepsAudible(true,10) && !nightFootstepsAudible(false,2));
 // Toy Chica's CAM09 frame and the office hallway state change on the same AI move.
 reset();a.level[TC]=20;a.loc[TC]=4;a.step[TC]=2;a.nextMove[TC]=0;
 nightAIUpdate(&a,&o,100,9,false,false);
 assert(a.loc[TC]==HALL && a.hallEvent && o.corridor!=0);
 assert(nightCameraFrame(&a,9,false)==2 && nightCameraFrame(&a,9,true)==3);
 // Leaving the center hallway under a lit flashlight triggers the static lock.
 reset();a.level[TF]=20;a.loc[TF]=HALL;a.step[TF]=3;a.nextMove[TF]=0;a.previousFlash=true;o.centerLight=true;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[TF]==INSIDE && a.hallExitEvent);
 // Withered Foxy waits for an occupied middle hall; only Mangle and Withered Bonnie
 // have combined hall frames with him in Office frames 20 and 19 respectively.
 officeReset(&o,7);o.centerLight=true;o.corridor=9;assert(officeFrame(&o)==19);
 o.corridor=10;assert(officeFrame(&o)==20);
 reset();a.level[FX]=a.custom[FX]=20;a.loc[FX]=8;a.step[FX]=0;a.nextMove[FX]=0;a.loc[TC]=HALL;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[FX]==8);
 reset();a.level[FX]=a.custom[FX]=20;a.loc[FX]=8;a.step[FX]=0;a.nextMove[FX]=0;a.loc[MG]=HALL;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[FX]==HALL && a.loc[MG]==HALL);
 reset();a.level[FX]=a.custom[FX]=20;a.loc[FX]=8;a.step[FX]=0;a.nextMove[FX]=0;a.loc[WB]=HALL;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[FX]==HALL && a.loc[WB]==HALL);
 reset();a.level[FX]=a.custom[FX]=20;a.loc[FX]=8;a.step[FX]=0;a.nextMove[FX]=0;a.loc[TC]=2;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[FX]==HALL);
 // A properly masked Toy Bonnie retreats to CAM03 and never routes through CAM01.
 reset();a.level[TB]=a.custom[TB]=20;o.maskFrame=10;a.loc[TB]=RIGHT_DOOR;a.step[TB]=5;a.defending[TB]=true;a.answered[TB]=true;a.defendAt[TB]=100;a.maskReadyAt=0;
 nightAIUpdate(&a,&o,5100,9,false,false);assert(a.loc[TB]==3);
 for(unsigned t=6000;t<50000;t+=6000){a.nextMove[TB]=0;nightAIUpdate(&a,&o,t,9,false,false);assert(a.loc[TB]!=1);}
 // Parts & Service: returning Foxy cannot obscure any remaining original.
 reset();a.level[FX]=20;assert(nightCameraFrame(&a,8,true)==0);
 a.loc[WB]=7;assert(nightCameraFrame(&a,8,true)==3);
 a.loc[WC]=4;assert(nightCameraFrame(&a,8,true)==4);
 a.loc[WF]=3;assert(nightCameraFrame(&a,8,true)==5);
 a.loc[FX]=HALL;a.shadow=true;assert(nightCameraFrame(&a,8,true)==6);
 // Shadow is only rolled when lowering the monitor, never on a periodic tick.
 reset();a.loc[WB]=7;a.loc[WC]=4;a.loc[WF]=3;a.loc[FX]=HALL;
 for(unsigned t=1;t<10000;t+=17)nightAIUpdate(&a,&o,t,8,false,false);
 assert(!a.shadow);
 unsigned count=0;for(unsigned i=0;i<10000;i++){
  a.previousMonitor=true;o.monitorFrame=0;o.monitorTarget=false;
  nightAIUpdate(&a,&o,10000+i,8,false,false);count+=a.shadow;
 }
 assert(count>60 && count<170);
 a.loc[FX]=8;a.previousMonitor=true;nightAIUpdate(&a,&o,20001,8,false,false);assert(!a.shadow);
 // A crossing cannot start while a Withered is in the office.
 reset();a.level[WC]=a.level[TC]=20;a.loc[WC]=INSIDE;a.loc[TC]=LEFT_DOOR;
 o.maskFrame=10;nightAIUpdate(&a,&o,100,9,false,false);
 assert(a.defending[WC] && !a.holding[TC] && !a.defending[TC]);
 nightAIUpdate(&a,&o,5100,9,false,false);assert(a.loc[WC]==4);
 nightAIUpdate(&a,&o,5200,9,false,false);assert(a.holding[TC]);
 // Door/duct queues preserve one resident and defer the second attack.
 reset();a.level[TB]=a.level[MG]=20;a.loc[TB]=RIGHT_DOOR;a.loc[MG]=6;
 a.nextMove[MG]=0;nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[MG]==6);
 reset();a.level[TB]=a.level[MG]=20;a.loc[TB]=RIGHT_DOOR;a.loc[MG]=2;
 a.nextMove[MG]=0;nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[MG]==2);
 // Mangle uses hallway before CAM02 and never skips straight to the duct.
 reset();a.level[MG]=20;unsigned mg[]={11,10,7,HALL,2,6,RIGHT_DOOR};
 a.loc[MG]=11;for(unsigned i=1;i<7;i++){a.nextMove[MG]=0;nightAIUpdate(&a,&o,i*100,9,false,false);assert(a.loc[MG]==mg[i]);}
 // Mangle has no old CAM06 overlay; her supplied Kid's Cove still is used on CAM12.
 reset();a.loc[MG]=6;assert(nightCameraFrame(&a,6,true)==7 && nightCameraFrame(&a,6,false)==6);
 a.loc[MG]=12;assert(nightCameraFrame(&a,12,true)==2 && nightCameraFrame(&a,12,false)==1);
 // BB laughs on every move, never visits CAM08; CAM01 queues before the duct.
 reset();a.level[BB]=20;for(unsigned t=100;t<5000;t+=100){a.nextMove[BB]=0;a.bbVoice=0;unsigned prev=a.loc[BB];nightAIUpdate(&a,&o,t,9,false,false);assert(a.loc[BB]!=8);if(prev!=a.loc[BB])assert(a.bbVoice>=1 && a.bbVoice<=3);if(a.loc[BB]==LEFT_DOOR)break;}
 reset();a.level[BB]=a.level[TC]=20;a.loc[BB]=1;a.loc[TC]=LEFT_DOOR;a.nextMove[BB]=0;
 nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[BB]==1 && !a.bbVoice);
 // Night 2 needs ~3.33 seconds of accumulated light, not three button presses.
 reset();a.night=2;o.night=2;a.loc[FX]=HALL;o.centerLight=true;
 nightAIUpdate(&a,&o,1,9,false,false);nightAIUpdate(&a,&o,3001,9,false,false);assert(a.loc[FX]==HALL);
 nightAIUpdate(&a,&o,3335,9,false,false);assert(a.loc[FX]==8);
 // Audio contract: only Toys and BB walk; originals use metal; Mangle has no footstep.
 for(unsigned who=0;who<10;who++)for(unsigned v=0;v<5;v++){
  const char *name=nightMovementAudio(who,v);
  if(who==MG || who==GF)assert(!name);
  else if(who==FX)assert(!strcmp(name,"metalrun.raw"));
  else assert(!strncmp(name,who>=WF?"metalwalk":"walk",who>=WF?9:4));
 }
 // The actual gameplay sound predicate blocks every unmasked stare trigger.
 for(unsigned who=0;who<10;who++){
  reset();a.loc[who]=INSIDE;o.maskFrame=0;assert(!nightStareAllowed(&a,&o,100));
  o.maskFrame=10;assert(nightStareAllowed(&a,&o,100)==(who==TF || who==WF || who==WB || who==WC));
 }
 reset();o.maskFrame=10;a.loc[TB]=RIGHT_DOOR;assert(nightStareAllowed(&a,&o,100));
 reset();o.maskFrame=10;a.loc[TC]=LEFT_DOOR;assert(nightStareAllowed(&a,&o,100));
 return 0;
}
