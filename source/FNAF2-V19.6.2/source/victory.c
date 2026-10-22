#include "victory.h"
#include "touch_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned char *clockBank,*topBank,*bottomBank;
static int lastTop=-1,lastBottom=-1;
static bool armed;
static unsigned completedNight;
static EndUIAction selection;
static EndUIAction renderedSelection;
static unsigned bottomFade;
#define LOWER_BYTES (528+512*384)
static unsigned char *readBank(const char *name,unsigned frames) {
    char path[96];snprintf(path,sizeof(path),"nitro:/%s",name);
    FILE *f=fopen(path,"rb");if(!f)return NULL;
    size_t bytes=(size_t)frames*VICTORY_FRAME_BYTES;
    unsigned char *p=malloc(bytes);
    if(!p || fread(p,1,bytes,f)!=bytes){free(p);p=NULL;}
    fclose(f);return p;
}
void victoryClose(void) {
    free(clockBank);free(topBank);free(bottomBank);
    clockBank=topBank=bottomBank=NULL;lastTop=lastBottom=-1;armed=false;selection=renderedSelection=END_UI_NONE;bottomFade=999;
}
bool victoryReady(void) {return clockBank && topBank && bottomBank;}
bool victoryOpen(unsigned night) {
    victoryClose();if(night<1 || night>7)return false;
    const char *group=night<=4?"toys":night==5?"withered_light":night==6?"withered_full":"shadow_golden";
    char name[64];clockBank=readBank("v194_clock.bin",9);
    snprintf(name,sizeof(name),"v194_top_%s.bin",group);topBank=readBank(name,16);
    completedNight=night;
    FILE *lower=fopen("nitro:/victory_industrial_sheet.bin","rb");
    if(lower){bottomBank=malloc(LOWER_BYTES);if(bottomBank && (fread(bottomBank,1,LOWER_BYTES,lower)!=LOWER_BYTES || memcmp(bottomBank,"VICT2X2!",8))){free(bottomBank);bottomBank=NULL;}fclose(lower);}
    if(!victoryReady()){victoryClose();return false;}
    return true;
}
static void decode(const unsigned char *bank,unsigned frame,u16 *dest) {
    const unsigned char *p=bank+(size_t)frame*VICTORY_FRAME_BYTES;
    const u16 *pal=(const u16*)p;const unsigned char *pixels=p+512;
    for(unsigned i=0;i<256*192;i++)dest[i]=pal[pixels[i]];
}
static const unsigned char digitRows[4][7]={
 {4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2}
};
static void nightNumber(u16 *dest){
 if(completedNight>4)return;
 /* Replace only the baked '1'; title background is black, paper repaired from its neighbour. */
 for(unsigned y=40;y<51;y++)for(unsigned x=113;x<122;x++)dest[y*256+x]=0x8000;
 for(unsigned y=118;y<126;y++)for(unsigned x=75;x<83;x++)dest[y*256+x]=dest[y*256+x+12];
 for(unsigned y=0;y<7;y++)for(unsigned x=0;x<5;x++)if(digitRows[completedNight-1][y]&(1u<<(4-x))){
  dest[(42+y)*256+115+x]=0xffff;dest[(119+y)*256+76+x]=0x8000;
 }
}
void victorySetSelection(EndUIAction selected){selection=selected;}
static void lowerRender(unsigned alpha,u16 *bottom){
 unsigned cell=completedNight<=4?0:completedNight-4;
 unsigned sx=cell%2*256,sy=cell/2*192;
 const u16 *pal=(const u16*)(bottomBank+16);const unsigned char *pixels=bottomBank+528;
 for(unsigned y=0;y<192;y++)for(unsigned x=0;x<256;x++)bottom[y*256+x]=pal[pixels[(sy+y)*512+sx+x]];
 nightNumber(bottom);touchUIDraw(bottom,true,selection);
 for(unsigned i=0;i<256*192;i++){
  u16 c=bottom[i];bottom[i]=0x8000|((c&31)*alpha/14)|((((c>>5)&31)*alpha/14)<<5)|((((c>>10)&31)*alpha/14)<<10);
 }
}
bool victoryRender(unsigned ms,u16 *top,u16 *bottom) {
    if(!victoryReady()){
        if(lastTop<0){dmaFillHalfWords(0x8000,top,256*192*2);memset(bottom,0,256*192*2);lastTop=0;return true;}
        return false;
    }
    unsigned ti=victoryTopFrame(ms),alpha=ms<9000?0:(ms-9000)/100;
    if(alpha>14)alpha=14;
    if(lastTop!=(int)ti){decode(ti<9?clockBank:topBank,ti<9?ti:ti-9,top);lastTop=(int)ti;}
    if(bottomFade!=alpha || renderedSelection!=selection){lowerRender(alpha,bottom);bottomFade=alpha;renderedSelection=selection;return true;}
    return false;
}
void victoryReleaseTouch(bool released) {if(released)armed=true;}
bool victoryTouchArmed(void) {return armed;}
