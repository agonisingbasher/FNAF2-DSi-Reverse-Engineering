#define _XOPEN_SOURCE 700
#include "progress_store.h"
#include "office_state.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc,char **argv) {
 char temp[]="/tmp/fnaf14-tests-XXXXXX";assert(mkdtemp(temp));
 ProgressStore s;char rom[512];snprintf(rom,sizeof(rom),"%s/FNAF2 DS.nds",temp);
 assert(progressStoreInit(&s,rom,"/"));assert(strstr(s.directory,"/saves"));
 assert(progressStoreInit(&s,rom,"/")); // Reuse existing directory.
 Progress p,q;progressReset(&p);p.night=4;p.stars=1;p.rewards=37;p.ai[2]=20;
 assert(progressStoreSave(&s,&p));bool migrated;assert(progressStoreLoad(&s,&q,&migrated));assert(!migrated && q.night==4 && q.rewards==37 && q.ai[2]==20);
 p.night=5;assert(progressStoreSave(&s,&p));
 char path[512];snprintf(path,sizeof(path),"%s/saves/FNAF2-PROGRESS.dat",temp);FILE *f=fopen(path,"wb");assert(f);fputs("damaged",f);fclose(f);
 assert(progressStoreLoad(&s,&q,&migrated));assert(q.night==4); // Valid backup survives damaged main.
 assert(progressStoreSave(&s,&q));assert(progressStoreLoad(&s,&q,&migrated));assert(q.night==4);
 char migration[]="/tmp/fnaf14-migration-XXXXXX";assert(mkdtemp(migration));
 snprintf(rom,sizeof(rom),"%s/FNAF2.nds",migration);assert(progressStoreInit(&s,rom,"/"));
 snprintf(path,sizeof(path),"%s/FNAF2-PROGRESS.dat",migration);
 FILE *original=fopen(argc>1?argv[1]:"tests/fixtures/night1-v2.dat","rb");assert(original);f=fopen(path,"wb");assert(f);int c;while((c=fgetc(original))!=EOF)fputc(c,f);fclose(f);fclose(original);
 assert(progressStoreLoad(&s,&q,&migrated));assert(migrated && q.night==1);
 assert(progressStoreSave(&s,&q));assert(access(path,F_OK)==0);assert(progressStoreLoad(&s,&q,&migrated));assert(!migrated);
 assert(progressStoreInit(&s,"FNAF2.nds",migration));assert(progressStoreInit(&s,NULL,migration));
 Office o;officeReset(&o,1);
 const uint32_t bus=33513982;
 for(unsigned hour=1;hour<=6;hour++) {
  uint32_t count=(uint32_t)(((uint64_t)hour*89000*bus+1023999)/1024000);
  officeAdvanceClock(&o,officeCounterMilliseconds(count-1,bus));assert(o.hour==hour-1 && !o.complete);
  officeAdvanceClock(&o,officeCounterMilliseconds(count,bus));assert(o.hour==hour && o.complete==(hour==6));
 }
 officeReset(&o,1);officeToggleMask(&o);
 for(unsigned i=0;i<5;i++){officeTick(&o,false,false,false,false,false);}assert(o.maskFrame==10);
 officeToggleMask(&o);for(unsigned i=0;i<5;i++)officeTick(&o,false,false,false,false,false);assert(o.maskFrame==0);
 officeToggleMonitor(&o);for(unsigned i=0;i<6;i++)officeTick(&o,false,false,false,false,false);assert(o.monitorFrame==11);
 officeToggleMonitor(&o);for(unsigned i=0;i<6;i++)officeTick(&o,false,false,false,false,false);assert(o.monitorFrame==0);
 for(unsigned i=0;i<57;i++){officePreview(&o);assert(o.preview<=18);}assert(o.preview==0);
 officeTick(&o,false,false,false,true,false);assert(o.leftLight && !o.rightLight);
 officeTick(&o,false,false,false,false,true);assert(o.rightLight && !o.leftLight);
 officeTick(&o,false,false,false,false,false);assert(!o.rightLight && !o.leftLight);
 return 0;
}
