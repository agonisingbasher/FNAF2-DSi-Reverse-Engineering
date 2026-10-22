#pragma once
#include <nds.h>
#include <cstdint>

// Integration adapter: renderer must supply image loading/conversion and drawing.
// The names below are asset paths relative to the game's resource root.
namespace FnafUI {
constexpr int ScreenW = 256, ScreenH = 192;
struct Rect { int x, y, w, h; bool contains(int px,int py) const {
    return px>=x && py>=y && px<x+w && py<y+h;
}};
constexpr Rect VictoryMenu {16, 150, 224, 16};
constexpr Rect VictoryContinue {16, 173, 224, 16};
constexpr Rect GameOverMenu {16, 150, 224, 16};
constexpr Rect GameOverRetry {16, 173, 224, 16};

// "draw" must blit the prepared asset with its appropriate transparency.
// "drawDocument" may scale/crop existing 224x104 document into 224x86.
// If your engine cannot scale, crop only noncritical margins or redesign placement.
using DrawImage = void(*)(const char* asset, int x, int y, int width, int height);
using Transition = void(*)();

struct Actions { Transition gameMenu=nullptr; Transition continueAfterWin=nullptr;
                 Transition retryNight=nullptr; };

class BottomUI {
public:
    BottomUI(DrawImage render, Actions transitions) : draw_(render), actions_(transitions) {}
    // Call once per frame after scanKeys() has been called by the main loop.
    // victoryReady should become true only after celebrationMs >= 1400.
    void victoryFrame(bool victoryReady);
    void gameOverFrame();
    // Clear touch latch whenever changing screens and entering a state.
    void resetTouch() { touchHeld_=true; }
private:
    bool newTouch(touchPosition& pos);
    DrawImage draw_;
    Actions actions_;
    bool touchHeld_=true;
};
} // namespace FnafUI
