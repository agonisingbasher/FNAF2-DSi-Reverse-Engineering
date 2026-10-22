#pragma once
#include <stdbool.h>
typedef struct {unsigned kind,frames;int panFifths;} Cutscene;
enum { CUTSCENE_LOG_BOOT, CUTSCENE_LOG_ERRORS, CUTSCENE_LOG_RECOGNITION, CUTSCENE_LOG_ITS_ME, CUTSCENE_LOG_UNKNOWN, CUTSCENE_LOG_OVERRIDE, CUTSCENE_LOG_BLACK };
void cutsceneReset(Cutscene *c,unsigned kind);
void cutsceneTick(Cutscene *c,bool left,bool right);
unsigned cutsceneRoomFrame(const Cutscene *c);
int cutscenePan(const Cutscene *c);
bool cutsceneGoldenVisible(const Cutscene *c);
bool cutsceneFinished(const Cutscene *c);
unsigned cutsceneLogMode(unsigned kind,unsigned frames,bool goldenVisible);
