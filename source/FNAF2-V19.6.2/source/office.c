#include "loading.h"
#include <nds.h>
#include <filesystem.h>
#include <calico/system/tick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "office.h"
#include "office_audio.h"
#include "office_panel.h"
#include "call_stream.h"
#include "ambience_state.h"
static AmbienceState ambienceChoice;
#include "camera_state.h"
#include "night_ai.h"
#include "camera_assets.h"
static Office room;
static u32 nightTimerStart;
#ifdef DEV_BUILD
static bool debugPaused;
static u32 debugPauseTick;
#endif
static u32 readNightTimer(void) {
 u16 high,low,again;
 do {high=TIMER_DATA(1);low=TIMER_DATA(0);again=TIMER_DATA(1);}while(high!=again);
 return ((u32)high<<16)|low;
}
static void startNightTimer(void) {
 /* ARM9 timers 0/1 are free here. Calico owns timers 2/3. */
 TIMER_CR(0)=0;TIMER_CR(1)=0;
 TIMER_DATA(0)=0;TIMER_DATA(1)=0;
 TIMER_CR(1)=TIMER_ENABLE|TIMER_CASCADE;
 TIMER_CR(0)=TIMER_ENABLE|ClockDivider_1024;
 nightTimerStart=readNightTimer();
#ifdef DEV_BUILD
 debugPaused=false;
#endif
}
u64 officeElapsedMilliseconds(void) {
 u32 now=readNightTimer();
#ifdef DEV_BUILD
 if(debugPaused)now=debugPauseTick;
#endif
 return officeCounterMilliseconds((u32)(now-nightTimerStart),BUS_CLOCK);
}
static bool nitroReady=false, initialized=false;
static u16 scene[400*192] __attribute__((aligned(32)));
static u16 screen[256*192] __attribute__((aligned(32)));
static u16 overlay[256*192];
static u16 desk[4][213*109];
static u16 toys[8][28*32],specialToy[48*18];
_Static_assert(sizeof(desk)>=(115*180+139*192)*sizeof(u16),"Desk buffer must fit cutscene sprites");
static u16 lightButtons[4][15*23], batteries[5][40*18];
static int cachedScene=-1,cachedOverlay=-1,cachedPanel=-1;
static CallStream phoneCall;
static int callChannel=-1;
static CameraState camera;
static NightAI ai;
typedef struct {CallStream stream;int channel;const char *name;unsigned rate;} LoopSound;
static LoopSound ambienceLoop={.channel=-1},boxLoop={.channel=-1},mangleLoop={.channel=-1},hallStream={.channel=-1},stareStream={.channel=-1},movementStream={.channel=-1};
/* Camera stream starts in BSS; loopSet/officeClose initialize its channel. */
static LoopSound cameraLoop;
static bool stareActive,endCrowd;static bool bbLaughStarted;static unsigned bbLaughBytes;
static u8 fxBuffer[16384] __attribute__((aligned(32)));
static int fxChannel=-1;
static u8 *hallLoopBuffer;static unsigned hallLoopBytes;static int hallLoopChannel=-1;
static int popChannel=-1;
static bool deathStarted,cameraMapVisible;
static unsigned panelExitNoise=6;
static unsigned deathFrame;
static u64 deathShownAt;
static u8 cameraNoise[4][6144];
static u16 camBoxes[2][29*16];static u8 camNumbers[12][(20*13+7)/8],camNames[12][(132*12+7)/8];
static u16 camWheels[21][11*11],camWind[47*14],camWindBoxes[2][62*20],camRec[8*8],camAlert[14*13],camFlash[2][33*20],camSignal[136*10];
static bool deskBorrowed;
static u16 actorSprite[128*128];static int cachedActor=-1;
enum { FAN, BUZZ, ERROR_SOUND, HORN, BREATH, MASK_ON, MASK_OFF, MONITOR_ON, MONITOR_OFF, AMBIENCE, WIND_SOUND, VENT_SOUND, BB_ERROR, AUDIO_COUNT };
static void *audio[AUDIO_COUNT];static unsigned audioBytes[AUDIO_COUNT];
static int channels[AUDIO_COUNT]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static const unsigned audioRates[]={8000,11025,11025,11025,8000,11025,11025,11025,11025,4000,8000,8000,8000};
static const char *audioNames[]={"fan","click","error","horn","breath","mask_on","mask_off","monitor_on","monitor_off","ambience","wind","vent","bb_error"};
static FILE *openResource(const char *name) {
 char path[128];
 if(nitroReady){snprintf(path,sizeof(path),"nitro:/%s",name);FILE *f=fopen(path,"rb");if(f)return f;}
 snprintf(path,sizeof(path),"/FNAF2-ASSETS/%s",name);return fopen(path,"rb");
}
static bool loadRaw(const char *name,void *target,unsigned bytes){FILE *f=openResource(name);if(!f)return false;bool ok=loadingRead(target,1,bytes,f)==bytes;fclose(f);return ok;}
static bool loadCameraUI(void){
 char name[48];
 for(unsigned i=0;i<2;i++){snprintf(name,sizeof(name),"cam_box_%u.u16",i);if(!loadRaw(name,camBoxes[i],sizeof(camBoxes[i])))return false;snprintf(name,sizeof(name),"cam_wind_box_%u.u16",i);if(!loadRaw(name,camWindBoxes[i],sizeof(camWindBoxes[i])))return false;snprintf(name,sizeof(name),"cam_flash_%u.u16",i);if(!loadRaw(name,camFlash[i],sizeof(camFlash[i])))return false;}
 for(unsigned i=0;i<12;i++){
  u16 pixels[132*12];snprintf(name,sizeof(name),"cam_number_%02u.u16",i+1);if(!loadRaw(name,pixels,20*13*2))return false;
  for(unsigned p=0;p<20*13;p++)if(pixels[p]&BIT(15))camNumbers[i][p>>3]|=1u<<(p&7);
  memset(pixels,0,sizeof(pixels));snprintf(name,sizeof(name),"cam_name_%02u.u16",i+1);if(!loadRaw(name,pixels,sizeof(pixels)))return false;
  for(unsigned p=0;p<132*12;p++)if(pixels[p]&BIT(15))camNames[i][p>>3]|=1u<<(p&7);
 }
 for(unsigned i=0;i<21;i++){snprintf(name,sizeof(name),"cam_wheel_%02u.u16",i);if(!loadRaw(name,camWheels[i],sizeof(camWheels[i])))return false;}
 return loadRaw("cam_signal.u16",camSignal,sizeof(camSignal)) && loadRaw("cam_wind.u16",camWind,sizeof(camWind)) && loadRaw("cam_rec.u16",camRec,sizeof(camRec)) && loadRaw("cam_alert.u16",camAlert,sizeof(camAlert)) && loadRaw("camera_noise.raw",cameraNoise,sizeof(cameraNoise));
}
static bool loadImage(const char *name,u16 *target,unsigned count) {
 char path[64];snprintf(path,sizeof(path),"%s.bin",name);FILE *f=openResource(path);if(!f)return false;
 u16 palette[256];u8 pixels[1024];
 bool ok=loadingRead(palette,2,256,f)==256;
 for(unsigned at=0;ok && at<count;) {
  unsigned n=count-at;if(n>sizeof(pixels))n=sizeof(pixels);
  if(loadingRead(pixels,1,n,f)!=n){ok=false;break;}
  for(unsigned i=0;i<n;i++)target[at+i]=palette[pixels[i]];
  at+=n;
 }
 fclose(f);return ok;
}
static void loadAudio(unsigned i) {
 char name[32];snprintf(name,sizeof(name),"%s.raw",audioNames[i]);FILE *f=openResource(name);if(!f)return;
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return;}long n=ftell(f);rewind(f);
 if(n<=0 || n>1024*1024 || n%4){fclose(f);return;}
 audio[i]=malloc(n);if(audio[i] && loadingRead(audio[i],1,n,f)==(size_t)n){audioBytes[i]=n;DC_FlushRange(audio[i],n);}else{free(audio[i]);audio[i]=NULL;}
 fclose(f);
}
/* hall.raw is the existing 8 kHz PCM conversion of With_S2 (1).wav.
   Play the complete bounded sample as a hardware loop: no light trigger,
   no restart at every frame and no streaming silence at EOF. */
static bool loadHallLoop(void) {
 if(hallLoopBuffer)return true;
 FILE *f=openResource("hall.raw");if(!f)return false;
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return false;}
 long n=ftell(f);rewind(f);
 if(n<=0 || n>1024*1024 || n%4){fclose(f);return false;}
 hallLoopBuffer=malloc(n);
 if(!hallLoopBuffer || loadingRead(hallLoopBuffer,1,n,f)!=(size_t)n){free(hallLoopBuffer);hallLoopBuffer=NULL;fclose(f);return false;}
 fclose(f);hallLoopBytes=(unsigned)n;DC_FlushRange(hallLoopBuffer,hallLoopBytes);return true;
}
static void stopHallLoop(void) {if(hallLoopChannel>=0)soundKill(hallLoopChannel);hallLoopChannel=-1;}
static void updateHallLoop(bool active) {
 if(!active){stopHallLoop();return;}
 if(hallLoopChannel>=0 && !(soundGetActiveChannels()&(1u<<hallLoopChannel)))hallLoopChannel=-1;
 if(hallLoopChannel<0 && hallLoopBuffer)hallLoopChannel=soundPlaySample(hallLoopBuffer,SoundFormat_8Bit,hallLoopBytes,8000,90,64,true,0);
}
static void stopAudio(unsigned i){if(channels[i]>=0)soundKill(channels[i]);channels[i]=-1;}
static void playAudio(unsigned i,bool loop,unsigned volume) {
 if(!audio[i])return;
 stopAudio(i);channels[i]=soundPlaySample(audio[i],SoundFormat_8Bit,audioBytes[i],audioRates[i],volume,64,loop,0);
}
static void playPopstatic(void) {
 if(!audio[ERROR_SOUND])return;
 if(popChannel>=0)soundKill(popChannel);
 popChannel=soundPlaySample(audio[ERROR_SOUND],SoundFormat_8Bit,audioBytes[ERROR_SOUND],audioRates[ERROR_SOUND],100,64,false,0);
}
static void stopCallChannel(void) {
 if(callChannel>=0)soundKill(callChannel);
 callChannel=-1;
}
static void stopCall(void) {
 stopCallChannel();callStreamClose(&phoneCall);room.callActive=false;
}
static void playCallBuffer(void) {
 DC_FlushRange(phoneCall.ring,CALL_BUFFER);
 callChannel=soundPlaySample(phoneCall.ring,SoundFormat_8Bit,CALL_BUFFER,CALL_RATE,110,64,true,0);
 phoneCall.startMilliseconds=officeElapsedMilliseconds();
 if(callChannel<0)stopCall();else room.callActive=true;
}
static void beginCall(unsigned night) {
 stopCall();
 if(night<1 || night>6)return;
 char name[32];snprintf(name,sizeof(name),"call_%u.raw",night);
 if(callStreamOpen(&phoneCall,openResource(name),officeElapsedMilliseconds()))playCallBuffer();
}
static void updateCall(unsigned activeChannels) {
 if(room.muted){stopCall();return;}
 if(!phoneCall.active)return;
 if(callChannel<0 || (officeElapsedMilliseconds()-phoneCall.startMilliseconds>100 && !(activeChannels&(1u<<callChannel)))) {
  stopCall();return;
 }
 uint32_t block=phoneCall.lastBlock;
 CallResult result=callStreamUpdate(&phoneCall,officeElapsedMilliseconds());
 if(result==CALL_UNDERRUN) {
  stopCallChannel();
  if(callStreamResume(&phoneCall,officeElapsedMilliseconds()))playCallBuffer();else stopCall();
 } else if(result!=CALL_RUNNING)stopCall();
 else if(block!=phoneCall.lastBlock)DC_FlushRange(phoneCall.ring,CALL_BUFFER);
 room.callActive=phoneCall.active;
}
static void blit(const u16 *src,unsigned w,unsigned h,int x,int y);
#include "camera_audio.inc"
bool officeOpen(unsigned night) {
 panelExitNoise=6;cameraLoop.channel=-1;popChannel=-1;officeReset(&room,night);cameraReset(&camera);deathStarted=false;bbLaughStarted=false;deathFrame=0;cachedScene=cachedOverlay=cachedPanel=cachedActor=-1;cameraMapVisible=false;
 if(!initialized) {
  nitroReady=nitroFSInit(NULL);
  char name[32];
  for(unsigned i=0;i<4;i++){snprintf(name,sizeof(name),"desk_%u",i);if(!loadImage(name,desk[i],213*109))return false;}
  for(unsigned i=0;i<8;i++){snprintf(name,sizeof(name),"plush_%u",i);loadImage(name,toys[i],28*32);}
  loadImage("plush_8",specialToy,48*18);
  for(unsigned i=0;i<4;i++){snprintf(name,sizeof(name),"light_%u",i);loadImage(name,lightButtons[i],15*23);}
  for(unsigned i=0;i<5;i++){snprintf(name,sizeof(name),"battery_%u",i);loadImage(name,batteries[i],40*18);}
  for(unsigned i=0;i<AUDIO_COUNT;i++)if(i!=AMBIENCE)loadAudio(i);
  if(!loadCameraUI())return false;
  initialized=true;
 }
 if(!loadHallLoop())return false;
 if(deskBorrowed){for(unsigned i=0;i<4;i++){char name[32];snprintf(name,sizeof(name),"desk_%u",i);if(!loadImage(name,desk[i],213*109))return false;}deskBorrowed=false;}
 if(!loadImage("office_02",scene,400*192))return false;
 cachedScene=2;
 soundEnable();soundSetMixerVolume(127);playAudio(FAN,true,60);startNightTimer();ambienceReset(&ambienceChoice,readNightTimer()^0x51395u^(unsigned)rand());updateOfficeAmbience();officeConfigureAI(NULL,false);beginCall(night);return true;
}
unsigned officeLoadedAudioMask(void){unsigned bits=0;for(unsigned i=0;i<AUDIO_COUNT;i++)if(audio[i] || (i==AMBIENCE && ambienceLoop.stream.active))bits|=1u<<i;return bits;}
void officeSetRewards(uint16_t mask){room.rewards=mask&511;}
void officeClose(void){stopHallLoop();free(hallLoopBuffer);hallLoopBuffer=NULL;hallLoopBytes=0;stopCall();if(cameraLoop.name)loopStop(&cameraLoop);else cameraLoop.channel=-1;stareActive=false;loopStop(&ambienceLoop);loopStop(&boxLoop);loopStop(&mangleLoop);loopStop(&hallStream);loopStop(&stareStream);loopStop(&movementStream);if(fxChannel>=0)soundKill(fxChannel);fxChannel=-1;if(popChannel>=0)soundKill(popChannel);popChannel=-1;for(unsigned i=0;i<AUDIO_COUNT;i++)stopAudio(i);}
const Office *officeStatus(void){return &room;}
#include "camera_input.inc"
static void blit(const u16 *src,unsigned w,unsigned h,int x,int y) {
 for(unsigned sy=0;sy<h;sy++) {
  int dy=y+sy;if(dy<0 || dy>=192)continue;
  for(unsigned sx=0;sx<w;sx++) {
   int dx=x+sx;if(dx<0 || dx>=256)continue;
   u16 v=src[sy*w+sx];if(v&BIT(15))screen[dy*256+dx]=v;
  }
 }
}
#include "camera_render.inc"
void officeRender(u16 *vram) {
 if(room.dead){deathRender(vram);return;}
 if(officeCameraMode()){cameraRender(vram);return;}
 if(deskBorrowed){for(unsigned i=0;i<4;i++){char name[32];snprintf(name,sizeof(name),"desk_%u",i);loadImage(name,desk[i],213*109);}deskBorrowed=false;}
 unsigned f=officeFrame(&room);
 if(cachedScene!=(int)f){char name[32];snprintf(name,sizeof(name),"office_%02u",f);if(loadImage(name,scene,400*192))cachedScene=f;}
 for(unsigned y=0;y<192;y++)memcpy(screen+y*256,scene+y*400+room.pan,256*2);
 u64 now=officeElapsedMilliseconds();bool defend=now<ai.defenseUntil && room.maskFrame;
 if((ai.loc[TF]==INSIDE || room.invasion || defend) && (now/70)%2==0)memset(screen,0,sizeof(screen));
 int maskFaceLift=room.maskFrame?40:0;
 if(ai.loc[TF]==INSIDE)actor(0,110-room.pan/2,42-maskFaceLift,false);
 if(ai.loc[GF]==INSIDE)actor(1,83-room.pan/3,60-maskFaceLift,false);
 if(ai.loc[MG]==CEILING)actor(5,64,-maskFaceLift,false);
 if(ai.bbDisabled)actor(22,12-room.pan/3,64-maskFaceLift,false);
 blit(desk[(room.ticks/3)%4],213,109,133-room.pan,83);
 // Conditional reward overlays, default locked; future challenge results set this bitmask.
 for(unsigned i=0;i<9;i++)if((room.rewards&(1u<<i)))blit(i<8?toys[i]:specialToy,i<8?28:48,i<8?32:18,141+(i%5)*35-room.pan,126-(i/5)*24);
 blit(lightButtons[room.leftLight?1:0],15,23,18-room.pan,96);
 blit(lightButtons[room.rightLight?3:2],15,23,367-room.pan,96);
 if(defend){unsigned t=5000-(unsigned)(ai.defenseUntil-now);int x=ai.defender==TB?nightBonnieMaskX(t):64-(int)(t*256/5000);actor(ai.defender==TB?8:15,x,40-maskFaceLift,false);}
 if(room.maskFrame || room.monitorFrame) {
  int key=room.maskFrame?(int)room.maskFrame:100+(int)room.monitorFrame;
  if(key!=cachedOverlay){char name[32];snprintf(name,sizeof(name),room.maskFrame?"mask_%02u":"monitor_%02u",room.maskFrame?room.maskFrame:room.monitorFrame);if(loadImage(name,overlay,256*192))cachedOverlay=key;else cachedOverlay=-1;}
  if(cachedOverlay==key)blit(overlay,256,192,0,0);
 }

 DC_FlushRange(screen,sizeof(screen));dmaCopy(screen,vram,sizeof(screen));
 if(!room.monitorFrame && ((now/70)%2!=0 || room.maskFrame==10))nightOfficePresented(&ai,officeElapsedMilliseconds());
}

#include "camera_panel.inc"

void officeDebugFiveAM(void){officeDebugSetHour(&room,5,officeElapsedMilliseconds());}
void officeMarkDebugUsed(void){room.debugUsed=true;}

#ifdef DEV_BUILD
void officeDebugPause(bool pause){
 if(pause&&!debugPaused){debugPauseTick=readNightTimer();debugPaused=true;}
 else if(!pause&&debugPaused){nightTimerStart+=readNightTimer()-debugPauseTick;debugPaused=false;cachedPanel=-1;}
}
void officeDebugVictory(void){room.debugUsed=true;room.dead=false;room.complete=true;room.hour=6;}
#endif
