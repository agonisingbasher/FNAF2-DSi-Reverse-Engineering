#pragma once
#include <stdint.h>
enum {CUSTOM_PRESETS=5};
extern const char *customPresetNames[CUSTOM_PRESETS];
void customPreset(uint8_t *levels,unsigned preset);
unsigned customMove(unsigned selected,int dx,int dy);
void customAdjust(uint8_t *levels,unsigned selected,int delta);
