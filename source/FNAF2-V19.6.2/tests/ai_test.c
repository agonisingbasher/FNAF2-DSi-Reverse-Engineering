#include "night_ai.h"
#include <assert.h>
#include <string.h>
static Office o;static NightAI a;static uint8_t levels[10];
static void reset(void){officeReset(&o,7);memset(levels,0,10);nightAIReset(&a,7,levels,false,123);}
static void intruder(unsigned who,unsigned where){reset();a.level[who]=20;a.loc[who]=where;a.entered[who]=100;a.last=100;}
int main(void){
 reset();for(unsigned t=0;t<=50000;t+=17)nightAIUpdate(&a,&o,t,9,false,false);assert(a.loc[TF]==9 && a.loc[BB]==10 && a.loc[WF]==8 && !o.dead);
 for(unsigned cam=1;cam<=12;cam++){reset();a.music=50000;o.monitorFrame=11;o.monitorTarget=true;nightAIUpdate(&a,&o,1000,cam,true,false);assert(cam==11?a.music>50000:a.music<50000);}
 reset();a.music=50000;nightAIUpdate(&a,&o,1000,11,true,false);assert(a.music<50000);
 reset();a.music=100;nightAIUpdate(&a,&o,1000,12,false,false);assert(a.puppetStage==3 && a.music==0);o.monitorFrame=11;o.monitorTarget=true;nightAIUpdate(&a,&o,2000,11,true,false);assert(a.puppetStage==4 && a.music==0);nightAIUpdate(&a,&o,18000,11,true,false);assert(o.dead && a.killer==PUPPET);
 const unsigned reaction[]={1667,1333,1000,917,833,833,750};
 for(unsigned n=1;n<=7;n++)assert(nightMaskReactionMilliseconds(n)==reaction[n-1]);
 intruder(WB,INSIDE);o.maskFrame=10;nightAIUpdate(&a,&o,849,9,false,false);assert(!o.dead && a.loc[WB]==INSIDE);nightAIUpdate(&a,&o,5100,9,false,false);assert(!o.dead && a.loc[WB]==7 && a.nextMove[WB]>=10100 && a.nextMove[WB]<=15100);
 intruder(WB,INSIDE);o.maskFrame=10;nightAIUpdate(&a,&o,1600,9,false,false);assert(o.dead && a.killer==WB);
 intruder(TF,INSIDE);o.maskFrame=10;nightAIUpdate(&a,&o,200,9,false,false);o.maskFrame=8;nightAIUpdate(&a,&o,500,9,false,false);assert(o.dead && a.killer==TF);
 intruder(TB,RIGHT_DOOR);o.maskFrame=10;unsigned cue=0;for(unsigned t=1000;t<20000;t+=500){nightAIUpdate(&a,&o,t,9,false,false);assert(a.loc[TB]==RIGHT_DOOR && !o.dead);if(a.defending[TB]){cue=t;break;}}assert(cue && a.defender==TB && a.defenseUntil==cue+5000);nightAIUpdate(&a,&o,cue+4999,9,false,false);assert(a.loc[TB]==RIGHT_DOOR);nightAIUpdate(&a,&o,cue+5000,9,false,false);assert(a.loc[TB]==3 && a.nextMove[TB]>=cue+10000 && a.nextMove[TB]<=cue+15000 && !o.dead);
 intruder(TC,LEFT_DOOR);o.maskFrame=10;nightAIUpdate(&a,&o,1000,9,false,false);nightAIUpdate(&a,&o,6000,9,false,false);assert(a.defending[TC] && a.loc[TC]==LEFT_DOOR);nightAIUpdate(&a,&o,11000,9,false,false);assert(a.loc[TC]==7 && !o.dead);
 intruder(BB,LEFT_DOOR);o.maskFrame=10;nightAIUpdate(&a,&o,1000,9,false,false);o.maskFrame=0;nightAIUpdate(&a,&o,1100,9,false,false);o.maskFrame=10;nightAIUpdate(&a,&o,1200,9,false,false);nightAIUpdate(&a,&o,6100,9,false,false);assert(a.loc[BB]==LEFT_DOOR);nightAIUpdate(&a,&o,6200,9,false,false);assert(a.loc[BB]==10 && !a.bbDisabled);
 intruder(WF,INSIDE);o.monitorFrame=11;o.monitorTarget=true;nightAIUpdate(&a,&o,3000,9,false,false);assert(!o.monitorTarget && !o.dead);o.monitorFrame=0;o.maskFrame=10;nightAIUpdate(&a,&o,3100,9,false,false);assert(!o.dead);
 intruder(BB,LEFT_DOOR);o.centerLight=o.leftLight=o.rightLight=true;nightAIUpdate(&a,&o,3600,9,false,false);assert(a.loc[BB]==INSIDE && o.batteryMilli==0 && !o.dead && a.bbDisabled && !o.centerLight && !o.leftLight && !o.rightLight);
 intruder(MG,RIGHT_DOOR);nightAIUpdate(&a,&o,3600,9,false,false);assert(a.loc[MG]==CEILING);nightAIUpdate(&a,&o,11600,9,false,false);assert(o.dead && a.killer==MG);
 intruder(FX,HALL);o.centerLight=true;for(unsigned t=100;t<=11767;t+=17)nightAIUpdate(&a,&o,t,9,false,false);assert(a.loc[FX]==HALL && !o.dead);nightAIUpdate(&a,&o,11784,9,false,false);assert(a.loc[FX]==8 && !o.dead);
 intruder(FX,HALL);o.maskFrame=10;nightAIUpdate(&a,&o,8100,9,false,false);assert(o.dead && a.killer==FX);
 intruder(GF,INSIDE);o.centerLight=true;nightAIUpdate(&a,&o,101,9,false,false);assert(o.dead && a.killer==GF);
 reset();a.level[WB]=20;a.loc[WB]=1;nightAIUpdate(&a,&o,1000,1,false,true);assert(a.frozen[WB]==7660);nightAIUpdate(&a,&o,2000,1,false,true);assert(a.frozen[WB]==7660);
 reset();for(unsigned i=0;i<10;i++)a.level[i]=20;for(unsigned t=0;t<=14000;t+=100){nightAIUpdate(&a,&o,t,9,false,false);if(a.loc[TF]!=9)assert(a.loc[TB]!=9 && a.loc[TC]!=9);if(a.loc[TC]!=9)assert(a.loc[TB]!=9);}assert(a.loc[TB]!=9 && a.loc[TC]!=9 && !o.dead);
 reset();a.level[WB]=20;a.loc[WB]=5;a.step[WB]=3;a.nextMove[WB]=0;a.level[WC]=20;a.loc[WC]=INSIDE;a.entered[WC]=1000;nightAIUpdate(&a,&o,1000,9,false,false);assert(a.loc[WB]==5);
 reset();a.loc[TF]=INSIDE;a.level[TF]=20;nightAIUpdate(&a,&o,10,9,false,false);assert(o.invasion==0);
 reset();a.level[WF]=20;a.loc[WF]=INSIDE;a.cheats=true;nightAIUpdate(&a,&o,1600,9,false,false);assert(!o.dead);
 reset();for(unsigned t=1;t<=1000;t++)nightAIUpdate(&a,&o,t,9,false,false);assert(a.music==99000);
 reset();o.monitorTarget=true;o.monitorFrame=11;o.cameraLight=true;officeTick(&o,true,false,false,true,true);assert(!o.centerLight && !o.leftLight && !o.rightLight && o.cameraLight);
 reset();a.loc[MG]=12;assert(nightCameraFrame(&a,12,false)==1 && nightCameraFrame(&a,12,true)==2);a.loc[MG]=9;assert(nightCameraFrame(&a,12,false)==1 && nightCameraFrame(&a,12,true)==0);
 reset();a.loc[TF]=a.loc[TB]=a.loc[TC]=9;assert(nightCameraFrame(&a,9,false)==0 && nightCameraFrame(&a,9,true)==1);
 a.loc[TB]=HALL;assert(nightCameraFrame(&a,9,false)==2 && nightCameraFrame(&a,9,true)==3);
 a.loc[TC]=HALL;assert(nightCameraFrame(&a,9,false)==4 && nightCameraFrame(&a,9,true)==5);
 a.loc[TF]=HALL;assert(nightCameraFrame(&a,9,false)==6 && nightCameraFrame(&a,9,true)==6);
 const unsigned masks[12]={0xF,0x3B,0x3B,0xF7,0x3F,0x7C0,0x3F,0x7F,0x7F,0x3F,0x3F,0x7};unsigned seed=1789;
 for(unsigned trial=0;trial<10000;trial++){for(unsigned i=0;i<10;i++){seed=seed*1664525+1013904223;a.loc[i]=seed%18;a.level[i]=(seed>>8)%21;}a.endo=trial%2;a.shadow=(trial/2)%2;a.bonnieMoved=(trial/4)%2;a.puppetStage=trial%5;for(unsigned cam=1;cam<=12;cam++)for(unsigned light=0;light<2;light++)assert(masks[cam-1]&(1u<<nightCameraFrame(&a,cam,light)));}
 unsigned puppet;uint8_t v[10];
 nightLevels(1,0,levels,v,&puppet);for(unsigned i=0;i<10;i++)assert(v[i]==0);assert(puppet==1);
 nightLevels(1,1,levels,v,&puppet);assert(v[TB]==0 && v[TC]==0);
 nightLevels(1,2,levels,v,&puppet);assert(v[TB]==1 && v[TC]==0 && v[TF]==0);
 nightLevels(1,3,levels,v,&puppet);assert(v[TB]==1 && v[TF]==0 && v[TC]==0);
 for(unsigned h=0;h<6;h++){nightLevels(1,h,levels,v,&puppet);assert(!v[WF] && !v[WB] && !v[WC] && !v[FX]);}
 nightLevels(2,1,levels,v,&puppet);assert(v[TF]==2 && v[TB]==3 && v[MG]==3 && v[FX]==1 && !v[WB]);
 nightLevels(3,1,levels,v,&puppet);assert(v[WB]==3 && v[WC]==2 && v[WF]==2 && v[FX]==3);
 nightLevels(4,2,levels,v,&puppet);assert(v[WB]==4 && v[WF]==3 && v[TC]==0);
 nightLevels(5,1,levels,v,&puppet);assert(v[TF]==1 && v[MG]==10 && v[WF]==5);
 nightLevels(6,2,levels,v,&puppet);assert(v[FX]==15 && v[BB]==9 && v[GF]==3);
 officeReset(&o,1);nightAIReset(&a,1,levels,false,111);assert(nightCameraFrame(&a,8,true)==0 && nightCameraFrame(&a,8,false)==1);
 for(unsigned t=0;t<178000;t+=100){o.hour=t/89000;nightAIUpdate(&a,&o,t,8,false,false);assert(a.loc[TB]==9 && a.loc[TC]==9 && a.loc[TF]==9);}
 reset();a.level[TB]=20;a.loc[TB]=2;a.step[TB]=3;a.nextMove[TB]=0;nightAIUpdate(&a,&o,100,9,false,false);assert(a.loc[TB]==6 && a.ventEvents==1);nightAIUpdate(&a,&o,5099,9,false,false);assert(a.loc[TB]==6 && a.ventEvents==1);nightAIUpdate(&a,&o,5100,9,false,false);assert(a.loc[TB]==RIGHT_DOOR && a.ventEvents==2);
 intruder(BB,LEFT_DOOR);nightAIUpdate(&a,&o,3600,9,false,false);o.maskFrame=10;nightAIUpdate(&a,&o,5000,9,false,false);assert(a.loc[BB]==INSIDE && a.bbDisabled && !o.dead && o.batteryMilli==0);
 // Latest author correction: Chica visits Main Hall, Party 4, hallway, Party 1, left vent.
 reset();a.level[TC]=20;a.loc[TB]=4;const unsigned chicaRoute[]={7,4,HALL,1,5,LEFT_DOOR};
 for(unsigned i=0;i<6;i++){a.nextMove[TC]=0;nightAIUpdate(&a,&o,100+i*5000,9,false,false);assert(a.loc[TC]==chicaRoute[i]);if(i==2){assert(a.hallEvent && !a.stareEvent);a.hallEvent=false;}if(i==5)assert(a.stareEvent);}
 for(unsigned who=0;who<9;who++){reset();a.level[who]=20;if(who==MG){a.loc[MG]=6;a.step[MG]=5;}else if(who==BB){a.loc[BB]=5;a.step[BB]=1;}else if(who==FX){a.loc[FX]=8;a.step[FX]=0;}else if(who==TF){a.loc[TF]=HALL;a.step[TF]=3;}else if(who==TB){a.loc[TB]=6;a.step[TB]=4;}else if(who==TC){a.loc[TC]=5;a.step[TC]=5;}else {a.loc[who]=who==WF?HALL:who==WB?5:6;a.step[who]=3;}a.nextMove[who]=0;nightAIUpdate(&a,&o,100,9,false,false);assert(a.stareEvent==(who==TF || who==TB || who==TC || who==WF || who==WB || who==WC));}
 return 0;
}
