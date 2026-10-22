#include "end_ui.h"
void endUIReset(EndUITouch *s){s->armed=false;}
EndUIAction endUITouch(EndUITouch *s,bool held,bool pressed,int x,int y,bool ready){
 if(!held){s->armed=true;return END_UI_NONE;}
 if(!pressed || !s->armed || !ready)return END_UI_NONE;
 s->armed=false; /* One action per press; release required after entry/activation. */
 if(y<150 || y>=186)return END_UI_NONE;
 if(x>=8 && x<124)return END_UI_PRIMARY;
 if(x>=132 && x<248)return END_UI_MENU;
 return END_UI_NONE;
}
unsigned endUIContinueNight(unsigned n){return n>=1 && n<7?n+1:7;}
