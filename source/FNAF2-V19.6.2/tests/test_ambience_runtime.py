"""Compile the actual streaming/variation functions with fake hardware and real PCM files."""
from pathlib import Path
import tempfile,subprocess,os
R=Path(__file__).resolve().parents[1]
code=(R/'source/camera_audio.inc').read_text().split('static void playVoice(')[0]
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);os.symlink(R/'nitrofiles',p/'inputs')
 harness=r'''#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "call_stream.h"
#include "ambience_state.h"
#define SoundFormat_8Bit 0
static uint64_t now;
static unsigned starts,kills;
static bool missingVariant;
typedef struct {CallStream stream;int channel;const char *name;unsigned rate;} LoopSound;
static LoopSound ambienceLoop={.channel=-1};static AmbienceState ambienceChoice;
static uint64_t officeElapsedMilliseconds(void){return now;}
static void DC_FlushRange(const void *p,unsigned n){(void)p;assert(n==CALL_BUFFER);}
static void soundKill(int channel){assert(channel>=0);kills++;}
static void soundSetVolume(int channel,unsigned volume){assert(channel>=0);assert(volume==55);}
static int soundPlaySample(const void *p,int format,unsigned n,unsigned rate,unsigned volume,int pan,bool loop,unsigned start){(void)p;(void)format;(void)pan;(void)start;assert(n==CALL_BUFFER);assert(rate==4000 || rate==8000);assert(volume==55);assert(loop);starts++;return 1;}
static FILE *openResource(const char *name){if(missingVariant && !strcmp(name,"ambience_depths.raw"))return NULL;char path[100];snprintf(path,sizeof(path),"inputs/%s",name);return fopen(path,"rb");}
'''
 harness += code
 harness += r'''
int main(void){
 ambienceReset(&ambienceChoice,0);updateOfficeAmbience();assert(ambienceChoice.current==0);assert(ambienceLoop.rate==4000);
 unsigned same=0,changed=0;bool sawVariant=false;
 while(ambienceChoice.decisions<20){
  unsigned old=ambienceChoice.current,choices=ambienceChoice.decisions;
  uint64_t due=ambienceLoop.stream.offset+(now+100-ambienceLoop.stream.startMilliseconds)*ambienceLoop.stream.rate/1000;
  bool eof=due>=ambienceLoop.stream.length;now+=100;updateOfficeAmbience();assert(ambienceLoop.stream.active);
  assert(ambienceChoice.decisions==choices+(eof?1:0));
  if(eof){if(old==ambienceChoice.current)same++;else changed++;}
  if(ambienceChoice.current){sawVariant=true;assert(ambienceLoop.rate==8000);assert(!strcmp(ambienceLoop.name,"ambience_depths.raw"));}
 }
 assert(same && changed && sawVariant);
 /* Force a missed refill: resume position, never treat it as a track completion. */
 loopStop(&ambienceLoop);ambienceChoice.current=1;startOfficeAmbience();unsigned count=ambienceChoice.decisions;
 now+=3000;updateOfficeAmbience();assert(ambienceChoice.decisions==count);assert(ambienceChoice.current==1);assert(ambienceLoop.stream.offset>0);
 /* Missing optional variant falls back to the original, without a full-track allocation. */
 loopStop(&ambienceLoop);ambienceChoice.current=1;missingVariant=true;startOfficeAmbience();assert(ambienceChoice.current==0);assert(ambienceLoop.stream.active);
 loopStop(&ambienceLoop);assert(!ambienceLoop.stream.active);assert(ambienceLoop.channel<0);assert(kills);
 printf("Ambience runtime: 20 real PCM EOFs, %u repeats/%u changes; underrun resumes; missing variant falls back; ring=%u bytes.\n",same,changed,CALL_BUFFER);
 return 0;
}
'''
 (p/'audio_test.c').write_text(harness)
 subprocess.run(['cc','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(R/'source'),str(p/'audio_test.c'),str(R/'source/call_stream.c'),str(R/'source/ambience_state.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],cwd=p,check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
