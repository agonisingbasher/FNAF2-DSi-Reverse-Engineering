void officeUpdate(u32 down,u32 held){
 if(room.dead){beginDeath();return;}
 u64 now=officeElapsedMilliseconds();officeAdvanceClock(&room,now);if(room.complete){officeClose();return;}
 unsigned active=soundGetActiveChannels();updateCall(active);
 for(unsigned i=0;i<AUDIO_COUNT;i++)if(channels[i]>=0 && !(active&(1u<<channels[i])))channels[i]=-1;
 if(fxChannel>=0 && !(active&(1u<<fxChannel)))fxChannel=-1;
 bool oldMask=room.maskTarget,oldMonitor=room.monitorTarget,mode=room.monitorFrame || room.monitorTarget;
 bool hornPressed=(down&KEY_UP) && !mode && officeUpIsHorn(&room);
 if((down&KEY_UP) && !hornPressed)officePanelApply(&room,PANEL_MUTE,now);
 touchPosition touch={0};if(held&KEY_TOUCH)touchRead(&touch);
 if(mode){
  if(cameraAdvanceClose(&camera,now))officeToggleMonitor(&room);
  if(down&KEY_DOWN){camera.closeStage=0;officeToggleMonitor(&room);}
  if(officeCameraMode() && !camera.closeStage){
   unsigned next=camera.selected;
   if(down&KEY_X)next=cameraNeighbor(next,0,-1);
   else if(down&KEY_Y)next=cameraNeighbor(next,-1,0);
   else if(down&KEY_A)next=cameraNeighbor(next,1,0);
   else if(down&KEY_B)next=cameraNeighbor(next,0,1);
   if((down&KEY_TOUCH) && camera.noiseStep>=6){unsigned hit=cameraHit(touch.px,touch.py);if(hit)next=hit;else if(cameraTabletHit(touch.px,touch.py))cameraRequestClose(&camera);else if(cameraLightHit(touch.px,touch.py))camera.touchLight=!camera.touchLight;}
   if(next!=camera.selected && !ai.bbDisabled){playVoice("blip.raw",55);}
   cameraSelect(&camera,next,now);
  }
 }else{
  if(down&KEY_B)officePanelApply(&room,PANEL_MASK,now);else if(down&KEY_DOWN)officePanelApply(&room,PANEL_TABLET,now);
  if(down&KEY_TOUCH){unsigned frame=cachedPanel>=0?(unsigned)cachedPanel:officePanelFrame(&room,now);officePanelApply(&room,officePanelHit(frame,touch.px,touch.py),now);}
 }
 if(!oldMonitor && room.monitorTarget){camera.noiseStep=0;camera.touchLight=false;camera.closeStage=0;camera.changedAt=now;}
 if(oldMonitor && !room.monitorTarget){camera.touchLight=false;cachedPanel=-1;panelExitNoise=0;}
 /* SELECT debug actions are handled in main before normal gameplay input. */
 if(room.muted)stopCall();
 room.cameraLight=officeCameraMode() && room.batteryMilli>0 && !ai.bbDisabled && ((held&KEY_R) || camera.touchLight);
 bool hallInputHeld=!mode && ((held&KEY_X) || room.touchLight==PANEL_HALL),lockFeedback=false;
 bool hallLocked=officeHallLightLockUpdate(&room,now,hallInputHeld,&lockFeedback);
 if(lockFeedback)playPopstatic();
 if(hallLocked && room.touchLight==PANEL_HALL)room.touchLight=PANEL_NONE;
 officeTick(&room,!mode && (held&KEY_X),!mode && (held&KEY_LEFT),!mode && (held&KEY_RIGHT),!mode && (held&KEY_Y),!mode && (held&KEY_A));
 officeAdvanceBattery(&room,now);
 bool wind=officeCameraMode() && camera.selected==11 && !camera.closeStage && ((held&KEY_L) || ((held&KEY_TOUCH) && cameraWindHit(touch.px,touch.py)));
 room.blockedLight=!mode && !room.maskFrame?((held&KEY_X) || room.touchLight==PANEL_HALL?PANEL_HALL:(held&KEY_Y) || room.touchLight==PANEL_LEFT?PANEL_LEFT:(held&KEY_A) || room.touchLight==PANEL_RIGHT?PANEL_RIGHT:0):0;
 nightAIUpdate(&ai,&room,now,camera.selected,wind,room.cameraLight);if(ai.bbDisabled)room.cameraLight=false;
 if(ai.hallExitEvent && room.centerLight){
  bool inputHeld=!mode && ((held&KEY_X) || room.touchLight==PANEL_HALL);
  officeHallLightLockStart(&room,now,inputHeld);room.centerLight=false;
  if(room.touchLight==PANEL_HALL)room.touchLight=PANEL_NONE;
  playPopstatic();
 }
 if(room.dead){beginDeath();return;}
 OfficeAudioPlan plan=officeAudioPlan(oldMask,oldMonitor,&room);
 bool signal=officeCameraMode() && (now<ai.signalUntil || (room.cameraLight && camera.selected==9 && ai.loc[TF]!=9));
 if(plan.buzz && !signal){if(channels[BUZZ]<0)playAudio(BUZZ,true,50);}else stopAudio(BUZZ);
 if(plan.maskOn || plan.maskOff){stopAudio(MASK_ON);stopAudio(MASK_OFF);playAudio(room.maskTarget?MASK_ON:MASK_OFF,false,95);}
 if(plan.monitorOn || plan.monitorOff){stopAudio(MONITOR_ON);stopAudio(MONITOR_OFF);playAudio(room.monitorTarget?MONITOR_ON:MONITOR_OFF,false,95);}
 if((ai.bbDisabled && room.blockedLight==PANEL_HALL) || signal || (room.moving && !mode && (held&KEY_X))){if(channels[ERROR_SOUND]<0)playAudio(ERROR_SOUND,true,65);}else stopAudio(ERROR_SOUND);
 if(hornPressed && !room.maskFrame && !room.monitorFrame)playAudio(HORN,false,70);
 if(channels[FAN]>=0)soundSetVolume(channels[FAN],room.monitorFrame?12:60);
 if(room.maskFrame && channels[BREATH]<0)playAudio(BREATH,true,38);
 if(!room.maskFrame)stopAudio(BREATH);
 updateOfficeAmbience();
 loopSet(&cameraLoop,officeCameraMode()?"camera_static.raw":NULL,8000,40);
 loopSet(&boxLoop,ai.puppetStage==4?"jack.raw":officeCameraMode() && (camera.selected==11 || camera.selected==10 || camera.selected==12 || camera.selected==9)?"box.raw":NULL,8192,ai.puppetStage==4?(now<ai.ghostUntil?104:90):camera.selected==11?70:25);
 loopSet(&mangleLoop,(officeCameraMode() && ai.loc[MG]==camera.selected) || ai.loc[MG]==6 || ai.loc[MG]==RIGHT_DOOR || ai.loc[MG]==CEILING?"mangle.raw":NULL,8192,ai.loc[MG]>=RIGHT_DOOR || ai.loc[MG]==6?127:65);
 if(ai.bbDisabled && room.blockedLight && room.blockedLight!=PANEL_HALL){if(channels[BB_ERROR]<0)playAudio(BB_ERROR,true,70);}else stopAudio(BB_ERROR);
 if(wind && ai.puppetStage<3){if(channels[WIND_SOUND]<0)playAudio(WIND_SOUND,true,80);}else stopAudio(WIND_SOUND);
 if(ai.ventEvents && channels[VENT_SOUND]<0){playAudio(VENT_SOUND,false,95);--ai.ventEvents;}
 if(!nightFootstepsAudible(officeCameraMode(),camera.selected)){
  if(movementStream.stream.active)loopStop(&movementStream);
  memset(ai.pendingMoves,0,sizeof(ai.pendingMoves));
 }else{
  streamOnceUpdate(&movementStream);
  if(!movementStream.stream.active){
   for(unsigned j=0;j<9;j++){unsigned who=(ai.moveCursor+j)%9;if(!ai.pendingMoves[who])continue;
    const char *name=nightMovementAudio(who,(unsigned)rand());
    if(name)streamOnceBegin(&movementStream,name,8000,55);
    --ai.pendingMoves[who];ai.moveCursor=(who+1)%9;break;
   }
  }
 }
 updateHallLoop(nightHallwayAudioActive(&ai,&room));
 ai.hallEvent=false;
 bool stareAllowed=nightStareAllowed(&ai,&room,now);
 if(stareAllowed && !stareActive){streamOnceBegin(&stareStream,"stare.raw",8000,90);stareActive=true;}
 if(stareActive && !stareAllowed){loopStop(&stareStream);stareActive=false;}else streamOnceUpdate(&stareStream);
 ai.stareEvent=false;
 if(ai.bbDisabled && !bbLaughStarted){playVoice("bb3.raw",90);if(fxChannel>=0){soundKill(fxChannel);DC_FlushRange(fxBuffer,sizeof(fxBuffer));fxChannel=soundPlaySample(fxBuffer,SoundFormat_8Bit,bbLaughBytes,8000,90,64,true,0);}bbLaughStarted=true;}
 if(ai.bbVoice && !ai.bbDisabled){char name[20];snprintf(name,sizeof(name),"bb%u.raw",ai.bbVoice);playVoice(name,80);ai.bbVoice=0;}
}
