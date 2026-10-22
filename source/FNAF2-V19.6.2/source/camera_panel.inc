bool officeRenderPanel(u16 *buffer){
 u64 now=officeElapsedMilliseconds();cameraMapVisible=false;
 if(room.dead){noiseRender(buffer,deathFrame%6,false);return true;}
 if(!room.monitorTarget && panelExitNoise<6){cachedPanel=-1;noiseRender(buffer,panelExitNoise++,false);return true;}
 if(room.monitorFrame || room.monitorTarget){
  cachedPanel=-1;if(camera.noiseStep<6 && room.monitorTarget){noiseRender(buffer,camera.noiseStep++,false);return true;}
  cameraMapVisible=true;unsigned pressed=camera.closeStage==1;int key=1000+pressed;
  if(cachedOverlay!=key){char name[24];snprintf(name,sizeof(name),"tablet_%u",pressed);if(loadImage(name,overlay,256*192))cachedOverlay=key;}
  memcpy(buffer,overlay,sizeof(overlay));
  for(unsigned i=0;i<12;i++){put(buffer,camBoxes[camera.selected==i+1],29,16,cameraBoxes[i].x,cameraBoxes[i].y);putMono(buffer,camNumbers[i],20,13,cameraBoxes[i].x+4,cameraBoxes[i].y+1);}
  putMono(buffer,camNames[camera.selected-1],132,12,15,173);unsigned battery=officeBatteryIcon(&room);put(buffer,batteries[battery],40,18,14,12);
  put(buffer,camFlash[room.cameraLight?1:0],33,20,215,141);
  if(camera.selected==11){put(buffer,camWindBoxes[channels[WIND_SOUND]>=0],62,20,145,140);unsigned wheel=ai.music*20/100000;put(buffer,camWheels[wheel],11,11,145,145);put(buffer,camWind,47,14,159,142);}
  if(ai.music<=30000 && (now/250)%2==0)put(buffer,camAlert,14,13,237,74);
  cameraClosePresented(&camera,now);return true;
 }
 unsigned frame=officePanelFrame(&room,now);if(cachedPanel==(int)frame){officePanelPresented(&room,frame,now);return false;}
 char name[32];snprintf(name,sizeof(name),"panel_%02u",frame);if(!loadImage(name,buffer,256*192))return false;cachedPanel=frame;officePanelPresented(&room,frame,officeElapsedMilliseconds());return true;
}
