#include "FNAF2_UI_DSi.h"
namespace FnafUI {
bool BottomUI::newTouch(touchPosition& pos) {
    const bool down=(keysHeld() & KEY_TOUCH)!=0;
    if (!down) { touchHeld_=false; return false; }
    if (touchHeld_) return false;
    touchHeld_=true;
    touchRead(&pos);
    return true;
}
void BottomUI::victoryFrame(bool victoryReady) {
    // Call after base artwork (night_complete/night_N_survived/document) is drawn.
    // To avoid overlapping buttons, document's visible area must end at y<=148.
    if (draw_) {
        draw_("assets/victory/bottom/button_game_menu.png",16,150,224,16);
        draw_("assets/victory/bottom/button_tap_continue.png",16,173,224,16);
    }
    touchPosition p{};
    // Drain touches until the lower screen UI is unlocked by victory timeline.
    if (!victoryReady) { const bool down=(keysHeld() & KEY_TOUCH)!=0;
        if (down) touchHeld_=true; else touchHeld_=false; return; }
    if (!newTouch(p)) return;
    if (VictoryMenu.contains(p.px,p.py)) {
        if (actions_.gameMenu) actions_.gameMenu();
    } else if (VictoryContinue.contains(p.px,p.py)) {
        if (actions_.continueAfterWin) actions_.continueAfterWin();
    }
}
void BottomUI::gameOverFrame() {
    if (draw_) {
        draw_("assets/gameover/gameover_bottom_bg.png",0,0,256,192);
        draw_("assets/victory/bottom/button_game_menu.png",16,150,224,16);
        draw_("assets/gameover/button_retry_night.png",16,173,224,16);
    }
    touchPosition p{};
    if (!newTouch(p)) return;
    if (GameOverMenu.contains(p.px,p.py)) {
        if (actions_.gameMenu) actions_.gameMenu();
    } else if (GameOverRetry.contains(p.px,p.py)) {
        if (actions_.retryNight) actions_.retryNight();
    }
}
} // namespace FnafUI
