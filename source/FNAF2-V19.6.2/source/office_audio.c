#include "office_audio.h"
OfficeAudioPlan officeAudioPlan(bool previousMask,bool previousMonitor,const Office *room) {
 OfficeAudioPlan p={0};p.stopAll=room->dead || room->complete;
 if(p.stopAll)return p;
 p.buzz=room->centerLight || room->leftLight || room->rightLight || room->cameraLight;
 p.maskOn=!previousMask && room->maskTarget;
 p.maskOff=previousMask && !room->maskTarget;
 p.monitorOn=!previousMonitor && room->monitorTarget;
 p.monitorOff=previousMonitor && !room->monitorTarget;
 return p;
}
