#pragma once
#include "office_state.h"
typedef enum {PANEL_NONE,PANEL_HALL,PANEL_RIGHT,PANEL_LEFT,PANEL_MASK,PANEL_TABLET,PANEL_MUTE} PanelAction;
unsigned officePanelFrame(const Office *o,uint64_t now);
PanelAction officePanelHit(unsigned frame,int x,int y);
void officePanelApply(Office *o,PanelAction action,uint64_t now);
bool officeUpIsHorn(const Office *o);

void officePanelPresented(Office *o,unsigned frame,uint64_t now);
