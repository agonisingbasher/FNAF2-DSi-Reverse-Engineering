#include "call_stream.h"
#include <string.h>
void callStreamClose(CallStream *s) {
 if(s->file)fclose(s->file);
 s->file=NULL;s->active=false;
}
static bool fill(CallStream *s,unsigned slot,uint32_t at) {
 uint8_t *dest=s->ring+slot*CALL_BLOCK;memset(dest,0,CALL_BLOCK);
 if(at>=s->length)return true;
 unsigned n=s->length-at;if(n>CALL_BLOCK)n=CALL_BLOCK;
 return fseek(s->file,at,SEEK_SET)==0 && fread(dest,1,n,s->file)==n;
}
bool callStreamOpenRate(CallStream *s,FILE *file,uint64_t now,unsigned rate) {
 s->rate=rate;
 callStreamClose(s);if(!file)return false;s->file=file;
 if(!rate){callStreamClose(s);return false;}
 if(fseek(file,0,SEEK_END)!=0){callStreamClose(s);return false;}
 long length=ftell(file);
 if(length<=0 || length>8*1024*1024 || length%4){callStreamClose(s);return false;}
 s->length=(uint32_t)length;s->offset=0;s->lastBlock=0;s->startMilliseconds=now;
 if(!fill(s,0,0) || !fill(s,1,CALL_BLOCK)){callStreamClose(s);return false;}
 s->active=true;return true;
}
static uint64_t position(const CallStream *s,uint64_t now) {
 return s->offset+(now-s->startMilliseconds)*s->rate/1000;
}
CallResult callStreamUpdate(CallStream *s,uint64_t now) {
 if(!s->active)return CALL_IDLE;
 uint64_t at=position(s,now);
 if(at>=s->length){callStreamClose(s);return CALL_FINISHED;}
 uint32_t block=(uint32_t)((at-s->offset)/CALL_BLOCK);
 if(block>=s->lastBlock+2)return CALL_UNDERRUN; // Caller must stop DMA before refilling both halves.
 if(block>s->lastBlock) {
  if(!fill(s,s->lastBlock%2,s->offset+(block+1)*CALL_BLOCK)){callStreamClose(s);return CALL_READ_ERROR;}
  s->lastBlock=block;
 }
 return CALL_RUNNING;
}
bool callStreamResume(CallStream *s,uint64_t now) {
 if(!s->active)return false;
 uint64_t at=position(s,now);if(at>=s->length){callStreamClose(s);return false;}
 s->offset=(uint32_t)at&~3u;s->startMilliseconds=now;s->lastBlock=0;
 if(!fill(s,0,s->offset) || !fill(s,1,s->offset+CALL_BLOCK)){callStreamClose(s);return false;}
 return true;
}

bool callStreamOpen(CallStream *s,FILE *file,uint64_t now){return callStreamOpenRate(s,file,now,CALL_RATE);}
