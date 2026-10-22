#include "end_ui.h"
#include <assert.h>
int main(void){
 EndUITouch s;endUIReset(&s);
 assert(endUITouch(&s,true,true,20,160,true)==END_UI_NONE);
 assert(endUITouch(&s,false,false,0,0,true)==END_UI_NONE);
 assert(endUITouch(&s,true,true,8,150,false)==END_UI_NONE);
 assert(endUITouch(&s,true,true,8,150,true)==END_UI_PRIMARY);
 assert(endUITouch(&s,true,true,140,160,true)==END_UI_NONE);
 endUITouch(&s,false,false,0,0,true);
 assert(endUITouch(&s,true,true,247,185,true)==END_UI_MENU);
 const int miss[][2]={{7,150},{248,150},{128,160},{20,149},{20,186},{-1,160},{20,192}};
 for(unsigned i=0;i<sizeof(miss)/sizeof(miss[0]);i++){endUIReset(&s);endUITouch(&s,false,false,0,0,true);assert(endUITouch(&s,true,true,miss[i][0],miss[i][1],true)==END_UI_NONE);}
 for(unsigned n=1;n<7;n++)assert(endUIContinueNight(n)==n+1);
 assert(endUIContinueNight(7)==7);
 return 0;
}
