#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef struct {uint32_t random;unsigned current,decisions;} AmbienceState;
void ambienceReset(AmbienceState *s,uint32_t seed);
unsigned ambienceAdvance(AmbienceState *s,bool finished);
