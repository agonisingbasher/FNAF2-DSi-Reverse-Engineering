#include "call_stream.h"
#include <assert.h>
#include <string.h>
static FILE *clip(unsigned blocks,unsigned tail) {
 FILE *f=tmpfile();assert(f);
 for(unsigned b=0;b<blocks;b++)for(unsigned i=0;i<CALL_BLOCK;i++)fputc(10+b,f);
 for(unsigned i=0;i<tail;i++)fputc(42,f);
 rewind(f);return f;
}
int main(void) {
 CallStream s={0};assert(callStreamOpen(&s,clip(7,4),100));
 assert(s.ring[0]==10 && s.ring[CALL_BLOCK]==11);
 assert(callStreamUpdate(&s,1099)==CALL_RUNNING && s.lastBlock==0);
 assert(callStreamUpdate(&s,1100)==CALL_RUNNING && s.ring[0]==12 && s.ring[CALL_BLOCK]==11);
 assert(callStreamUpdate(&s,2100)==CALL_RUNNING && s.ring[0]==12 && s.ring[CALL_BLOCK]==13);
 assert(callStreamUpdate(&s,5100)==CALL_UNDERRUN && s.ring[0]==12); // No writes while DMA could own both halves.
 assert(callStreamResume(&s,5100));assert(s.offset==5*CALL_BLOCK && s.ring[0]==15 && s.ring[CALL_BLOCK]==16);
 assert(callStreamUpdate(&s,6100)==CALL_RUNNING && s.ring[0]==42 && s.ring[4]==0);
 assert(callStreamUpdate(&s,7100)==CALL_RUNNING);
 assert(callStreamUpdate(&s,7101)==CALL_FINISHED && !s.active && !s.file);
 assert(callStreamUpdate(&s,8100)==CALL_IDLE);
 assert(callStreamOpen(&s,clip(1,0),0));callStreamClose(&s);assert(!s.active && !s.file);
 assert(callStreamOpen(&s,clip(0,4),0));assert(s.ring[0]==42 && s.ring[4]==0 && s.ring[CALL_BLOCK]==0);
 assert(callStreamUpdate(&s,1)==CALL_FINISHED);
 assert(!callStreamOpen(&s,NULL,0));assert(!callStreamOpen(&s,clip(0,0),0));assert(!callStreamOpen(&s,clip(0,3),0));
 assert(callStreamOpenRate(&s,clip(4,0),100,4000));
 assert(callStreamUpdate(&s,2147)==CALL_RUNNING && s.lastBlock==0);
 assert(callStreamUpdate(&s,2148)==CALL_RUNNING && s.lastBlock==1 && s.ring[0]==12);
 callStreamClose(&s);assert(!callStreamOpenRate(&s,clip(1,0),0,0));
 return 0;
}
