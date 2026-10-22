#pragma once
#include "office_state.h"
typedef struct { bool buzz, maskOn, maskOff, monitorOn, monitorOff, stopAll; } OfficeAudioPlan;
OfficeAudioPlan officeAudioPlan(bool previousMask,bool previousMonitor,const Office *room);
