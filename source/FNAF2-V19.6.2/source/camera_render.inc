static void put(u16 *target,const u16 *pixels,unsigned w,unsigned h,int x,int y){
 for(unsigned sy=0;sy<h;sy++)for(unsigned sx=0;sx<w;sx++){int dx=x+sx,dy=y+sy;u16 color=pixels[sy*w+sx];if(dx>=0 && dx<256 && dy>=0 && dy<192 && (color&32768))target[dy*256+dx]=color;}
}
static void putMono(u16 *target,const u8 *pixels,unsigned w,unsigned h,int x,int y){
 for(unsigned sy=0;sy<h;sy++)for(unsigned sx=0;sx<w;sx++){unsigned p=sy*w+sx;if(pixels[p>>3]&(1u<<(p&7))){int dx=x+sx,dy=y+sy;if(dx>=0 && dx<256 && dy>=0 && dy<192)target[dy*256+dx]=0xffff;}}
}
static void noiseRender(u16 *target,unsigned frame,bool blend){
 for(unsigned y=0;y<192;y++)for(unsigned x=0;x<256;x++){
  unsigned n=(y/2)*128+x/2;u8 packed=cameraNoise[frame%4][n/2];unsigned gray=((n&1)?packed&15:packed>>4)*31/15;
  unsigned at=y*256+x;u16 old=target[at];unsigned red=gray,green=gray,blue=gray;
  if(blend){red=(3*(old&31)+gray)/4;green=(3*((old>>5)&31)+gray)/4;blue=(3*((old>>10)&31)+gray)/4;}
  target[at]=32768|red|(green<<5)|(blue<<10);
 }
}
static void actor(unsigned id,int x,int y,bool translucent){
 if(cachedActor!=(int)id){char name[32];snprintf(name,sizeof(name),"actor_%02u",id);if(!loadImage(name,actorSprite,128*128))return;cachedActor=id;}
 if(!translucent){blit(actorSprite,128,128,x,y);return;}
 for(unsigned sy=0;sy<128;sy++)for(unsigned sx=0;sx<128;sx++){
  int dx=x+sx,dy=y+sy;u16 color=actorSprite[sy*128+sx];
  if(dx>=0 && dx<256 && dy>=0 && dy<192 && (color&32768)){
   u16 old=screen[dy*256+dx];unsigned r=((old&31)*3+(color&31)*2)/5,g=(((old>>5)&31)*3+((color>>5)&31)*2)/5,b=(((old>>10)&31)*3+((color>>10)&31)*2)/5;screen[dy*256+dx]=32768|r|(g<<5)|(b<<10);
  }
 }
}
static bool loadCameraScene(unsigned cameraId,unsigned frame){
 char name[40];snprintf(name,sizeof(name),"cam_%02u_%02u.bin",cameraId,frame);
 FILE *f=openResource(name);if(!f)return false;
 u16 palette[256];u8 row[400];
 bool ok=fread(palette,sizeof(u16),256,f)==256;
 for(unsigned y=0;ok&&y<192;y++){
  if(fread(row,1,sizeof(row),f)!=sizeof(row)){ok=false;break;}
  for(unsigned x=0;x<400;x++)scene[y*400+x]=palette[row[x]];
 }
 fclose(f);return ok;
}
static void cameraRender(u16 *vram){
 u64 now=officeElapsedMilliseconds();unsigned frame=nightCameraFrame(&ai,camera.selected,room.cameraLight);
 int key=20000+(int)camera.selected*100+(int)frame;
 if(cachedScene!=key){if(loadCameraScene(camera.selected,frame))cachedScene=key;else memset(scene,0,sizeof(scene));}
 unsigned pan=cameraPan(camera.selected,now-camera.changedAt);
 for(unsigned y=0;y<192;y++)memcpy(screen+y*256,scene+y*400+pan,256*sizeof(u16));
 /* Mangle's real Kid's Cove still is part of CAM12 frame 2; no static overlays
    are added to other camera feeds. */
 if(now<ai.ghostUntil)actor(6+ai.ghostFrame,64,32,true);
 bool interrupted=now<ai.signalUntil;noiseRender(screen,(unsigned)(now/50)%6,!interrupted);if(interrupted)put(screen,camSignal,136,10,60,91);
 if((now/500)%2==0)put(screen,camRec,8,8,8,8);
 if(ai.music<=15000 && (now/250)%2==0)put(screen,camAlert,14,13,235,173);
 DC_FlushRange(screen,sizeof(screen));dmaCopy(screen,vram,sizeof(screen));
}
static void deathRender(u16 *vram){
 unsigned who=ai.killer<11?ai.killer:10,count=jumpCounts[who];if(deathFrame>=count)return;
 int key=50000+who*100+deathFrame;if(cachedOverlay!=key){char name[40];snprintf(name,sizeof(name),"jump_%02u_%02u",who,deathFrame);if(loadImage(name,overlay,256*192))cachedOverlay=key;deathShownAt=officeElapsedMilliseconds();}
 DC_FlushRange(overlay,sizeof(overlay));dmaCopy(overlay,vram,sizeof(overlay));if(officeElapsedMilliseconds()-deathShownAt>=65)++deathFrame;
}
