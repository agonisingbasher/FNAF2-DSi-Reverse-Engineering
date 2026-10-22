#pragma once
#include <stdint.h>
struct LoadingTiming {
 uint32_t poseUs=0,scrollFraction=0,effectsUs=0;
 unsigned pose=0,offsetX=0,row=0;
};
inline void loadingAdvance(LoadingTiming &s,uint32_t deltaUs){
 uint64_t poseTotal=uint64_t(s.poseUs)+deltaUs;
 s.pose^=unsigned(poseTotal/125000)&1u;
 s.poseUs=unsigned(poseTotal%125000);
 uint64_t motion=uint64_t(s.scrollFraction)+uint64_t(deltaUs)*120;
 uint64_t x=uint64_t(s.offsetX)+motion/1000000;
 s.scrollFraction=unsigned(motion%1000000);
 s.row=(s.row+unsigned(x/864))%3;
 s.offsetX=unsigned(x%864);
 s.effectsUs=unsigned((uint64_t(s.effectsUs)+deltaUs)%9600000);
}
