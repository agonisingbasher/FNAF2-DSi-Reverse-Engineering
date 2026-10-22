#include "ambience_state.h"
#include <assert.h>
int main(void){
 AmbienceState s;ambienceReset(&s,0);assert(s.current==0);assert(s.random!=0);
 uint32_t seed=s.random;
 for(unsigned i=0;i<2000;i++){assert(ambienceAdvance(&s,false)==0);assert(s.random==seed);assert(!s.decisions);}
 unsigned repeated=0,switched=0;
 for(unsigned i=0;i<1000;i++){
  unsigned old=s.current;unsigned next=ambienceAdvance(&s,true);assert(next<2);assert(s.decisions==i+1);
  if(next==old)repeated++;else switched++;
  seed=s.random;assert(ambienceAdvance(&s,false)==next);assert(seed==s.random);
 }
 assert(repeated>400 && repeated<600);assert(switched>400 && switched<600);
 AmbienceState same;ambienceReset(&same,0);ambienceReset(&s,0);
 for(unsigned i=0;i<50;i++)assert(ambienceAdvance(&s,true)==ambienceAdvance(&same,true));
 return 0;
}
