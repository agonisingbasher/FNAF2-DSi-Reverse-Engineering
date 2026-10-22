#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
bool loadingBegin(unsigned short *top,unsigned short *bottom);
void loadingPump(void);
void loadingEnd(void);
size_t loadingRead(void *ptr,size_t size,size_t count,FILE *f);
#ifdef __cplusplus
}
#endif
