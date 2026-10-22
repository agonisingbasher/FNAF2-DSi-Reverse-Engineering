#include "ambience_state.h"
void ambienceReset(AmbienceState *s,uint32_t seed){*s=(AmbienceState){.random=seed?seed:0xA8615u};}
unsigned ambienceAdvance(AmbienceState *s,bool finished){
 if(finished){uint32_t n=s->random;n^=n<<13;n^=n>>17;n^=n<<5;s->random=n;s->current^=n&1u;++s->decisions;}
 return s->current;
}
