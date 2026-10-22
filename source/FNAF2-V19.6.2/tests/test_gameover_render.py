"""Exercise the actual C renderer against source rectangles after removing its file path."""
from pathlib import Path
import tempfile,subprocess,os
R=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'nds.h').write_text('''#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
typedef uint16_t u16;
static inline void dmaFillHalfWords(uint16_t v,void *ptr,size_t n){uint16_t *d=ptr;for(size_t i=0;i<n/2;i++)d[i]=v;}
''')
 (p/'test.c').write_text('''#include "gameover_render.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
static u16 lower[256*192];
int main(void){
 assert(!gameOverOpen());assert(!gameOverReady());
 assert(symlink("inputs","nitro:")==0);assert(gameOverOpen());assert(gameOverReady());
 assert(unlink("nitro:")==0); /* The animation must have no filesystem dependency. */
 unsigned char *source=malloc(GO_SHEET_BYTES);assert(source);
 FILE *f=fopen("inputs/gameover_crt_sheet.bin","rb");assert(f);assert(fread(source,1,GO_SHEET_BYTES,f)==GO_SHEET_BYTES);fclose(f);
 for(unsigned frame=0;frame<9;frame++){
  assert(gameOverRender(frame,lower));assert(!gameOverRender(frame,lower));
  unsigned sx=frame%3*256,sy=frame/3*192;
  for(unsigned y=0;y<192;y++)for(unsigned x=0;x<256;x++){
   unsigned ix=source[528+(sy+y)*768+sx+x];u16 expect=source[16+ix*2]|source[17+ix*2]<<8;
   assert(lower[y*256+x]==expect);
  }
 }
 gameOverRender(9,lower);for(unsigned i=0;i<256*192;i++)assert(lower[i]==0x8000);
 assert(!gameOverRender(9,lower));gameOverClose();gameOverClose();assert(!gameOverReady());
 gameOverRender(0,lower);for(unsigned i=0;i<256*192;i++)assert(lower[i]==0x8000);
 /* Reject corrupt, truncated and oversized sheet data without leaking the allocation. */
 assert(mkdir("nitro:",0700)==0);
 for(unsigned kind=0;kind<3;kind++){
  f=fopen("nitro:/gameover_crt_sheet.bin","wb");assert(f);
  if(kind==0){source[0]='!';assert(fwrite(source,1,GO_SHEET_BYTES,f)==GO_SHEET_BYTES);source[0]='G';}
  if(kind==1)assert(fwrite(source,1,100,f)==100);
  if(kind==2){assert(fwrite(source,1,GO_SHEET_BYTES,f)==GO_SHEET_BYTES);fputc(0,f);}
  fclose(f);assert(!gameOverOpen());assert(!gameOverReady());
 }
 free(source);return 0;
}
'''.replace('#include <unistd.h>','#include <unistd.h>\n#include <sys/stat.h>'))
 os.symlink(R/'nitrofiles',p/'inputs')
 subprocess.run(['cc','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(p),'-I'+str(R/'source'),str(p/'test.c'),str(R/'source/gameover_render.c'),str(R/'source/touch_ui.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],cwd=p,check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
print('CRT renderer: all nine exact source rectangles, cached frames, black, missing/bad data and no playback IO passed (ASan/UBSan).')
