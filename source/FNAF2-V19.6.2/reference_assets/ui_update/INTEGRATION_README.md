# FNAF 2 V19.5 — DSi bottom UI patch resources

Four new **RGBA PNG** assets. The 224x16 buttons are sprite-sized source images, not full-screen frames. Background Game Over is **256x192**. All text is pre-rendered; no runtime font rendering is required.

## Files and layouts

Victory, after `elapsedMs>=9000`: keep original base assets (`night_complete`, `night_N_survived`, and for 5-7 the document) under the two new buttons. Draw `button_game_menu.png` at **(16,150)** and `button_tap_continue.png` at **(16,173)**. The old `tap_to_continue.png` is superseded. The original documents at (16,60) 224x104 end at y164 and would collide with Game Menu; change to **(16,60) 224x86**, or crop carefully if scaling is unsupported, keeping the main details visible. Alternately rebuild layout to preserve full documents and allocate another UI layer/scroll region. Never draw the original button as well.

Game Over: draw `gameover_bottom_bg.png` at **(0,0)**, `button_game_menu.png` at **(16,150)** and `button_retry_night.png` at **(16,173)**.

Both menus: only accept **new touches after a release**, never a held touch from the previous scene. Victory buttons activate only after the celebration fade has unlocked the screen (`celebrationMs>=1400`). This example makes both buttons visible at that time; caller must avoid invoking `victoryFrame` before the unlock if it needs the initial black/fade sequence. Save night progress and play existing transition audio via the **existing** game-state callbacks; `continueAfterWin` must not independently increment the saved night a second time. `retryNight` must restart the current failed night with its standard initialization; `gameMenu` exits through the project's existing main-menu state.

## C++ adapter

`FNAF2_UI_DSi.h/.cpp` is a libnds-oriented adapter. It is **not a patched engine**: this ZIP contains no V19.5 engine sources, and `DrawImage` is a callback to be implemented with the project's existing PNG-conversion and lower-screen renderer. Initialize `BottomUI` with your lower-screen draw function and your three game-state callbacks. Call `scanKeys()` exactly once in the main loop before calling `victoryFrame` or `gameOverFrame`. Reset touch using `resetTouch()` upon entering either screen. Do not call scanKeys twice. Do not delete or reinstall devkitARM. The lower screen may be hardware engine A or B depending on swap configuration; use the actual existing framebuffer pipeline.

For RAM, pre-convert PNG assets (e.g. RGB5551 / tiled BG/OBJ / existing built-in converter), cache just the needed assets and use already-allocated buffers. No runtime Pillow and no PNG decoding unless the engine already supports it. Avoid loading victory documents on Game Over.

## Install

Copy `assets/...` to the game's source asset tree; include the two C++ files in the build and wire its callbacks to the existing renderer and state transitions. Asset generation can be repeated with `python generate_ui.py` (requires Pillow). The script uses `/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf`; change `FONT` to an installed mono font on other computers. This ZIP includes only new UI art, not a compiled ROM.
