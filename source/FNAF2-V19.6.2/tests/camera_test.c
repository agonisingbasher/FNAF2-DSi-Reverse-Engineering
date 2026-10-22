#include "camera_state.h"
#include <assert.h>
int main(void){
 CameraState c;cameraReset(&c);assert(c.selected==9);unsigned reach=1u<<8;
 for(unsigned pass=0;pass<12;pass++)for(unsigned i=1;i<=12;i++)if(reach&(1u<<(i-1))){const int d[4][2]={{0,-1},{-1,0},{1,0},{0,1}};for(unsigned k=0;k<4;k++){unsigned n=cameraNeighbor(i,d[k][0],d[k][1]);assert(n>=1 && n<=12);reach|=1u<<(n-1);if(n!=i)assert((cameraBoxes[n-1].x-cameraBoxes[i-1].x)*d[k][0]+(cameraBoxes[n-1].y-cameraBoxes[i-1].y)*d[k][1]>0);}}
 assert(reach==4095);
 for(unsigned i=0;i<12;i++)for(int y=cameraBoxes[i].y;y<cameraBoxes[i].y+16;y++)for(int x=cameraBoxes[i].x;x<cameraBoxes[i].x+29;x++){assert(cameraHit(x,y)==i+1);assert(!cameraLightHit(x,y));assert(!cameraTabletHit(x,y));assert(!cameraWindHit(x,y));}
 for(int y=0;y<192;y++)for(int x=0;x<256;x++)assert(!(cameraWindHit(x,y) && (cameraLightHit(x,y)||cameraTabletHit(x,y))));
 assert(cameraNeighbor(1,0,-1)==3);assert(cameraNeighbor(3,0,1)==1);assert(cameraNeighbor(1,1,0)==2);assert(cameraNeighbor(2,-1,0)==1);
 cameraRequestClose(&c);assert(c.closeStage==1);assert(!cameraAdvanceClose(&c,9000));cameraClosePresented(&c,9000);assert(!cameraAdvanceClose(&c,9079));assert(!cameraAdvanceClose(&c,9080));assert(c.closeStage==2);assert(!cameraAdvanceClose(&c,20000));cameraClosePresented(&c,20000);assert(!cameraAdvanceClose(&c,20049));assert(cameraAdvanceClose(&c,20050));assert(!c.closeStage);
 cameraSelect(&c,12,5);assert(c.selected==12 && c.changedAt==5);cameraSelect(&c,13,10);assert(c.selected==12 && c.changedAt==5);
 for(unsigned i=1;i<=6;i++)for(unsigned t=0;t<40000;t+=9)assert(cameraPan(i,t)==(i==1?48:i==2?96:72));
 assert(cameraPan(10,999)==0);assert(cameraPan(10,1900)==72);assert(cameraPan(10,2800)==144);assert(cameraPan(10,4700)==72);assert(cameraPan(10,5600)==0);
 for(unsigned i=1;i<=12;i++)for(unsigned t=0;t<40000;t++)assert(cameraPan(i,t)<=144);
 return 0;
}
