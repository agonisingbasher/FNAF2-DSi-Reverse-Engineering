/* Bounded streaming buffers: no complete ambience/music track allocation. */
static void loopStop(LoopSound *l){if(l->channel>=0)soundKill(l->channel);l->channel=-1;callStreamClose(&l->stream);l->name=NULL;}
static void loopPlay(LoopSound *l,unsigned volume){DC_FlushRange(l->stream.ring,CALL_BUFFER);l->channel=soundPlaySample(l->stream.ring,SoundFormat_8Bit,CALL_BUFFER,l->rate,volume,64,true,0);l->stream.startMilliseconds=officeElapsedMilliseconds();if(l->channel<0)loopStop(l);}
static void loopSet(LoopSound *l,const char *name,unsigned rate,unsigned volume){
 if(!name){loopStop(l);return;}
 if(!l->name || strcmp(l->name,name)){
  loopStop(l);l->name=name;l->rate=rate;if(callStreamOpenRate(&l->stream,openResource(name),officeElapsedMilliseconds(),rate))loopPlay(l,volume);else loopStop(l);return;
 }
 unsigned block=l->stream.lastBlock;CallResult result=callStreamUpdate(&l->stream,officeElapsedMilliseconds());
 if(result!=CALL_RUNNING){
  if(l->channel>=0)soundKill(l->channel);
  l->channel=-1;bool ok=result==CALL_UNDERRUN?callStreamResume(&l->stream,officeElapsedMilliseconds()):callStreamOpenRate(&l->stream,openResource(name),officeElapsedMilliseconds(),rate);
  if(ok)loopPlay(l,volume);else loopStop(l);
 }else{if(block!=l->stream.lastBlock)DC_FlushRange(l->stream.ring,CALL_BUFFER);if(l->channel>=0)soundSetVolume(l->channel,volume);}
}
/* Only gameplay ambience varies; cutscene ambience remains independent. */
static void startOfficeAmbience(void){
 static const char *names[]={"ambience.raw","ambience_depths.raw"};
 unsigned which=ambienceChoice.current;
 loopSet(&ambienceLoop,names[which],which?8000:4000,55);
 if(!ambienceLoop.stream.active && which){ambienceChoice.current=0;loopSet(&ambienceLoop,names[0],4000,55);}
}
static void updateOfficeAmbience(void){
 if(!ambienceLoop.stream.active){startOfficeAmbience();return;}
 unsigned block=ambienceLoop.stream.lastBlock;
 CallResult result=callStreamUpdate(&ambienceLoop.stream,officeElapsedMilliseconds());
 if(result==CALL_RUNNING){if(block!=ambienceLoop.stream.lastBlock)DC_FlushRange(ambienceLoop.stream.ring,CALL_BUFFER);return;}
 if(ambienceLoop.channel>=0)soundKill(ambienceLoop.channel);
 ambienceLoop.channel=-1;
 if(result==CALL_UNDERRUN && callStreamResume(&ambienceLoop.stream,officeElapsedMilliseconds())){loopPlay(&ambienceLoop,55);return;}
 if(result==CALL_FINISHED)ambienceAdvance(&ambienceChoice,true);
 loopStop(&ambienceLoop);startOfficeAmbience();
}
static void playVoice(const char *name,unsigned volume){
 if(fxChannel>=0)soundKill(fxChannel);
 fxChannel=-1;FILE *f=openResource(name);if(!f)return;unsigned n=fread(fxBuffer,1,sizeof(fxBuffer),f);fclose(f);n&=~3u;bbLaughBytes=n;if(!n)return;
 DC_FlushRange(fxBuffer,n);fxChannel=soundPlaySample(fxBuffer,SoundFormat_8Bit,n,8000,volume,64,false,0);
}
static void beginDeath(void){if(deathStarted)return;officeClose();deathStarted=true;deathFrame=0;deathShownAt=0;playVoice("scream.raw",127);}
void officeConfigureAI(const uint8_t *levels,bool cheats){nightAIReset(&ai,room.night,levels,cheats,readNightTimer()^0xFA216u);room.aiManaged=true;ai.presentationTracking=true;}
bool officeCameraMode(void){return room.monitorFrame==11 && room.monitorTarget;}
bool officeCameraMapVisible(void){return cameraMapVisible;}
unsigned officeCameraSelected(void){return camera.selected;}
bool officeDeathFinished(void){unsigned who=ai.killer<11?ai.killer:10;return deathStarted && deathFrame>=jumpCounts[who];}

static void streamOnceBegin(LoopSound *l,const char *name,unsigned rate,unsigned volume){loopStop(l);l->rate=rate;l->name=name;if(callStreamOpenRate(&l->stream,openResource(name),officeElapsedMilliseconds(),rate))loopPlay(l,volume);else loopStop(l);}
static void streamOnceUpdate(LoopSound *l){
 if(!l->stream.active)return;
 unsigned block=l->stream.lastBlock;CallResult result=callStreamUpdate(&l->stream,officeElapsedMilliseconds());
 if(result==CALL_UNDERRUN){if(l->channel>=0)soundKill(l->channel);l->channel=-1;if(callStreamResume(&l->stream,officeElapsedMilliseconds()))loopPlay(l,90);else loopStop(l);}
 else if(result!=CALL_RUNNING)loopStop(l);
 else if(block!=l->stream.lastBlock)DC_FlushRange(l->stream.ring,CALL_BUFFER);
}

void officeShowAsset(const char *name,u16 *vram){if(!nitroReady)nitroReady=nitroFSInit(NULL);if(loadImage(name,overlay,256*192)){DC_FlushRange(overlay,sizeof(overlay));dmaCopy(overlay,vram,sizeof(overlay));}cachedOverlay=-1;}
void officeEndAudioBegin(bool won){endCrowd=false;if(won)streamOnceBegin(&stareStream,"chime.raw",8000,100);else loopSet(&boxLoop,"gameover_static.raw",8000,55);}
void officeEndAudioUpdate(bool won,unsigned elapsed){if(!won){loopSet(&boxLoop,"gameover_static.raw",8000,55);return;}streamOnceUpdate(&stareStream);if(elapsed>=9000 && !endCrowd){streamOnceBegin(&boxLoop,"crowd.raw",8000,100);endCrowd=true;}streamOnceUpdate(&boxLoop);}
void officeCutsceneBegin(unsigned kind){
 (void)kind;officeClose();if(!nitroReady)nitroReady=nitroFSInit(NULL);
 startNightTimer();cachedScene=cachedOverlay=-1;deskBorrowed=true;
 loadRaw("dream_mask.u16",overlay,sizeof(overlay));
 loadRaw("dream_golden.u16",(u16*)desk,115*180*2);
 loadRaw("dream_puppet.u16",(u16*)desk+115*180,139*192*2);
 soundEnable();loopSet(&ambienceLoop,"dream_ambience.raw",8000,40);
}
void officeCutsceneRender(unsigned state,int pan,bool golden,bool puppet,u16 *vram){
 if(cachedScene!=(int)state){char name[32];snprintf(name,sizeof(name),"dream_room_%u",state);if(loadImage(name,scene,400*192))cachedScene=state;}
 for(unsigned y=0;y<192;y++)memcpy(screen+y*256,scene+y*400+pan,256*2);
 if(golden)blit((u16*)desk,115,180,70,12);
 if(puppet)blit((u16*)desk+115*180,139,192,58,0);
 blit(overlay,256,192,0,0);DC_FlushRange(screen,sizeof(screen));dmaCopy(screen,vram,sizeof(screen));
}
void officeCutsceneAudio(unsigned kind,unsigned frames,bool moving){
 if(frames>=959){loopStop(&ambienceLoop);loopStop(&boxLoop);loopStop(&movementStream);loopStop(&mangleLoop);loopStop(&hallStream);loopStop(&stareStream);if(kind==4)playVoice("blip.raw",100);return;}
 loopSet(&ambienceLoop,"dream_ambience.raw",8000,40);
 loopSet(&movementStream,moving?"dream_turn.raw":NULL,8000,45);
 if(kind==4)loopSet(&mangleLoop,"Robot.raw",8000,75);
 else{static const char letters[]="ITISME";if(frames%90==0){char name[32];snprintf(name,sizeof(name),"dream_letter_%c.raw",letters[(frames/90)%6]);streamOnceBegin(&stareStream,name,8000,65);}streamOnceUpdate(&stareStream);}
}
