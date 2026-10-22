#include "gameover_render.h"
#include "touch_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned char *sheet;
static int lastFrame=-1;
static unsigned word(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
void gameOverClose(void){free(sheet);sheet=NULL;lastFrame=-1;}
bool gameOverReady(void){return sheet!=NULL;}
bool gameOverOpen(void){
 gameOverClose();FILE *f=fopen("nitro:/gameover_crt_sheet.bin","rb");if(!f)return false;
 sheet=malloc(GO_SHEET_BYTES);
 bool ok=sheet && fread(sheet,1,GO_SHEET_BYTES,f)==GO_SHEET_BYTES && fgetc(f)==EOF;
 fclose(f);
 if(ok)ok=!memcmp(sheet,"GOCRT4X5",8) && word(sheet+8)==1280 && word(sheet+10)==768 && word(sheet+12)==256 && word(sheet+14)==192;
 if(!ok){gameOverClose();}
 return ok;
}
bool gameOverRender(unsigned frame,u16 *bottom){
 if(frame>=18)frame=14;
 if(lastFrame==(int)frame)return false;
 lastFrame=(int)frame;
 if(frame==14 || !sheet){dmaFillHalfWords(0x8000,bottom,256*192*2);return true;}
 const u16 *palette=(const u16*)(sheet+16);
 const unsigned char *pixels=sheet+16+512;
 unsigned sourceX=(frame%5)*256,sourceY=(frame/5)*192;
 for(unsigned y=0;y<192;y++){
  const unsigned char *src=pixels+(sourceY+y)*GO_SHEET_WIDTH+sourceX;
  u16 *dest=bottom+y*256;
  for(unsigned x=0;x<256;x++)dest[x]=palette[src[x]];
 }
 return true;
}

bool gameOverRenderUI(unsigned frame,EndUIAction selected,u16 *bottom){
 static EndUIAction previous=END_UI_NONE;
 if(previous!=selected)lastFrame=-1;
 previous=selected;
 bool changed=gameOverRender(frame,bottom);
 if(changed && sheet && (frame<10 || frame==16 || frame==17)){
  const u16 *pal=(const u16*)(sheet+16);
  const unsigned char *px=sheet+528;
  unsigned col=selected==END_UI_PRIMARY?1:selected==END_UI_MENU?2:0;
  /* Only the button strip from row 4; never cover the entrance/glitch text. */
  for(unsigned y=150;y<192;y++)for(unsigned x=0;x<256;x++)
   bottom[y*256+x]=pal[px[(3*192+y)*GO_SHEET_WIDTH+col*256+x]];
 }
 return changed;
}
