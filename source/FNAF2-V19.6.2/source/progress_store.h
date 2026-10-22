#ifndef PROGRESS_STORE_H
#define PROGRESS_STORE_H
#include "game_state.h"
#include <stdbool.h>
#define SAVE_PATH_SIZE 512
typedef struct {char directory[SAVE_PATH_SIZE],parent[SAVE_PATH_SIZE],root[SAVE_PATH_SIZE];} ProgressStore;
bool progressStoreInit(ProgressStore *store,const char *romPath,const char *workingDirectory);
bool progressStoreLoad(const ProgressStore *store,Progress *progress,bool *migrated);
bool progressStoreSave(const ProgressStore *store,const Progress *progress);
#endif
