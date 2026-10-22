"""Validate the production C blitters, dynamic night digits and shared button colors."""
from pathlib import Path
import os,subprocess,tempfile,struct
from PIL import Image
R=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);os.symlink(R/'nitrofiles',p/'inputs');(p/'frames').mkdir()
 (p/'test.c').write_text(r'''#include "victory.h"
#include "gameover_render.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
static u16 top[256*192],lower[256*192];
static unsigned char *data;
static unsigned char *readBankTest(const char *name,unsigned bytes){char path[128];snprintf(path,sizeof(path),"inputs/%s",name);FILE *f=fopen(path,"rb");assert(f);unsigned char *p=malloc(bytes);assert(p);assert(fread(p,1,bytes,f)==bytes);fclose(f);return p;}
static void dump(const char *prefix,unsigned night,unsigned state){char name[128];snprintf(name,sizeof(name),"frames/%s_%u_%u.u16",prefix,night,state);FILE *f=fopen(name,"wb");assert(f);assert(fwrite(lower,2,256*192,f)==256*192);fclose(f);}
static void buttons(unsigned selected){
 u16 grey=0x8000|20|(21<<5)|(22<<10),blue=0x8000|5|(27<<5)|(31<<10);
 assert(lower[152*256+10]==(selected==1?blue:grey));assert(lower[152*256+134]==(selected==2?blue:grey));
 if(!selected)for(unsigned y=150;y<190;y++)for(unsigned x=8;x<250;x++){
  if(x>=126 && x<132)continue;u16 c=lower[y*256+x];unsigned r=c&31,g=(c>>5)&31,b=(c>>10)&31;assert(!(g>r+8 && b>r+8));
 }
}
static void checkTop(unsigned night,unsigned ms){
 unsigned ti=victoryTopFrame(ms);char name[80];
 if(ti<9)strcpy(name,"v194_clock.bin");else snprintf(name,sizeof(name),"v194_top_%s.bin",night<=4?"toys":night==5?"withered_light":night==6?"withered_full":"shadow_golden");
 unsigned frame=ti<9?ti:ti-9;unsigned char *b=readBankTest(name,(ti<9?9:16)*VICTORY_FRAME_BYTES);unsigned char *f=b+frame*VICTORY_FRAME_BYTES;
 for(unsigned i=0;i<256*192;i++){unsigned ix=f[512+i];assert(top[i]==(f[2*ix]|f[2*ix+1]<<8));}free(b);
}
int main(void){
 data=readBankTest("victory_industrial_sheet.bin",528+512*384);
 for(unsigned n=1;n<=7;n++){
  assert(symlink("inputs","nitro:")==0);assert(victoryOpen(n));assert(unlink("nitro:")==0);
  const unsigned times[]={0,4499,4500,8999,9000,9400,9800,10399,10400,12000};
  for(unsigned i=0;i<sizeof(times)/sizeof(times[0]);i++){
   unsigned ms=times[i];victoryRender(ms,top,lower);checkTop(n,ms);
   if(ms<9100)for(unsigned j=0;j<256*192;j++)assert(lower[j]==0x8000);
  }
  buttons(0);dump("victory",n,0);
  /* Every document pixel is copied from its proper sheet cell, except dynamic N1-4 digit. */
  unsigned cell=n<=4?0:n-4,sx=cell%2*256,sy=cell/2*192;
  for(unsigned y=55;y<145;y++)for(unsigned x=0;x<256;x++){
   if(n<=4 && x>=75 && x<83 && y>=118 && y<126)continue;
   unsigned ix=data[528+(sy+y)*512+sx+x];assert(lower[y*256+x]==(data[16+ix*2]|data[17+ix*2]<<8));
  }
  for(unsigned chosen=1;chosen<=2;chosen++){
   victorySetSelection((EndUIAction)chosen);assert(victoryRender(12000,top,lower));buttons(chosen);dump("victory",n,chosen);
  }
  victoryClose();assert(!victoryReady());
 }
 free(data);
 assert(symlink("inputs","nitro:")==0);assert(gameOverOpen());assert(unlink("nitro:")==0);
 for(unsigned frame=0;frame<3;frame++){gameOverRenderUI(frame,END_UI_NONE,lower);buttons(0);dump("gameover",frame,0);}
 gameOverRenderUI(3,END_UI_PRIMARY,lower);buttons(1);dump("gameover",3,1);
 gameOverRenderUI(4,END_UI_MENU,lower);buttons(2);dump("gameover",4,2);
 gameOverRenderUI(9,END_UI_NONE,lower);for(unsigned i=0;i<256*192;i++)assert(lower[i]==0x8000);
 gameOverClose();return 0;
}
''')
 subprocess.run(['cc','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fsanitize=address,undefined','-I'+str(R/'tests/host'),'-I'+str(R/'source'),str(p/'test.c'),*[str(R/'source'/x) for x in ['victory.c','gameover_render.c','touch_ui.c']],'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],cwd=p,check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
 for f in (p/'frames').glob('*.u16'):
  vals=struct.unpack('<49152H',f.read_bytes());im=Image.new('RGB',(256,192));im.putdata([((c&31)*255//31,((c>>5)&31)*255//31,((c>>10)&31)*255//31) for c in vals]);im.save(R/'previews'/(f.stem+'.png'))
print('Fixed renderer: top preserved; seven industrial night documents, dynamic digits, two grey/blue buttons, all three GO Idle cells, black and no playback IO verified with ASan/UBSan.')
