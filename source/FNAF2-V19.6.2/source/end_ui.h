#pragma once
#include <stdbool.h>
typedef enum {END_UI_NONE,END_UI_PRIMARY,END_UI_MENU} EndUIAction;
typedef struct {bool armed;} EndUITouch;
void endUIReset(EndUITouch *state);
EndUIAction endUITouch(EndUITouch *state,bool held,bool pressed,int x,int y,bool ready);
unsigned endUIContinueNight(unsigned night);
