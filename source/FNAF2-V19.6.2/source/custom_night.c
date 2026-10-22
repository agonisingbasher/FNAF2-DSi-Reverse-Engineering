#include "custom_night.h"
const char *customPresetNames[CUSTOM_PRESETS]={"CUSTOM","CUPCAKE CHALLENGE","NEW AND SHINY","20 20 20 20","GOLDEN FREDDY"};
void customPreset(uint8_t *v,unsigned p){
 if(!p || p>=CUSTOM_PRESETS)return;
 for(unsigned i=0;i<10;i++)v[i]=p==1?5:p==2?(i<5?10:0):20;
}
unsigned customMove(unsigned s,int dx,int dy){
 int col=(int)(s%5)+dx,row=(int)(s/5)+dy;
 return (unsigned)((row+2)%2*5+(col+5)%5);
}
void customAdjust(uint8_t *v,unsigned s,int d){
 if(s>=10)return;
 int n=(int)v[s]+d;v[s]=(uint8_t)(n<0?0:n>20?20:n);
}
