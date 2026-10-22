#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "end_ui.h"
enum {GO_IDLE_MS=400,GO_SELECTED_MS=200,GO_OFF_MS=90,GO_BLACK_FRAME=14,GO_ENTRY_MS=120};
typedef enum {GO_ENTRY,GO_IDLE,GO_PRESS,GO_CONFIRM,GO_SHUTDOWN,GO_BLACK,GO_DONE} GameOverPhase;
typedef struct {
 GameOverPhase phase;
 EndUIAction selected;
 uint32_t phaseStart,idleStart;
 unsigned shutdownStep;
 bool armed,delivered;
} GameOverState;
typedef struct {bool held,pressed;int x,y;bool retryKey,menuKey;} GameOverInput;
void gameOverReset(GameOverState *s,uint32_t now);
EndUIAction gameOverUpdate(GameOverState *s,uint32_t now,GameOverInput input);
unsigned gameOverFrame(const GameOverState *s,uint32_t now);
bool gameOverLocked(const GameOverState *s);
