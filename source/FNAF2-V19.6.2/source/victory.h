#pragma once
#include <nds.h>
#include "victory_timing.h"
#include "end_ui.h"
void victorySetSelection(EndUIAction selected);
bool victoryOpen(unsigned night);
void victoryClose(void);
bool victoryReady(void);
bool victoryRender(unsigned elapsedMs,u16 *top,u16 *bottom);
void victoryReleaseTouch(bool released);
bool victoryTouchArmed(void);
