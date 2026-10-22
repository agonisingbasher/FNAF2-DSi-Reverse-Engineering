#include "loading.h"
#include <nds.h>
#include <calico/system/tick.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <calico/system/thread.h>
#include "loading_timing.h"
namespace {
unsigned char *topSheet,*swipeSheet;
unsigned short *topTarget,*bottomTarget;
LoadingTiming timing;
unsigned pairIndex,initSerial;
volatile bool active;
bool rendererStarted;
Thread rendererThread;
alignas(8) unsigned char rendererStack[8192];
void render();
int renderLoop(void*){
 while(active){
  swiWaitForVBlank();
  swiWaitForVBlank();
  if(!active)break;
  // 2 DS video frames ~33.43 ms, independent of gameplay timers and IO callbacks.
  loadingAdvance(timing,33428);
  render();
 }
 return 0;
}
unsigned char *load(const char *name,unsigned w,unsigned h,const char *magic){
 FILE *f=fopen(name,"rb");if(!f)return nullptr;
 unsigned n=528+w*h;
 auto *p=static_cast<unsigned char*>(malloc(n));
 bool ok=p && fread(p,1,n,f)==n && fgetc(f)==EOF;
 fclose(f);
 if(ok)ok=memcmp(p,magic,8)==0 && (p[8]|unsigned(p[9])<<8)==w && (p[10]|unsigned(p[11])<<8)==h;
 if(!ok){free(p);return nullptr;}return p;
}
unsigned short sample(unsigned char *p,unsigned w,unsigned x,unsigned y){
 const auto *pal=reinterpret_cast<unsigned short*>(p+16);
 return pal[p[528+y*w+x]];
}
unsigned short add(unsigned short a,unsigned short b,unsigned scale){
 unsigned r=(a&31)+((b&31)*scale/16),g=((a>>5)&31)+(((b>>5)&31)*scale/16),bl=((a>>10)&31)+(((b>>10)&31)*scale/16);
 return 0x8000|(r>31?31:r)|((g>31?31:g)<<5)|((bl>31?31:bl)<<10);
}
}
extern "C" bool loadingBegin(unsigned short *top,unsigned short *bottom){
 loadingEnd();
 topSheet=load("nitro:/loading_top.bin",1024,576,"LOAD4X3!");
 swipeSheet=load("nitro:/loading_swipe.bin",864,576,"SWIPE3!!");
 if(!topSheet || !swipeSheet){loadingEnd();return false;}
 topTarget=top;bottomTarget=bottom;
 const uint64_t ticks=tickGetCount();
 srand(unsigned(ticks)^unsigned(ticks>>32)^unsigned(time(nullptr))^(++initSerial*0x9e3779b9u));
 const unsigned option=1+unsigned(rand())%4;
 pairIndex=option-1;
 timing=LoadingTiming{};
 timing.row=unsigned(rand())%3;
 timing.offsetX=unsigned(rand())%864;
 active=true;
 render();
 // The renderer only reads preloaded atlases: no stdio/malloc/IRQ rendering.
 threadPrepare(&rendererThread,renderLoop,nullptr,rendererStack+sizeof(rendererStack),MAIN_THREAD_PRIO-1);
 threadAttachLocalStorage(&rendererThread,nullptr);
 rendererStarted=true;threadStart(&rendererThread);
 return true;
}
namespace {
void render(){
 unsigned ms=timing.effectsUs/1000;
 unsigned frame=pairIndex*2+timing.pose;
 unsigned effect=(ms/2400)%4,brightness=12;
 if(effect==1)brightness=4+((ms/140)%5)*3;
 if(effect==3)brightness=5+((ms/160)%5)*2;
 unsigned sourceX[256],sourceRow[256],effectX[256];
 for(unsigned x=0;x<256;x++){
  // Positive translation, no mirroring; incoming edge uses next row.
  int sx=int(x)-int(timing.offsetX);
  sourceX[x]=sx<0?unsigned(sx+864):unsigned(sx);
  sourceRow[x]=sx<0?(timing.row+1)%3:timing.row;
  effectX[x]=effect==0?(x+ms/40)%256:effect==2?(x+((ms/110)%3)*4)%256:x;
 }
 for(unsigned y=0;y<192;y++)for(unsigned x=0;x<256;x++){
  bottomTarget[y*256+x]=sample(swipeSheet,864,sourceX[x],sourceRow[x]*192+y);
  unsigned short base=sample(topSheet,1024,(frame%4)*256+x,(frame/4)*192+y);
  unsigned short bg=0x8000;
  unsigned ex=effectX[x],ey=effect==0?(y+ms/25)%192:y;
  bg=add(bg,sample(topSheet,1024,effect*256+ex,384+ey),brightness);
  topTarget[y*256+x]=(base&0x8000)?base:bg;
 }
}
}
extern "C" void loadingPump(){
 // Kept for existing callers. The VBlank renderer now owns Update/Render.
}
extern "C" void loadingEnd(){
 active=false;if(rendererStarted){threadJoin(&rendererThread);rendererStarted=false;}free(topSheet);free(swipeSheet);topSheet=swipeSheet=nullptr;
}
extern "C" size_t loadingRead(void *ptr,size_t size,size_t count,FILE *f){
 if(!active)return fread(ptr,size,count,f);
 if(!size || count>SIZE_MAX/size)return 0;
 size_t total=size*count,done=0;
 while(done<total){
  size_t n=total-done;if(n>8192)n=8192;
  size_t got=fread(static_cast<unsigned char*>(ptr)+done,1,n,f);
  done+=got;loadingPump();if(got!=n)break;
 }
 return done/size;
}
