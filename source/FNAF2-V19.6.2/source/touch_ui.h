#pragma once
#include <nds.h>
#include "end_ui.h"
/* Shared victory/Game Over plate hitboxes, color rules and runtime overlays. */
EndUIAction touchUIHit(int x,int y);
void touchUIDraw(u16 *dest,bool victory,EndUIAction selected);
