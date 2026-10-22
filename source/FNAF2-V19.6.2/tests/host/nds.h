#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
typedef uint16_t u16;
static inline void dmaFillHalfWords(uint16_t v,void *p,size_t n){uint16_t *d=p;for(size_t i=0;i<n/2;i++)d[i]=v;}
