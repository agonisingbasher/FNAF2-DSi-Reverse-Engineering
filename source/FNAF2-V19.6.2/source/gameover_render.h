#pragma once
#include <nds.h>
#include <stdbool.h>
#include "end_ui.h"
enum {GO_SHEET_WIDTH=1280,GO_SHEET_HEIGHT=768,GO_SHEET_BYTES=16+512+1280*768};
bool gameOverOpen(void);
void gameOverClose(void);
bool gameOverReady(void);
/* row-major frame 0..17, 14=black; 18/19 never rendered. Source rect changes; no per-frame allocation/IO. */
bool gameOverRender(unsigned frame,u16 *bottom);
bool gameOverRenderUI(unsigned frame,EndUIAction selected,u16 *bottom);
