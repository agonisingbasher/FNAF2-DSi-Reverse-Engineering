#pragma once
#include "office_state.h"
#include "game_state.h"
enum {TF,TB,TC,MG,BB,WF,WB,WC,FX,GF,PUPPET};
enum {HALL=13,LEFT_DOOR,RIGHT_DOOR,INSIDE,CEILING};
typedef struct {
 uint8_t level[10],step[10],loc[10];
 uint64_t nextMove[10],entered[10],frozen[10],last,puppetAt,nextGolden,shadowAt,maskReadyAt,defenseUntil,ghostUntil;
 uint64_t defendAt[10],holdAt[10],cueAt[10];bool defending[10],holding[10],answered[10];
 uint32_t rng;int music;unsigned puppetStage,killer,flashes,bbVoice,drainRemainder,defender,ghostFrame,lastCamera;
 uint8_t pendingMoves[9];unsigned moveCursor;
 uint8_t custom[10];unsigned night,puppetAI,ventEvents,attackOwner,foxyLightMs;bool stareEvent,hallEvent,hallExitEvent,mangleVisible;unsigned moveSound;
 bool bbDisabled,bonnieMoved,goldenPending,shadow,endo,previousFlash,previousCameraFlash,previousMonitor,previousMask,cheats;
 bool presentationTracking,awaitingOfficeFrame[10];
 uint64_t signalUntil;
} NightAI;
void nightAIReset(NightAI *a,unsigned night,const uint8_t *levels,bool cheats,uint32_t seed);
void nightAIUpdate(NightAI *a,Office *o,uint64_t now,unsigned camera,bool wind,bool cameraLight);
unsigned nightCameraFrame(const NightAI *a,unsigned camera,bool light);

void nightLevels(unsigned night,unsigned hour,const uint8_t *custom,uint8_t *out,unsigned *puppet);

unsigned nightMaskReactionMilliseconds(unsigned night);

const char *nightMovementAudio(unsigned who,unsigned variation);
bool nightStareAllowed(const NightAI *a,const Office *o,uint64_t now);
bool nightHallwayAudioActive(const NightAI *a,const Office *o);
int nightBonnieMaskX(unsigned elapsedMilliseconds);
bool nightFootstepsAudible(bool cameraMode,unsigned camera);

void nightOfficePresented(NightAI *a,uint64_t now);
