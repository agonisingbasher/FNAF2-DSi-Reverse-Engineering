#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define CALL_RATE 8192u
#define CALL_BLOCK 8192u
#define CALL_BUFFER (2u*CALL_BLOCK)
typedef struct {
 FILE *file;
 uint8_t ring[CALL_BUFFER] __attribute__((aligned(32)));
 uint32_t rate;
 uint32_t length,offset,lastBlock;
 uint64_t startMilliseconds;
 bool active;
} CallStream;
typedef enum {CALL_IDLE,CALL_RUNNING,CALL_FINISHED,CALL_UNDERRUN,CALL_READ_ERROR} CallResult;
bool callStreamOpen(CallStream *s,FILE *file,uint64_t now);
void callStreamClose(CallStream *s);
CallResult callStreamUpdate(CallStream *s,uint64_t now);
bool callStreamResume(CallStream *s,uint64_t now);

bool callStreamOpenRate(CallStream *s,FILE *file,uint64_t now,unsigned rate);
