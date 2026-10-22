#include "night_ai.h"
#include <string.h>
static const uint8_t routes[9][9]={{9,10,7,HALL,INSIDE,0},{9,4,3,2,6,RIGHT_DOOR,0},{9,7,4,HALL,1,5,LEFT_DOOR,0},{12,11,10,7,1,2,6,RIGHT_DOOR,0},{10,5,LEFT_DOOR,0},{8,3,7,HALL,INSIDE,0},{8,7,1,5,INSIDE,0},{8,4,2,6,INSIDE,0},{8,HALL,0}};
static uint32_t randomValue(NightAI *a){a->rng^=a->rng<<13;a->rng^=a->rng>>17;a->rng^=a->rng<<5;return a->rng;}
unsigned nightMaskReactionMilliseconds(unsigned night){static const unsigned ms[]={1667,1333,1000,917,833,833,750};return ms[(night>=1 && night<=7?night:7)-1];}
static bool isWithered(unsigned i){return i==WF || i==WB || i==WC;}
static unsigned maskWindow(const NightAI *a,unsigned i){
 unsigned ms=nightMaskReactionMilliseconds(a->night);
 return isWithered(i) && ms<1500?1500:ms;
}
void nightOfficePresented(NightAI *a,uint64_t now){
 for(unsigned i=WF;i<=WC;i++)if(a->loc[i]==INSIDE && a->awaitingOfficeFrame[i]){
  a->awaitingOfficeFrame[i]=false;a->defendAt[i]=now;a->entered[i]=now;
 }
}
static void retreat(NightAI *a,unsigned i,uint64_t now){
 unsigned step=i==TB?2:i==TC?1:i==WB?1:i==WC?1:0;
 a->step[i]=step;a->loc[i]=i==GF?0:routes[i][step];a->entered[i]=now;
 /* Resume on a fresh randomized movement cycle; AI level/night tables decide
    whether that opportunity succeeds. Foxy has the longer Parts & Service reset. */
 unsigned recovery=i==FX?8300+randomValue(a)%8301:5000+randomValue(a)%5001;
 a->nextMove[i]=now+recovery;a->frozen[i]=now+recovery;
 a->awaitingOfficeFrame[i]=false;
 a->defending[i]=a->holding[i]=a->answered[i]=false;
 if(a->attackOwner==i)a->attackOwner=99;
 if(i<9 && i!=MG && a->pendingMoves[i]<255)++a->pendingMoves[i];
 if(i==BB){a->bbVoice=1+randomValue(a)%3;++a->ventEvents;}
 if(i==FX){a->flashes=0;a->foxyLightMs=0;}
}
static void kill(NightAI *a,Office *o,unsigned i){if(!a->cheats && !o->previewUsed){o->dead=true;a->killer=i;}}
/* Community reverse-engineering: Boxfigs/RBLXComical, original game event tables.
   https://www.reddit.com/r/fivenightsatfreddys/comments/37v6jy/ */
void nightLevels(unsigned night,unsigned hour,const uint8_t *custom,uint8_t *v,unsigned *puppet){
 static const uint8_t initial[6][10]={
 {0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,1,0},
 {0,0,0,0,1,0,1,1,2,0},{0,0,0,5,3,0,1,0,7,0},
 {5,2,2,1,5,2,2,2,5,0},{0,0,0,3,5,5,5,5,10,1}};
 static const unsigned puppets[6]={1,5,8,9,10,15};
 if(night==7){memcpy(v,custom,10);*puppet=15;return;}
 if(night<1 || night>6)night=1;
 memcpy(v,initial[night-1],10);*puppet=puppets[night-1];
 if(night==1){/* Tutorial: only Toy Bonnie, at level 1 from 2 AM onward. */
  if(hour>=2)v[TB]=1;
 }
 if(night==2 && hour>=1){v[TF]=2;v[TB]=v[TC]=v[MG]=v[BB]=3;}
 if(night==3 && hour>=1){v[TB]=v[TC]=1;v[WF]=2;v[WB]=3;v[WC]=2;v[FX]=3;v[BB]=2;}
 if(night==4 && hour>=2){v[TB]=1;v[WF]=3;v[WB]=v[WC]=4;}
 if(night==5 && hour>=1){v[TF]=1;v[MG]=10;v[WF]=v[WB]=v[WC]=5;v[FX]=7;}
 if(night==6 && hour>=2){v[TF]=v[TB]=v[TC]=5;v[MG]=10;v[BB]=9;v[WF]=v[WB]=v[WC]=10;v[FX]=15;v[GF]=3;}
}
void nightAIReset(NightAI *a,unsigned night,const uint8_t *levels,bool cheats,uint32_t seed){
 memset(a,0,sizeof(*a));a->night=night;if(levels)memcpy(a->custom,levels,10);
 nightLevels(night,0,a->custom,a->level,&a->puppetAI);
 a->music=100000;a->rng=seed?seed:0xFA2D51;a->killer=99;a->defender=99;a->attackOwner=99;a->cheats=cheats;a->nextGolden=5000;
 for(unsigned i=0;i<9;i++){a->loc[i]=routes[i][0];a->nextMove[i]=5000+i*150;}
}
static bool occupied(const NightAI *a,unsigned loc,unsigned except){for(unsigned j=0;j<10;j++)if(j!=except && !(j==BB && a->bbDisabled) && a->loc[j]==loc)return true;return false;}
static bool attackBusy(const NightAI *a,unsigned except){
 if(a->attackOwner<10 && a->attackOwner!=except)return true;
 for(unsigned i=0;i<10;i++)if(i!=except && !(i==BB && a->bbDisabled) &&
 (a->loc[i]==INSIDE || a->loc[i]==CEILING || a->defending[i] || a->holding[i]))return true;
 return false;
}
static bool partsEmpty(const NightAI *a){return a->loc[WB]!=8 && a->loc[WC]!=8 && a->loc[WF]!=8 && a->loc[FX]!=8;}
void nightAIUpdate(NightAI *a,Office *o,uint64_t now,unsigned camera,bool wind,bool cameraLight){
 a->hallExitEvent=false;
 if(o->previewUsed || o->dead || o->complete){a->last=now;return;}
 if(a->night>=1 && a->night<=6)nightLevels(a->night,o->hour,a->custom,a->level,&a->puppetAI);
 uint64_t dt=now-a->last;a->last=now;bool monitor=o->monitorFrame==11 && o->monitorTarget;
 if(a->puppetStage<3){
  unsigned denominator=o->night==1?40:10;
  uint64_t drain=(o->night==1?dt:dt*(3+o->night))+a->drainRemainder;
  a->music-=(int)(drain/denominator);a->drainRemainder=drain%denominator;
  if(monitor && camera==11 && wind)a->music+=(int)(dt*14);
  if(a->music>100000)a->music=100000;
  if(a->music<=0){a->music=0;a->puppetStage=3;a->puppetAt=now;}else a->puppetStage=a->music<=15000?2:a->music<=30000?1:0;
 }else if(a->puppetStage==3 && now-a->puppetAt>=1000){a->puppetStage=4;a->puppetAt=now;a->endo=randomValue(a)%40==0;}
 if(monitor && camera!=a->lastCamera && a->puppetStage==4 && randomValue(a)%4==0){a->ghostUntil=now+250;a->ghostFrame=randomValue(a)%2;}
 a->lastCamera=monitor?camera:0;
 if(o->maskFrame==10 && !a->previousMask)a->maskReadyAt=now;
 a->previousMask=o->maskFrame==10;
 if(a->puppetStage==4 && now-a->puppetAt>=16000)kill(a,o,PUPPET);
 if(now>=a->nextGolden){
  a->nextGolden=now+5000;
  if(monitor && a->level[GF] && o->night>=4 && randomValue(a)%100<(o->night==4?1:a->level[GF]))a->goldenPending=true;

 }
 if(!partsEmpty(a))a->shadow=false;
 if(a->previousMonitor && !monitor)a->shadow=partsEmpty(a) && randomValue(a)%1000<11;
 if(a->goldenPending && !o->monitorFrame && !attackBusy(a,GF)){a->loc[GF]=INSIDE;a->entered[GF]=now;a->goldenPending=false;}
 if(monitor && cameraLight && camera==8 && a->shadow){if(!a->shadowAt)a->shadowAt=now;else if(now-a->shadowAt>=2500)kill(a,o,GF);}else a->shadowAt=0;
 bool flash=o->centerLight && !a->bbDisabled;
 if(flash && !a->previousFlash){
  for(unsigned i=0;i<10;i++)if(a->loc[i]==HALL){a->frozen[i]=now+6660;a->entered[i]=now;if(i==MG && randomValue(a)%3==0){retreat(a,i,now);a->hallExitEvent=true;}}
  if(a->loc[GF]==INSIDE)kill(a,o,GF);
 }
 /* Real elapsed exposure, independent of variable rendering frame rate.
    Community event analysis: 100 light frames per night, at 60 Hz. */
 if(a->loc[FX]==HALL && flash){
  if(a->previousFlash)a->foxyLightMs+=(unsigned)dt;
  a->frozen[FX]=now+1000;a->entered[FX]=now;
  if(a->foxyLightMs>=(100000u*a->night+59)/60){retreat(a,FX,now);a->hallExitEvent=true;}
 }
 a->previousFlash=flash;
 if(cameraLight && !a->previousCameraFlash)for(unsigned i=0;i<9;i++)if(a->loc[i]==camera)a->frozen[i]=now+6660;
 a->previousCameraFlash=cameraLight;
 for(unsigned rank=20;rank>0;rank--)for(unsigned i=0;i<10;i++)if(a->level[i]==rank){
  unsigned loc=a->loc[i];
  if(i==BB && a->bbDisabled){o->batteryMilli=0;continue;}
  if(loc==INSIDE || loc==LEFT_DOOR || loc==RIGHT_DOOR){
   if(i==GF){if(o->maskFrame==10 && now-a->entered[i]>=900)retreat(a,i,now);else if(now-a->entered[i]>=1500)kill(a,o,i);continue;}
   bool masked=o->maskFrame==10;
   if((loc==LEFT_DOOR || loc==RIGHT_DOOR) && attackBusy(a,i)){
    a->entered[i]=now;a->holding[i]=false;continue;
   }
   if(loc==INSIDE)a->attackOwner=i;
   if(loc==INSIDE && o->monitorFrame){o->monitorTarget=false;a->entered[i]=now;continue;}
   if(a->defending[i]){
    if(a->awaitingOfficeFrame[i])continue;
    if(masked && a->maskReadyAt<a->defendAt[i]+maskWindow(a,i))a->answered[i]=true;
    if(a->answered[i] && !masked){kill(a,o,i);continue;}
    if(!a->answered[i] && now-a->defendAt[i]>=maskWindow(a,i)){kill(a,o,i);continue;}
    if(a->answered[i] && now-a->defendAt[i]>=5000)retreat(a,i,now);
    continue;
   }
   if(loc==INSIDE){a->defending[i]=true;
    a->defendAt[i]=a->entered[i];
    if(isWithered(i) && a->presentationTracking){a->awaitingOfficeFrame[i]=true;a->defendAt[i]=now;continue;}
    if(masked && a->maskReadyAt<a->entered[i]+maskWindow(a,i))a->answered[i]=true;
    else if(now-a->entered[i]>=maskWindow(a,i))kill(a,o,i);
    continue;
   }
   if(masked){
    if(!a->holding[i]){a->attackOwner=i;a->holding[i]=true;a->holdAt[i]=now;a->cueAt[i]=now+500;}
    if(i==TB){
     if(now>=a->cueAt[i]){a->cueAt[i]=now+500;if(randomValue(a)%2){a->defending[i]=a->answered[i]=true;a->defendAt[i]=now;a->defender=i;a->defenseUntil=now+5000;}}
    }else if(now-a->holdAt[i]>=5000){
     if(i==TC){/* Requested Toy Chica crossing, specific to this mod. */
      a->defending[i]=a->answered[i]=true;a->defendAt[i]=now;a->defender=i;a->defenseUntil=now+5000;
     }else retreat(a,i,now);
    }
   }else{a->holding[i]=false;if(a->attackOwner==i)a->attackOwner=99;
    unsigned opening=i==TB?(1000-100*a->night)*1000/60:i==TC?6000:3500;
    if(now-a->entered[i]>=opening){if(i==BB){a->bbDisabled=true;a->loc[BB]=INSIDE;o->batteryMilli=0;}else if(i==MG){a->attackOwner=i;a->loc[i]=CEILING;a->entered[i]=now;}else kill(a,o,i);}
   }
   continue;
  }
  if(loc==CEILING){if(now-a->entered[i]>=8000)kill(a,o,i);continue;}
  if(i==FX && loc==HALL){if(now>=a->frozen[i] && now-a->entered[i]>=8000)kill(a,o,i);continue;}
  if(i==GF || now<a->nextMove[i] || now<a->frozen[i])continue;
  a->nextMove[i]=now+(a->night==1?15000:5000);if(randomValue(a)%20>=a->level[i])continue;
  if(i==TF && loc==9 && (a->loc[TB]==9 || a->loc[TC]==9))continue;
  if(i==TC && loc==9 && a->loc[TB]==9)continue;
  unsigned next=0;
  if(i==MG){switch(loc){case 12:next=randomValue(a)%2?9:11;break;case 9:case 11:next=10;break;case 10:next=7;break;case 7:next=HALL;break;case HALL:next=2;break;case 2:next=6;break;case 6:next=RIGHT_DOOR;break;default:break;}}
  else if(i==BB){
   if(loc==1)next=5;else if(loc==5)next=LEFT_DOOR;
   else{static const uint8_t rooms[]={1,2,3,4,7,9,10,11,12,HALL};next=rooms[randomValue(a)%10];if(next==loc)next=1;}
  }else if(a->step[i]<8)next=routes[i][a->step[i]+1];
  /* Toy Bonnie's route never includes CAM01, including after a masked retreat. */
  if(i==TB && next==1)continue;
  if(i==FX && next==HALL){
   bool hallwayBlocked=false;
   /* Office frames 19 and 20 show Foxy with Withered Bonnie and Mangle. */
   for(unsigned j=0;j<10;j++)if(j!=FX && j!=MG && j!=WB && a->loc[j]==HALL && !(j==BB && a->bbDisabled)){hallwayBlocked=true;break;}
   if(hallwayBlocked)continue;
  }
  if(!next)continue;
  if((next==5 || next==6 || next>=LEFT_DOOR) && !(i==FX && next==HALL) && occupied(a,next,i))continue;
  if((next==5 && occupied(a,LEFT_DOOR,i)) || (next==6 && occupied(a,RIGHT_DOOR,i)))continue;
  if((next==INSIDE || next==CEILING) && attackBusy(a,i))continue;
  if(monitor && (loc==camera || next==camera))a->signalUntil=now+500;
  if(next==5 || next==6 || ((loc==5 || loc==6) && (next==INSIDE || next==LEFT_DOOR || next==RIGHT_DOOR)))++a->ventEvents;
  if((next==INSIDE && (i==TF || i==WF || i==WB || i==WC)) || ((next==LEFT_DOOR || next==RIGHT_DOOR) && (i==TB || i==TC)))a->stareEvent=true;
  if(next==HALL)a->hallEvent=true;
  if(loc==HALL && next!=HALL && flash)a->hallExitEvent=true;
  if(i!=MG && a->pendingMoves[i]<255)++a->pendingMoves[i];
  if(i==MG)a->mangleVisible=true;
  a->awaitingOfficeFrame[i]=false;
 a->defending[i]=a->holding[i]=a->answered[i]=false;if(i!=BB && i!=MG)a->step[i]++;a->loc[i]=next;a->entered[i]=now;
  if(i==TB)a->bonnieMoved=true;
  if(i==BB)a->bbVoice=1+randomValue(a)%3;
 }
 o->bbBlocked=a->bbDisabled;if(a->bbDisabled){o->batteryMilli=0;o->centerLight=o->leftLight=o->rightLight=false;}
 o->invasion=o->corridor=o->leftVent=o->rightVent=0;unsigned best=0;
 for(unsigned i=0;i<10;i++)if(i!=BB && a->loc[i]==INSIDE && a->level[i]>=best){best=a->level[i];o->invasion=i==WB?2:i==WC?3:i==WF?1:0;}
 best=0;for(unsigned i=0;i<10;i++)if(a->loc[i]==HALL && a->level[i]>=best){best=a->level[i];o->corridor=i==FX?7:i==WF?6:i==TF?(now-a->entered[i]>=5000?4:3):i==MG?2:1;}
 if(a->loc[FX]==HALL && a->loc[MG]==HALL)o->corridor=10;
 if(a->loc[FX]==HALL && a->loc[WB]==HALL)o->corridor=9;
 if(a->loc[TC]==LEFT_DOOR)o->leftVent=1;
 if(a->loc[BB]==LEFT_DOOR)o->leftVent=2;
 if(a->loc[TB]==RIGHT_DOOR)o->rightVent=1;
 if(a->loc[MG]==RIGHT_DOOR || a->loc[MG]==CEILING)o->rightVent=2;
 a->previousMonitor=monitor;
}
unsigned nightCameraFrame(const NightAI *a,unsigned c,bool light){
 if(!light){switch(c){case 2:return a->loc[WC]==2?3:0;case 3:return a->loc[WF]==3?3:0;case 4:return a->loc[TB]==4?4:0;case 6:return 6;case 7:return a->loc[TC]==7?4:0;case 8:return 1;case 9:{bool f=a->loc[TF]==9,b=a->loc[TB]==9,ch=a->loc[TC]==9;if(f&&b&&ch)return 0;if(f&&(b||ch))return 2;if(f)return 4;return 6;}case 10:return a->loc[BB]==10?0:2;case 11:return 4;case 12:return 1;default:return 0;}}
 switch(c){
 case 1:return a->loc[TC]==1?2:a->loc[WB]==1?3:1;
 case 2:return a->loc[WC]==2?4:a->loc[TB]==2?5:1;
 case 3:return a->loc[WF]==3?4:a->loc[TB]==3?5:1;
 case 4:return a->loc[TB]==4?5:a->loc[TC]==4?6:a->loc[WC]==4?7:a->bonnieMoved?2:1;
 case 5:return a->loc[TC]==5?2:a->loc[BB]==5?3:a->loc[WB]==5?4:a->endo?5:1;
 case 6:return a->loc[TB]==6?8:a->loc[WC]==6?10:7;
 case 7:return a->loc[TC]==7?5:a->loc[WF]==7?3:a->loc[WB]==7?2:1;
 case 8:
  /* The original three Withereds share the standard illuminated frame at
     the start of the night; reserve their individual frames for later states. */
  if(a->loc[WB]==8 && a->loc[WC]==8 && a->loc[WF]==8)return 0;
  if(a->loc[WB]==8)return 2;
  if(a->loc[WC]==8)return 3;
  if(a->loc[WF]==8)return 4;
  return a->loc[FX]==8?5:a->shadow?6:0;
 case 9:{bool f=a->loc[TF]==9,b=a->loc[TB]==9,ch=a->loc[TC]==9;if(f&&b&&ch)return 1;if(f&&(b||ch))return 3;if(f)return 5;return 6;}
 case 10:return a->loc[BB]==10?(a->loc[TF]==10?4:1):(a->loc[TF]==10?5:3);
 case 11:return a->endo?5:a->puppetStage<4?a->puppetStage:0;
 /* CAM12 has three source states: frame 0 is lit and empty, frame 1 is
    dark, and frame 2 is lit with Mangle. */
 case 12:return light?(a->loc[MG]==12?2:0):1;
 default:return 0;
 }
}

const char *nightMovementAudio(unsigned who,unsigned variation){
 static const char *walk[]={"walk1.raw","walk2.raw","walk3.raw","walk4.raw","walk5.raw"};
 static const char *metal[]={"metalwalk1.raw","metalwalk2.raw","metalwalk3.raw"};
 if(who==TF || who==TB || who==TC || who==BB)return walk[variation%5];
 if(who==WF || who==WB || who==WC)return metal[variation%3];
 if(who==FX)return "metalrun.raw";
 return NULL;
}
bool nightStareAllowed(const NightAI *a,const Office *o,uint64_t now){
 return o->maskFrame==10 && (a->loc[TF]==INSIDE || a->loc[WF]==INSIDE || a->loc[WB]==INSIDE || a->loc[WC]==INSIDE || a->loc[TB]==RIGHT_DOOR || a->loc[TC]==LEFT_DOOR || (now<a->defenseUntil && (a->defender==TB || a->defender==TC)));
}
bool nightHallwayAudioActive(const NightAI *a,const Office *o){
 if(o->dead || o->complete)return false;
 for(unsigned i=0;i<10;i++)if(a->loc[i]==HALL)return true;
 return false;
}
int nightBonnieMaskX(unsigned elapsedMilliseconds){
 if(elapsedMilliseconds>5000)elapsedMilliseconds=5000;
 return 256-(int)(elapsedMilliseconds*384u/5000u);
}
bool nightFootstepsAudible(bool cameraMode,unsigned camera){
 return cameraMode && camera>=1 && camera<=4;
}
