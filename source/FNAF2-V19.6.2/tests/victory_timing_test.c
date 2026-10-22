#include "victory_timing.h"
#include <assert.h>
int main(void) {
    assert(victoryTopFrame(0)==0);
    assert(victoryTopFrame(4499)==0);
    assert(victoryTopFrame(4500)==1);
    assert(victoryTopFrame(8999)==8);
    assert(victoryTopFrame(9000)==9);
    assert(victoryTopFrame(9300)==20); /* B, confetti frame 3 */
    assert(victoryTopFrame(9600)==15); /* A, confetti frame 6 */
    assert(victoryBottomFrame(8999)==0);
    assert(victoryBottomFrame(9000)==0);
    assert(victoryBottomFrame(9400)==4);
    assert(victoryBottomFrame(10399)==13);
    assert(victoryBottomFrame(10400)==14);
    assert(victoryBottomFrame(10900)==15);
    assert(victoryBottomFrame(11400)==14);
    assert(!victoryCanContinue(10399));assert(victoryCanContinue(10400));
    for(unsigned ms=0;ms<120000;ms++){
        assert(victoryTopFrame(ms)<25);assert(victoryBottomFrame(ms)<16);
    }
    return 0;
}
