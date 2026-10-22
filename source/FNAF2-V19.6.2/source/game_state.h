#pragma once
#include <stdbool.h>
#include <stdint.h>
#define AI_COUNT 10
/* Serialized explicitly, independent of compiler padding. */
typedef struct { uint8_t night, stars, cheats, ai[AI_COUNT]; uint16_t rewards; } Progress;
void progressReset(Progress *p);
bool progressValid(const Progress *p);
bool progressVictory(Progress *p, unsigned night, const uint8_t *ai, bool cheatsUsed);
unsigned menuFrame(const Progress *p, unsigned selected);
unsigned menuCount(const Progress *p);

bool progressCanContinue(const Progress *p);

void progressNewGame(Progress *p);
