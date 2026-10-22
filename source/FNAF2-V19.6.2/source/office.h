#pragma once
#include <nds.h>
#include "office_state.h"
bool officeOpen(unsigned night);
void officeSetRewards(uint16_t mask);
void officeClose(void);
void officeUpdate(u32 down,u32 held);
void officeRender(u16 *vram);
const Office *officeStatus(void);

unsigned officeLoadedAudioMask(void);

uint64_t officeElapsedMilliseconds(void);

bool officeRenderPanel(u16 *buffer);

void officeConfigureAI(const uint8_t *levels,bool cheats);
bool officeCameraMode(void);
bool officeCameraMapVisible(void);
unsigned officeCameraSelected(void);
bool officeDeathFinished(void);

void officeShowAsset(const char *name,u16 *vram);

void officeEndAudioBegin(bool won);
void officeEndAudioUpdate(bool won,unsigned elapsed);

void officeCutsceneBegin(unsigned kind);
void officeCutsceneRender(unsigned state,int pan,bool golden,bool puppet,u16 *vram);
void officeCutsceneAudio(unsigned kind,unsigned frames,bool moving);

void officeDebugFiveAM(void);
void officeMarkDebugUsed(void);

#ifdef DEV_BUILD
void officeDebugPause(bool pause);
void officeDebugVictory(void);
#endif
