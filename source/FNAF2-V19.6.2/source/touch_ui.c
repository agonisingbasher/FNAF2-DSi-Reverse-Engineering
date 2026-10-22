#include "touch_ui.h"
#include "touch_labels.h"
#define COL(r,g,b) ((u16)(0x8000|(r)|((g)<<5)|((b)<<10)))
EndUIAction touchUIHit(int x,int y){
 if(y<150 || y>=190)return END_UI_NONE;
 if(x>=8 && x<126)return END_UI_PRIMARY;
 if(x>=132 && x<250)return END_UI_MENU;
 return END_UI_NONE;
}
static void outline(u16 *p,int x,int y,int w,int h,u16 color){
 for(int i=x;i<x+w;i++){p[y*256+i]=color;p[(y+h-1)*256+i]=color;}
 for(int j=y;j<y+h;j++){p[j*256+x]=color;p[j*256+x+w-1]=color;}
}
void touchUIDraw(u16 *p,bool victory,EndUIAction selected){
 for(unsigned which=0;which<2;which++){
  int x=which?132:8;bool active=selected==(which?END_UI_MENU:END_UI_PRIMARY);
  u16 color=active?COL(5,27,31):COL(20,21,22);
  /* Entire face is repainted: pre-baked cyan, including an Idle cell, cannot leak. */
  for(int y=150;y<190;y++)for(int px=x;px<x+118;px++)p[y*256+px]=COL(3,4,5);
  outline(p,x,150,118,40,COL(7,8,8));outline(p,x+2,152,114,36,color);outline(p,x+5,155,108,30,color);
  /* Small dark rivets and pale corner scratches retain a mechanical plate style. */
  p[157*256+x+8]=COL(9,10,10);p[182*256+x+109]=COL(9,10,10);
  const unsigned char *mask=which?label_menu:victory?label_continue:label_retry;
  for(unsigned i=0;i<1200;i++)if(mask[i/8]&(1u<<(i%8)))p[(164+i/100)*256+x+9+i%100]=color;
 }
}
