#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct {
 unsigned night, ticks, maskFrame, monitorFrame, hour;
 int pan, batteryMilli, batteryDrainRemainder;
 unsigned invasion, corridor, leftVent, rightVent, preview;
 uint64_t batteryLastMilliseconds;
 int64_t debugClockOffsetMilliseconds;
 bool batteryClockSet,batteryLighting,debugUsed;
 uint64_t hallLightLockUntil;
 bool maskTarget, monitorTarget, centerLight, leftLight, rightLight;
 bool moving, flashlightNeedsRelease, dead, muted, complete, previewUsed;
 unsigned invasionTicks, touchLight;
 bool aiManaged,cameraLight,bbBlocked,hallLightLockActive,hallLightLockHeld;unsigned blockedLight;
 bool callActive, muteBluePending, muteRestPending;
 uint64_t muteFeedbackUntil, muteResetUntil;
 uint16_t rewards;
} Office;
void officeReset(Office *o,unsigned night);
unsigned officeFrame(const Office *o);
void officeTick(Office *o,bool x,bool left,bool right,bool l,bool r);
void officeToggleMask(Office *o);
void officeToggleMonitor(Office *o);
void officePreview(Office *o);
void officeHallLightLockStart(Office *o,uint64_t now,bool inputHeld);
bool officeHallLightLockUpdate(Office *o,uint64_t now,bool inputHeld,bool *playFeedback);

void officeAdvanceClock(Office *o,uint64_t elapsedMilliseconds);

uint64_t officeCounterMilliseconds(uint32_t elapsedTicks,uint32_t busClock);

void officeAdvanceBattery(Office *o,uint64_t elapsedMilliseconds);
unsigned officeBatteryIcon(const Office *o);
bool officeDebugSetHour(Office *o,unsigned hour,uint64_t elapsedMilliseconds);
unsigned officeDebugNextNight(unsigned current,int direction);
