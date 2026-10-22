#pragma once
#include <stdint.h>
#include <stdbool.h>
enum { VICTORY_FRAME_BYTES=512+256*192 };
static inline unsigned victoryTopFrame(uint32_t ms) {
    if(ms<4500)return 0;
    if(ms<9000)return 1+(ms-4500)*8/4500;
    uint32_t t=ms-9000;
    return 9+((t/300)%2)*8+(t/100)%8;
}
static inline unsigned victoryBottomFrame(uint32_t ms) {
    if(ms<9000)return 0;
    uint32_t t=ms-9000;
    if(t<1400)return t/100;
    return ((t-1400)/500)%2?15:14;
}
static inline bool victoryCanContinue(uint32_t ms) {return ms>=10400;}
