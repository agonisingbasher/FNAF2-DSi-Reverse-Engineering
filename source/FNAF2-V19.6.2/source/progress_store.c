#ifdef DEV_BUILD
#define SAVE_PREFIX "FNAF2-DEV-PROGRESS."
#else
#define SAVE_PREFIX "FNAF2-PROGRESS."
#endif
#include "progress_store.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
static bool join(char *out,const char *dir,const char *name) {
 int n=snprintf(out,SAVE_PATH_SIZE,"%s%s%s",dir,dir[0] && dir[strlen(dir)-1]!='/'?"/":"",name);
 return n>=0 && n<SAVE_PATH_SIZE;
}
bool progressStoreInit(ProgressStore *s,const char *rom,const char *cwd) {
 memset(s,0,sizeof(*s));char path[SAVE_PATH_SIZE];
 if(!cwd || !*cwd)cwd="/";
 if(rom && *rom) {
  if(rom[0]=='/' || strchr(rom,':')) {if(strlen(rom)>=sizeof(path))return false;strcpy(path,rom);}
  else if(!join(path,cwd,rom))return false;
  char *slash=strrchr(path,'/');
  if(!slash)return false;
  slash[1]=0;
 } else {if(strlen(cwd)>=sizeof(path))return false;strcpy(path,cwd);}
 strcpy(s->parent,path);
 char *colon=strchr(path,':');
 if(colon){size_t n=(size_t)(colon-path)+1;if(n+2>sizeof(s->root))return false;memcpy(s->root,path,n);s->root[n]='/';s->root[n+1]=0;}
 else strcpy(s->root,"/");
 if(!join(s->directory,s->parent,"saves"))return false;
 struct stat info;
 if(stat(s->directory,&info)==0)return S_ISDIR(info.st_mode);
 return mkdir(s->directory,0777)==0;
}
static bool readSave(const char *dir,const char *name,Progress *out) {
 char path[SAVE_PATH_SIZE];if(!join(path,dir,name))return false;
 FILE *f=fopen(path,"rb");if(!f)return false;
 unsigned char bytes[23];size_t n=fread(bytes,1,sizeof(bytes),f);fclose(f);
 if(n<5 || memcmp(bytes,"F2SV",4))return false;
 unsigned payload=bytes[4]==1 && n==20?18:bytes[4]==2 && n==22?20:0;
 if(!payload)return false;
 unsigned sum=0;for(unsigned i=0;i<payload;i++)sum+=bytes[i];
 if(sum!=(unsigned)(bytes[payload]|bytes[payload+1]<<8))return false;
 Progress p={0};p.night=bytes[5];p.stars=bytes[6];p.cheats=bytes[7];memcpy(p.ai,bytes+8,AI_COUNT);
 if(payload==20)p.rewards=bytes[18]|bytes[19]<<8;
 if(!progressValid(&p))return false;
 *out=p;return true;
}
bool progressStoreLoad(const ProgressStore *s,Progress *p,bool *migrated) {
 *migrated=false;
 if(readSave(s->directory,SAVE_PREFIX "dat",p) || readSave(s->directory,SAVE_PREFIX "bak",p))return true;
 if(readSave(s->parent,SAVE_PREFIX "dat",p) || readSave(s->parent,SAVE_PREFIX "bak",p) ||
    readSave(s->root,SAVE_PREFIX "dat",p) || readSave(s->root,SAVE_PREFIX "bak",p)) {
  *migrated=true;return true;
 }
 return false;
}
bool progressStoreSave(const ProgressStore *s,const Progress *p) {
 if(!progressValid(p))return false;
 char dat[SAVE_PATH_SIZE],bak[SAVE_PATH_SIZE],tmp[SAVE_PATH_SIZE];
 if(!join(dat,s->directory,SAVE_PREFIX "dat") || !join(bak,s->directory,SAVE_PREFIX "bak") || !join(tmp,s->directory,SAVE_PREFIX "tmp"))return false;
 unsigned char bytes[22]={'F','2','S','V',2,p->night,p->stars,p->cheats};
 memcpy(bytes+8,p->ai,AI_COUNT);bytes[18]=p->rewards&255;bytes[19]=p->rewards>>8;
 unsigned sum=0;for(unsigned i=0;i<20;i++)sum+=bytes[i];bytes[20]=sum&255;bytes[21]=sum>>8;
 FILE *f=fopen(tmp,"wb");if(!f)return false;
 bool ok=fwrite(bytes,1,sizeof(bytes),f)==sizeof(bytes);if(fclose(f)!=0)ok=false;if(!ok)return false;
 /* Never replace a valid backup with an invalid main save. */
 Progress old;bool hadOld=false;
 if(readSave(s->directory,SAVE_PREFIX "dat",&old)) {
  struct stat info;if(stat(bak,&info)==0 && remove(bak)!=0)return false;
  if(rename(dat,bak)!=0)return false;
  hadOld=true;
 } else {struct stat info;if(stat(dat,&info)==0 && remove(dat)!=0)return false;}
 if(rename(tmp,dat)==0)return true;
 if(hadOld)rename(bak,dat);
 return false;
}
