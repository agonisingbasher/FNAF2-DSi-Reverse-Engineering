#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct {int x,y;} CamPoint;
extern const CamPoint cameraBoxes[12];
typedef struct {unsigned selected,closeStage,noiseStep;bool closeShown,touchLight;uint64_t closeUntil,changedAt;} CameraState;
void cameraReset(CameraState *c);
unsigned cameraNeighbor(unsigned current,int dx,int dy);
unsigned cameraHit(int x,int y);
bool cameraTabletHit(int x,int y);
bool cameraWindHit(int x,int y);
bool cameraLightHit(int x,int y);
void cameraSelect(CameraState *c,unsigned id,uint64_t now);
void cameraRequestClose(CameraState *c);
bool cameraAdvanceClose(CameraState *c,uint64_t now);
void cameraClosePresented(CameraState *c,uint64_t now);
unsigned cameraPan(unsigned id,uint64_t elapsed);
