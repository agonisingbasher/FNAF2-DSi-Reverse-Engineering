#include "loading.h"
#include "custom_night.h"
#include "custom_portraits.h"
#include "cutscene_state.h"
#include <nds.h>
#include <filesystem.h>
#include <calico/system/tick.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "progress_store.h"


#include "contract_state.h"

#include "game_state.h"
#include "office.h"
#include "victory.h"
#include "end_ui.h"
#include "gameover_state.h"
#include "gameover_render.h"
#include "victory_interaction.h"


#define FRAME_PIXELS (256 * 192)
#define FRAME_BYTES  (FRAME_PIXELS * sizeof(u16))
#define GLITCH_MIN_TICKS 300    /* 5 seconds */
#define GLITCH_SPAN_TICKS 600   /* up to another 10 seconds */
#define SIGN_X0 55
#define SIGN_X1 201
#define SIGN_Y0 99
#define SIGN_Y1 124

static int topBitmap;
static int bottomBitmap;
static u16 bottomBuffer[FRAME_PIXELS];
static Progress progress;
static unsigned selectedOption = 0;
static bool storageReady = false;
static int musicChannel = -1;
static u8 *titleMusicBuffer;
#define TITLE_MUSIC_BYTES 177192u
#define TITLE_MUSIC_RATE 12000u
static void stopTitleMusic(void);
static void beginDream(unsigned kind,bool returnsToIntro);
static unsigned activeNight = 1;
static EndUITouch endTouch;
static GameOverState gameOver;
static VictoryInteraction victoryInput;
static bool debugSession;
static unsigned customSelected = 0;
static unsigned customPresetIndex=0,customControl=0,customFeedback=0;
static bool customMenuFocus=false,customOptions=false;
static int customDelta=0;
#ifdef DEV_BUILD
static unsigned devNight=1,devControl=0;
static bool devChordReleased=false;
#endif
static Cutscene dream;static bool dreamReturnsToIntro;static unsigned dreamSeenMask;
static unsigned page = 0; /* 0 title, 1 custom setup, 2 night entry */
static ContractState contractState;
static bool pendingNewGame;
static unsigned warningBlinkPhase;
static bool contractStrokeActive;
static int contractLastX,contractLastY;
static u8 contractInkPixels[(SIGN_X1-SIGN_X0+1)*(SIGN_Y1-SIGN_Y0+1)];
static void presentBottom(void);
static bool loadNitroRaw(const char *name,void *target,unsigned bytes,unsigned offset);
static bool contractInsideSignature(int x,int y);
static void contractInkPoint(int x,int y);
static void contractInkLine(int x0,int y0,int x1,int y1);
static const char *notice = NULL;
static unsigned noticeTicks=0;
static u64 victoryStartMilliseconds;static bool endWon;static unsigned endRare,endStage,endBlink,introStart,introStage;
static void startTitleMusic(void);
static void copyTopFrame(int frame);
static unsigned int frameCounter = 0;
static unsigned int glitchTimer = GLITCH_MIN_TICKS;
static int glitchFrame = 0;

static bool musicStarted = false;
#ifndef DISABLE_STARTUP_LOG
static bool startupLogReady = false;
static unsigned int logSequence = 0;

static void logUnsigned(FILE *log, unsigned int value, unsigned int width)
{
    char digits[10];
    unsigned int count = 0;
    do {
        digits[count++] = (char)('0' + value % 10);
        value /= 10;
    } while (value && count < sizeof(digits));
    while (count < width && count < sizeof(digits))
        digits[count++] = '0';
    while (count)
        fputc(digits[--count], log);
}

/* Write each checkpoint separately so the last completed step survives a crash. */
static void logCheckpoint(const char *label)
{
    if (!startupLogReady)
        return;

    FILE *log = fopen("/FNAF2-STARTUP.LOG", "a");
    if (!log)
        return;

    logUnsigned(log, ++logSequence, 4);
    fputs(" frame=", log);
    logUnsigned(log, frameCounter, 1);
    fputc(' ', log);
    fputs(label, log);
    fputs("\r\n", log);
    fflush(log);
    fclose(log);
}

static void initializeStartupLog(void)
{
    if (!fatInitDefault())
        return;

    FILE *log = fopen("/FNAF2-STARTUP.LOG", "w");
    if (!log)
        return;

    startupLogReady = true;
    fputs("FNAF2 DS diagnostic build 0.19.6.2 V19.6.2\r\n", log);
    fputs("log_file=/FNAF2-STARTUP.LOG\r\n", log);
    fputs("build_date=", log);
    fputs(__DATE__, log);
    fputc(' ', log);
    fputs(__TIME__, log);
    fputs("\r\nfat_mount=ok\r\n", log);
    fflush(log);
    fclose(log);
}
#else
static void logCheckpoint(const char *label)
{
    (void)label;
}

static void initializeStartupLog(void)
{
}
#endif

/* Compact 5x7 font used for live text over the lower-screen bitmap. */
static const u8 glyphs[42][7] = {
    {0,0,0,0,0,0,0},                         /* space */
    {14,17,17,31,17,17,17},                  /* A */
    {30,17,17,30,17,17,30},                  /* B */
    {14,17,16,16,16,17,14},                  /* C */
    {30,17,17,17,17,17,30},                  /* D */
    {31,16,16,30,16,16,31},                  /* E */
    {31,16,16,30,16,16,16},                  /* F */
    {14,17,16,23,17,17,15},                  /* G */
    {17,17,17,31,17,17,17},                  /* H */
    {14,4,4,4,4,4,14},                       /* I */
    {7,2,2,2,18,18,12},                       /* J */
    {17,18,20,24,20,18,17},                  /* K */
    {16,16,16,16,16,16,31},                  /* L */
    {17,27,21,21,17,17,17},                  /* M */
    {17,25,21,19,17,17,17},                  /* N */
    {14,17,17,17,17,17,14},                  /* O */
    {30,17,17,30,16,16,16},                  /* P */
    {14,17,17,17,21,18,13},                  /* Q */
    {30,17,17,30,20,18,17},                  /* R */
    {15,16,16,14,1,1,30},                    /* S */
    {31,4,4,4,4,4,4},                        /* T */
    {17,17,17,17,17,17,14},                  /* U */
    {17,17,17,17,17,10,4},                   /* V */
    {17,17,17,21,21,21,10},                  /* W */
    {17,17,10,4,10,17,17},                   /* X */
    {17,17,10,4,4,4,4},                     /* Y */
    {31,1,2,4,8,16,31},                      /* Z */
    {14,17,19,21,25,17,14},                  /* 0 */
    {4,12,4,4,4,4,14},                       /* 1 */
    {14,17,1,2,4,8,31},                      /* 2 */
    {30,1,1,14,1,1,30},                      /* 3 */
    {2,6,10,18,31,2,2},                     /* 4 */
    {31,16,16,30,1,1,30},                    /* 5 */
    {14,16,16,30,17,17,14},                 /* 6 */
    {31,1,2,4,8,8,8},                        /* 7 */
    {14,17,17,14,17,17,14},                 /* 8 */
    {14,17,17,15,1,1,14},                   /* 9 */
    {0,4,4,0,4,4,0},                         /* : */
    {10,21,21,10,0,0,0},                    /* @ */
    {0,0,0,0,0,12,12},                      /* . */
    {4,4,2,0,0,0,0},                        /* ' */
    {0,0,0,0,0,0,31}                        /* _ */
};

static const u8 *glyphFor(char c)
{
    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (c == ' ') return glyphs[0];
    if (c >= 'A' && c <= 'Z') return glyphs[1 + c - 'A'];
    static const u8 plus[7]={0,4,4,31,4,4,0},minus[7]={0,0,0,31,0,0,0},lt[7]={2,4,8,16,8,4,2},gt[7]={8,4,2,1,2,4,8};
    if(c=='+')return plus;
    if(c=='-')return minus;
    if(c=='<')return lt;
    if(c=='>')return gt;
    if (c >= '0' && c <= '9') return glyphs[27 + c - '0'];
    if (c == ':') return glyphs[37];
    if (c == '@') return glyphs[38];
    if (c == '.') return glyphs[39];
    if (c == '\'') return glyphs[40];
    if (c == '_') return glyphs[41];
    return glyphs[0];
}

static void drawStringTo(u16 *buffer, int x, int y, const char *text, u16 color)
{
    while (*text) {
        const u8 *glyph = glyphFor(*text++);
        for (int gy = 0; gy < 7; ++gy)
            for (int gx = 0; gx < 5; ++gx)
                if (glyph[gy] & (1 << (4 - gx)))
                    buffer[(y + gy) * 256 + x + gx] = color;
        x += 6;
    }
}

static void drawString(int x,int y,const char *text,u16 color)
{ drawStringTo(bottomBuffer,x,y,text,color); }

static void contractRect(u16 *target,int x,int y,int w,int h,u16 color)
{
    for(int py=y;py<y+h;py++)for(int px=x;px<x+w;px++)target[py*256+px]=color;
}

static void contractOutline(u16 *target,int x,int y,int w,int h,u16 color)
{
    for(int px=x;px<x+w;px++){target[y*256+px]=color;target[(y+h-1)*256+px]=color;}
    for(int py=y;py<y+h;py++){target[py*256+x]=color;target[py*256+x+w-1]=color;}
}

static void drawContractButton(u16 *target,int x,int w,const char *label,bool enabled)
{
    u16 edge=RGB15(1,1,1)|BIT(15), fill=enabled?(RGB15(28,27,21)|BIT(15)):(RGB15(11,11,10)|BIT(15));
    contractRect(target,x,146,w,19,fill);contractOutline(target,x,146,w,19,edge);
    int tx=x+(w-(int)strlen(label)*6)/2;
    drawStringTo(target,tx,152,label,enabled?(RGB15(1,1,1)|BIT(15)):(RGB15(15,15,13)|BIT(15)));
}

/* The DSi digitizer is on the lower display, so mirror the form there. */
static void drawContractOne(u16 *target)
{
    if(!loadNitroRaw("contract_form.bin",target,FRAME_BYTES,0))return;
    for(int y=SIGN_Y0;y<=SIGN_Y1;y++)for(int x=SIGN_X0;x<=SIGN_X1;x++)
        if(contractInkPixels[(y-SIGN_Y0)*(SIGN_X1-SIGN_X0+1)+(x-SIGN_X0)])target[y*256+x]=RGB15(2,2,2)|BIT(15);
    if(contractState.signedForm)drawContractButton(target,203,48,"APPLY",true);
}

static void drawContract(void)
{
    loadNitroRaw("newspaper.bin",bgGetGfxPtr(topBitmap),FRAME_BYTES,0);
    drawContractOne(bottomBuffer);
    contractRect(bottomBuffer,0,168,256,24,0x8000);
    drawString(4,170,"DESENHE OU PRESSIONE L",0xffff);
    drawString(4,181,"L: ASSINAR E CONTINUAR",0xffff);
    presentBottom();
}

static bool updateContract(u32 down,u32 held)
{
    if(down&KEY_L){
        contractStrokeActive=false;
        contractInkLine(65,105,65,119);contractInkLine(65,119,76,114);
        contractInkLine(76,114,85,119);contractInkLine(85,119,96,107);
        contractInkLine(96,107,110,118);contractInkLine(110,118,126,111);
        return contractStateApply(&contractState);
    }
    if(down&KEY_TOUCH) {
        touchPosition touch;touchRead(&touch);
        int x=touch.px,y=touch.py;
        if(x>=5&&x<=53&&y>=146&&y<=164) {
            memset(contractInkPixels,0,sizeof(contractInkPixels));
            contractStateClear(&contractState);contractStrokeActive=false;
            drawContract();return false;
        }
        if(contractStateCanApply(&contractState)&&x>=203&&x<=255&&y>=146&&y<=164) {
            contractStrokeActive=false;return contractStateApply(&contractState);
        }
        contractStrokeActive=contractInsideSignature(x,y);
        if(contractStrokeActive){
            bool hadInk=contractState.signedForm;contractLastX=x;contractLastY=y;contractInkPoint(x,y);
            if(!hadInk&&contractState.signedForm)drawContractButton(bottomBuffer,203,48,"APPLY",true);
            presentBottom();
        }
    } else if((held&KEY_TOUCH)&&contractStrokeActive) {
        touchPosition touch;touchRead(&touch);
        int x=touch.px,y=touch.py;
        if(!contractInsideSignature(x,y))contractStrokeActive=false;
        else {
            bool hadInk=contractState.signedForm;contractInkLine(contractLastX,contractLastY,x,y);
            contractLastX=x;contractLastY=y;
            if(!hadInk&&contractState.signedForm)drawContractButton(bottomBuffer,203,48,"APPLY",true);
            presentBottom();
        }
    } else contractStrokeActive=false;
    return false;
}

static bool contractInsideSignature(int x,int y)
{ return contractPointAllowed(x,y); }

static void contractInkPoint(int x,int y)
{
    if(!contractInsideSignature(x,y))return;
    unsigned index=(unsigned)(y-SIGN_Y0)*(SIGN_X1-SIGN_X0+1)+(unsigned)(x-SIGN_X0);
    if(!contractInkPixels[index]){contractInkPixels[index]=1;contractStateAddInk(&contractState);}
    bottomBuffer[y*256+x]=RGB15(2,2,2)|BIT(15);
}

static void contractInkLine(int x0,int y0,int x1,int y1)
{
    int dx=abs(x1-x0),sx=x0<x1?1:-1,dy=-abs(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
    for(;;){contractInkPoint(x0,y0);if(x0==x1&&y0==y1)break;int e2=2*err;if(e2>=dy){err+=dy;x0+=sx;}if(e2<=dx){err+=dx;y0+=sy;}}
}

static void presentBottom(void)
{
    DC_FlushRange(bottomBuffer, FRAME_BYTES);
    dmaCopy(bottomBuffer, bgGetGfxPtr(bottomBitmap), FRAME_BYTES);
}
static void drawMenuText(void)
{
    unsigned n = menuFrame(&progress, selectedOption);
    if(!loadNitroRaw("title_menu_frames.bin",bottomBuffer,FRAME_BYTES,n*FRAME_BYTES))
        dmaFillHalfWords(0x8000,bottomBuffer,FRAME_BYTES);
    if(noticeTicks && notice) {
        for(int y=165;y<188;y++)for(int x=4;x<252;x++)bottomBuffer[y*256+x]=RGB15(1,2,3)|BIT(15);
        drawString(10,173,notice,RGB15(31,26,12)|BIT(15));
    }
    presentBottom();
    logCheckpoint("menu_render_complete");
}
static const char *aiNames[AI_COUNT]={"T.FREDDY","T.BONNIE","T.CHICA","MANGLE","BB","FREDDY","BONNIE","CHICA","FOXY","GOLDEN"};
static const char *aiShort[AI_COUNT]={"T.FRED","T.BON","T.CHICA","MANGLE","BB","FREDDY","BONNIE","CHICA","FOXY","GOLDEN"};
static const char *aiHelp[AI_COUNT]={"OBSERVE O CORREDOR. USE A MASCARA.","ENTRA PELA DIREITA. USE A MASCARA.","VIGIE A ESQUERDA. USE A MASCARA.","OUCA O RADIO E VIGIE A VENTILACAO.","NAO DEIXE ENTRAR: BLOQUEIA A LUZ.","CORREDOR: COLOQUE A MASCARA.","USE A MASCARA AO BAIXAR O MONITOR.","USE A MASCARA QUANDO INVADIR.","USE A LANTERNA NO CORREDOR.","REAGE RAPIDO. USE A MASCARA."};
static void customButton(int x,int y,int w,const char *text,u16 color,bool focus){
 contractRect(bottomBuffer,x,y,w,30,color);
 contractOutline(bottomBuffer,x,y,w,30,focus?(RGB15(31,16,0)|BIT(15)):0xffff);
 drawString(x+(w-(int)strlen(text)*6)/2,y+11,text,0xffff);
}
static void drawCustom(void){
 u16 *top=bgGetGfxPtr(topBitmap);
 for(unsigned y=0;y<192;y++)for(unsigned x=0;x<256;x++)
  top[y*256+x]=bottomBuffer[y*256+x]=0x8000|(((x*13+y*7+frameCounter/10)%97==0)?RGB15(2,3,3):0);
 drawStringTo(top,20,3,"FIVE NIGHTS AT FREDDY'S 2",0xffff);
 drawStringTo(top,92,13,"CUSTOM NIGHT",0xffff);
 drawStringTo(top,62,180,"FAZBEAR ENTERTAINMENT",RGB15(5,5,5)|BIT(15));
 for(unsigned i=0;i<10;i++){
  int x=5+(i%5)*50,y=27+(i/5)*62;
  for(int py=0;py<40;py++)for(int px=0;px<40;px++)top[(y+py)*256+x+px]=customPortraits[i*1600+py*40+px];
  drawStringTo(top,x,y+42,aiShort[i],0xffff);
  if(i==customSelected){
   u16 c=customMenuFocus?(RGB15(10,10,10)|BIT(15)):(RGB15(31,16,0)|BIT(15));
   for(int k=0;k<8;k++){
    top[(y-2)*256+x-2+k]=c;top[(y-2+k)*256+x-2]=c;
    top[(y-2)*256+x+41-k]=c;top[(y-2+k)*256+x+41]=c;
    top[(y+40)*256+x-2+k]=c;top[(y+40-k)*256+x-2]=c;
    top[(y+40)*256+x+41-k]=c;top[(y+40-k)*256+x+41]=c;
   }
  }
 }
 drawStringTo(top,5,151,aiNames[customSelected],RGB15(31,16,0)|BIT(15));
 drawStringTo(top,5,164,aiHelp[customSelected],0xffff);
 if(customOptions){
  drawString(20,16,"OPTIONS / CONTROLES",0xffff);
  drawString(8,40,"D-PAD: SELECIONAR PERSONAGEM",0xffff);
  drawString(8,54,"A: AUMENTAR   B: DIMINUIR",0xffff);
  drawString(8,68,"X: TROCAR FOCO ENTRE TELAS",0xffff);
  drawString(8,82,"START: INICIAR A NOITE",0xffff);
  drawString(8,100,progress.cheats?"CHEATS ON: SEM MORTE POR IA":"CHEATS OFF: DIFICULDADE NORMAL",0xffff);
  drawString(8,114,"CHEATS: SEM PREMIOS OU PROGRESSO",0xffff);
  drawString(8,128,"Y: GAME MENU",0xffff);
  customButton(8,140,116,"VOLTAR",RGB15(1,6,12)|BIT(15),true);
  customButton(132,140,116,"GAME MENU",RGB15(1,6,12)|BIT(15),false);
 }else{
  customButton(8,4,240,customPresetNames[customPresetIndex],RGB15(2,3,5)|BIT(15),customMenuFocus&&customControl==0);
  drawString(12,16,"<",0xffff);drawString(238,16,">",0xffff);
  char number[4];snprintf(number,sizeof(number),"%02u",progress.ai[customSelected]);
  customButton(8,46,50,"-",RGB15(2,4,5)|BIT(15),customMenuFocus&&customControl==1);
  customButton(198,46,50,"+",RGB15(2,4,5)|BIT(15),customMenuFocus&&customControl==1);
  for(unsigned c=0;c<2;c++){const u8 *g=glyphFor(number[c]);for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<(4-gx)))contractRect(bottomBuffer,107+c*22+gx*3,48+gy*3,3,3,0xffff);}
  drawString(14,83,"0-2 EASY   3-6 MED",0xffff);
  drawString(14,95,"7-12 HARD  13-20 EXTREME",0xffff);
  customButton(8,114,116,"READY",RGB15(1,14,4)|BIT(15),customMenuFocus&&customControl==2);
  customButton(132,114,116,"OPTIONS",RGB15(1,7,20)|BIT(15),customMenuFocus&&customControl==3);
  drawString(8,155,"D-PAD SELECT  A +1  B -1",0xffff);
  drawString(8,168,"X FOCUS   START READY",0xffff);
  if(customFeedback)drawString(174,48,customDelta>0?"+1":"-1",RGB15(31,16,0)|BIT(15));
 }
 presentBottom();
}
static void drawPage(void){if(page==1)drawCustom();}
static ProgressStore progressStore;
static bool saveProgress(void) {
 return storageReady && progressStoreSave(&progressStore,&progress);
}
static void loadProgress(const char *romPath) {
 storageReady=fatInitDefault();
 if(!storageReady){logCheckpoint("save_storage_unavailable");return;}
 char cwd[SAVE_PATH_SIZE];if(!getcwd(cwd,sizeof(cwd)))strcpy(cwd,"/");
 storageReady=progressStoreInit(&progressStore,romPath,cwd);
 if(!storageReady){logCheckpoint("save_directory_unavailable");return;}
 char message[SAVE_PATH_SIZE+32];snprintf(message,sizeof(message),"save_directory=%s",progressStore.directory);logCheckpoint(message);
 if(!romPath)logCheckpoint("rom_path_missing_using_working_directory");
 bool migrated=false;
 if(progressStoreLoad(&progressStore,&progress,&migrated) && migrated)
  logCheckpoint(saveProgress()?"legacy_save_copied_to_saves":"legacy_save_loaded_copy_failed");
}
static void drawOfficeControls(void){
 const Office *o=officeStatus();bool redraw=officeRenderPanel(bottomBuffer);u16 white=RGB15(30,30,30)|BIT(15);char text[40];
 if(o->dead){if(redraw)presentBottom();return;}
 bool tablet=o->monitorFrame || o->monitorTarget;if(tablet && !officeCameraMapVisible()){if(redraw)presentBottom();return;}
 u16 *target=tablet?bottomBuffer:bgGetGfxPtr(topBitmap);
 snprintf(text,sizeof(text),"%u AM",o->hour?o->hour:12);drawStringTo(target,202,9,text,white);
 snprintf(text,sizeof(text),"NIGHT %u",o->night);drawStringTo(target,196,20,text,white);
 if(o->debugUsed)drawStringTo(target,196,31,"DEBUG",RGB15(31,14,0)|BIT(15));
 if(tablet)drawStringTo(target,239,145,"R",white);
 if(redraw || tablet)presentBottom();
}
static void enterOffice(void)
{
    stopTitleMusic();
    loadingBegin(bgGetGfxPtr(topBitmap),bgGetGfxPtr(bottomBitmap));
    bool officeLoaded=officeOpen(activeNight);loadingEnd();
    if(officeLoaded){officeSetRewards(progress.rewards);officeConfigureAI(progress.ai,progress.cheats);if(debugSession)officeMarkDebugUsed();
#ifdef DEV_BUILD
officeMarkDebugUsed();
#endif
page=2;officeRender(bgGetGfxPtr(topBitmap));drawOfficeControls();logCheckpoint("office_ready");char audioInfo[64];snprintf(audioInfo,sizeof(audioInfo),"office_audio_loaded_mask=%03X core=3FF",officeLoadedAudioMask());logCheckpoint(audioInfo);snprintf(audioInfo,sizeof(audioInfo),"office_call_night=%u started=%u",activeNight,officeStatus()->callActive);logCheckpoint(audioInfo);}
    else {
        officeClose();page=0;notice="OFFICE ASSETS NOT FOUND";noticeTicks=300;
        drawMenuText();startTitleMusic();logCheckpoint("office_assets_failed");
    }
}
static bool dreamSeenFile(bool writeFlag){
 char path[640];snprintf(path,sizeof(path),
#ifdef DEV_BUILD
 "%s/FNAF2-DEV-DREAM-SEEN.dat",
#else
 "%s/FNAF2-DREAM-SEEN.dat",
#endif
 progressStore.directory);
 FILE *f=fopen(path,writeFlag?"wb":"rb");if(!f)return false;
 unsigned char data[10]={ 'F','2','D','R','E','A','M','1',(unsigned char)dreamSeenMask,(unsigned char)(dreamSeenMask^0xA5)};
 bool ok=writeFlag?fwrite(data,1,10,f)==10:fread(data,1,10,f)==10 && !memcmp(data,"F2DREAM1",8) && (data[8]^0xA5)==data[9] && data[8]<16;
 if(!writeFlag && ok)dreamSeenMask=data[8];
 if(fclose(f)!=0)ok=false;
 return ok;
}

static void drawWarningStart(unsigned phase)
{
    if(loadNitroRaw("warning_start.bin",bottomBuffer,FRAME_BYTES,(phase&1u)*FRAME_BYTES))presentBottom();
}

static bool loadNitroRaw(const char *name,void *target,unsigned bytes,unsigned offset)
{
    char path[64];snprintf(path,sizeof(path),"nitro:/%s",name);
    FILE *file=fopen(path,"rb");if(!file)return false;
    bool ok=fseek(file,(long)offset,SEEK_SET)==0&&fread(target,1,bytes,file)==bytes;
    fclose(file);return ok;
}

static void launchPendingNight(void)
{
    if(pendingNewGame) {
        progressNewGame(&progress);activeNight=1;
        if(!saveProgress())logCheckpoint("save_write_failed");
        pendingNewGame=false;
    }
    stopTitleMusic();
    if(activeNight==1){beginDream(1,true);logCheckpoint("apply_direct_night1_dream");return;}
    page=4;introStart=frameCounter;introStage=1;
    if(!introStage)officeShowAsset("transition_1",bgGetGfxPtr(topBitmap));
    else {char name[32];snprintf(name,sizeof(name),"night_intro_%u",activeNight);officeShowAsset(name,bgGetGfxPtr(topBitmap));}
    for(unsigned i=0;i<FRAME_PIXELS;i++)bottomBuffer[i]=0x8000;
    presentBottom();
}

static void terminalLine(unsigned row,const char *text,u16 color)
{ drawString(8,32+(int)row*14,text,color); }

static void drawCutsceneTerminal(unsigned kind,unsigned frames,bool goldenVisible)
{
    unsigned mode=cutsceneLogMode(kind,frames,goldenVisible);
    dmaFillHalfWords(0x8000,bottomBuffer,FRAME_BYTES);
    if(mode==CUTSCENE_LOG_BLACK){presentBottom();return;}

    const u16 green=RGB15(3,30,7)|BIT(15);
    const u16 dimGreen=RGB15(1,15,3)|BIT(15);
    if(mode==CUTSCENE_LOG_ITS_ME){
        u32 noise=0x6D2B79F5u+frames*747796405u;
        for(unsigned i=0;i<1000;i++){
            noise=noise*1664525u+1013904223u;
            bottomBuffer[((noise>>8)%192)*256+(noise&255)]=dimGreen;
        }
    }

    drawString(8,8,"FREDDY SYSTEM TERMINAL",green);
    drawString(8,20,"----------------------",dimGreen);
    if(mode==CUTSCENE_LOG_ERRORS){
        static const char *errors[]={"ERR: SENSOR READ FAILED","ERR: IMAGE FEED LOST","ERR: FACE ID MISMATCH","ERR: MEMORY BUFFER CORRUPT","ERR: UNKNOWN INPUT SIGNAL","ERR: MOTOR STATUS INVALID"};
        for(unsigned row=0;row<11;row++)terminalLine(row,errors[row%6],green);
    }else if(mode==CUTSCENE_LOG_RECOGNITION){
        terminalLine(0,"ALERT: BONNIE FACE ID FAILED",green);
        terminalLine(1,"ALERT: CHICA FACE ID FAILED",green);
        terminalLine(2,"VISUAL MATCH: 0 PERCENT",green);
        terminalLine(3,"MASK RECOGNITION: ERROR",green);
        terminalLine(4,"SENSOR STATE: UNSTABLE",green);
        terminalLine(5,"RETRYING IDENTIFICATION",dimGreen);
        terminalLine(6,"RETRYING IDENTIFICATION",dimGreen);
    }else if(mode==CUTSCENE_LOG_ITS_ME){
        for(unsigned row=0;row<10;row++){
            terminalLine(row,(row&1)?"IT'S ME  IT'S ME  IT'S ME":"IT'S ME  IT'S ME",green);
        }
    }else if(mode==CUTSCENE_LOG_UNKNOWN){
        terminalLine(0,"UNKNOWN_ENTITY_DETECTED",green);
        terminalLine(1,"ENTITY: GOLDEN FREDDY",green);
        terminalLine(2,"VISUAL ID: UNRESOLVED",green);
        terminalLine(3,"MOTION: UNCONFIRMED",dimGreen);
        terminalLine(4,"WARNING: DO NOT APPROACH",green);
    }else if(mode==CUTSCENE_LOG_OVERRIDE){
        terminalLine(0,"SYSTEM OVERRIDE",green);
        terminalLine(1,"OVERRIDE SOURCE: PUPPET",green);
        terminalLine(2,"FREDDY CONTROL: LOST",green);
        terminalLine(3,"SHUTTING DOWN...",dimGreen);
    }else{
        terminalLine(0,"BOOT SEQUENCE START",green);
        terminalLine(1,"SENSORS: OK",green);
        terminalLine(2,"MOTOR CONTROL: OK",green);
        terminalLine(3,"CAMERA LINK: OK",green);
        terminalLine(4,"AUDIO FEED: ACTIVE",green);
        terminalLine(5,"MEMORY CHECK: PASSED",green);
        if(kind>=5){
            terminalLine(7,"NIGHT DIAGNOSTICS: READY",dimGreen);
            terminalLine(8,"AWAITING VISUAL INPUT",dimGreen);
        }
    }
    presentBottom();
}

static void beginDream(unsigned kind,bool returnsToIntro){
 stopTitleMusic();
 cutsceneReset(&dream,kind);dreamReturnsToIntro=returnsToIntro;page=5;loadingBegin(bgGetGfxPtr(topBitmap),bgGetGfxPtr(bottomBitmap));officeCutsceneBegin(kind);loadingEnd();
 drawCutsceneTerminal(kind,dream.frames,cutsceneGoldenVisible(&dream));
 officeCutsceneRender(cutsceneRoomFrame(&dream),72,cutsceneGoldenVisible(&dream),kind==4,bgGetGfxPtr(topBitmap));
 officeCutsceneAudio(kind,0,false);
}
/* Both outcomes keep the same large hitboxes. Victory is gated by its fade;
   Game Over can be retried as soon as the old touch has been released. */
static void endToMenu(void)
{
    officeClose();victoryClose();gameOverClose();page=0;selectedOption=0;
    copyTopFrame(0);drawMenuText();startTitleMusic();
}
static void showEndFallback(bool won)
{
    dmaFillHalfWords(0x8000,bottomBuffer,FRAME_BYTES);
    drawString(16,28,won?"NIGHT SURVIVED":"GAME OVER",RGB15(31,4,4)|BIT(15));
    for(unsigned i=0;i<2;i++){
        unsigned x=i?132:8;u16 color=(i?RGB15(22,23,24):RGB15(7,27,29))|BIT(15);
        for(unsigned y=150;y<186;y++)for(unsigned px=x;px<x+116;px++)
            if(y==150 || y==185 || px==x || px==x+115)bottomBuffer[y*256+px]=color;
    }
    drawString(16,163,won?"TAP CONTINUE":"RETRY NIGHT",RGB15(7,27,29)|BIT(15));
    drawString(154,163,"GAME MENU",RGB15(29,29,29)|BIT(15));
}
static void finishNight(void)
{
    Office result=*officeStatus();
    endUIReset(&endTouch);
    victoryInteractionReset(&victoryInput);
    officeClose();
    bool earned=!result.previewUsed && !result.debugUsed && !result.dead && progressVictory(&progress,activeNight,progress.ai,progress.cheats);
    bool saved=earned && saveProgress();
    endWon=!result.dead;endRare=!endWon && rand()%100<2?6+(unsigned)rand()%3:0;endStage=0;endBlink=99;
    if(endWon){
        bool loaded=victoryOpen(activeNight);
        logCheckpoint(loaded?"victory_both_screens_preloaded":"victory_assets_load_failed");
        page=6;
        victoryStartMilliseconds=officeElapsedMilliseconds();
        officeEndAudioBegin(true);
        victoryRender(0,bgGetGfxPtr(topBitmap),bottomBuffer);presentBottom();
    }else{
        page=3;
        if(endRare){char name[32];snprintf(name,sizeof(name),"transition_%u",endRare);officeShowAsset(name,bgGetGfxPtr(topBitmap));}
        else{officeShowAsset("transition_2",bgGetGfxPtr(topBitmap));officeEndAudioBegin(false);}
        victoryStartMilliseconds=officeElapsedMilliseconds();
        bool loaded=gameOverOpen();
        victoryStartMilliseconds=officeElapsedMilliseconds();
        gameOverReset(&gameOver,0);
        gameOverRenderUI(gameOverFrame(&gameOver,0),END_UI_NONE,bottomBuffer);
        if(!loaded)showEndFallback(false);
        presentBottom();
        logCheckpoint(loaded?"gameover_crt_single_sheet_ready":"gameover_crt_sheet_load_failed");
    }
    logCheckpoint(saved?"night_victory_saved":"night_end_no_saved_award");
}

static void activateOption(void)
{
    unsigned cheatsOption=menuCount(&progress)-1;
    notice=NULL;noticeTicks=0;
    if(selectedOption==cheatsOption) {
        progress.cheats=!progress.cheats;
        if(!saveProgress())logCheckpoint("save_write_failed");
        drawMenuText();return;
    }
    if(selectedOption==0) {
        debugSession=false;pendingNewGame=true;activeNight=1;
    } else if(selectedOption==1) {
        if(!progressCanContinue(&progress)){notice="COMPLETE NIGHT 1 FIRST";noticeTicks=180;drawMenuText();return;}
        debugSession=false;pendingNewGame=false;
        activeNight=progress.night;
    } else if(selectedOption==2){debugSession=false;pendingNewGame=false;activeNight=6;}
    else if(selectedOption==3){page=1;customSelected=0;customMenuFocus=false;customOptions=false;customPresetIndex=0;drawPage();return;}
    if(!pendingNewGame){launchPendingNight();return;}
    memset(contractInkPixels,0,sizeof(contractInkPixels));contractStateClear(&contractState);contractStrokeActive=false;
    page=7;drawContract();logCheckpoint("contract_signature_required");
}

static void copyTopFrame(int frame)
{
    logCheckpoint("top_frame_copy_start");
    char name[32];snprintf(name,sizeof(name),"title_top_%u.bin",(unsigned)frame);
    if(!loadNitroRaw(name,bgGetGfxPtr(topBitmap),FRAME_BYTES,0))
        dmaFillHalfWords(0x8000,bgGetGfxPtr(topBitmap),FRAME_BYTES);
    logCheckpoint("top_frame_copy_complete");
}

static void stopTitleMusic(void)
{
    if(musicChannel>=0)soundKill(musicChannel);
    musicChannel=-1;musicStarted=false;
    free(titleMusicBuffer);titleMusicBuffer=NULL;
}

static void startTitleMusic(void)
{
    if (musicStarted)
        return;
    logCheckpoint("audio_start_begin");
    soundEnable();
    soundSetMixerVolume(127);
    soundSetMixerConfig(SoundOutSrc_Mixer, SoundOutSrc_Mixer, false, false);
    logCheckpoint("audio_engine_enabled");
    titleMusicBuffer=malloc(TITLE_MUSIC_BYTES);
    if(!titleMusicBuffer || !loadNitroRaw("title_music.raw",titleMusicBuffer,TITLE_MUSIC_BYTES,0)){stopTitleMusic();logCheckpoint("title_music_missing");return;}
    DC_FlushRange(titleMusicBuffer, TITLE_MUSIC_BYTES);
    musicChannel = soundPlaySample(titleMusicBuffer, SoundFormat_8Bit, TITLE_MUSIC_BYTES,
                    TITLE_MUSIC_RATE, 127, 64, true, 0);
    musicStarted = musicChannel >= 0;
    char audioStatus[64];
    snprintf(audioStatus,sizeof(audioStatus),"audio_channel=%d sample_bytes=%u",musicChannel,(unsigned)TITLE_MUSIC_BYTES);
    logCheckpoint(audioStatus);
    logCheckpoint(musicStarted ? "audio_request_accepted" : "audio_request_failed");
}

static void handleMenuTouch(void)
{
    touchPosition touch;touchRead(&touch);
    unsigned count=menuCount(&progress);
    int first=progress.night>=6?48:(progress.night==5?67:60);
    int height=progress.night>=6?21:(progress.night==5?20:27);
    if(touch.px<76 || touch.px>232 || touch.py<first)return;
    unsigned row=(touch.py-first)/height;
    if(row<count){selectedOption=row;activateOption();}
}

#ifdef DEV_BUILD
static void drawDevMenu(void){
 dmaFillHalfWords(0x8000,bottomBuffer,FRAME_BYTES);
 drawString(14,10,"DEV MENU - SEM RECOMPENSAS",0xffff);
 char text[32];snprintf(text,sizeof(text),"NOITE %u",devNight);
 customButton(8,30,240,text,RGB15(3,4,5)|BIT(15),devControl==0);
 customButton(8,66,240,"CARREGAR NOITE",RGB15(1,10,6)|BIT(15),devControl==1);
 customButton(8,102,240,"VER COMEMORACAO",RGB15(1,7,16)|BIT(15),devControl==2);
 customButton(8,138,240,"VOLTAR AO JOGO",RGB15(5,5,5)|BIT(15),devControl==3);
 drawString(8,179,"A OK  B VOLTA  X CUSTOM NIGHT",0xffff);
 presentBottom();
}
static void updateDevMenu(u32 down,u32 held){
 if(!devChordReleased){if(!(held&(KEY_L|KEY_R)))devChordReleased=true;return;}
 if(down&KEY_X){officeDebugPause(false);officeClose();page=1;customSelected=0;customMenuFocus=false;customOptions=false;customPresetIndex=0;drawCustom();return;}
 bool confirm=(down&KEY_A)!=0;
 if(down&KEY_UP)devControl=(devControl+3)%4;
 if(down&KEY_DOWN)devControl=(devControl+1)%4;
 if(down&KEY_LEFT)devNight=devNight==1?7:devNight-1;
 if(down&KEY_RIGHT)devNight=devNight==7?1:devNight+1;
 if(down&KEY_TOUCH){touchPosition t;touchRead(&t);
  if(t.py>=30&&t.py<60){devNight=officeDebugNextNight(devNight,t.px<128?-1:1);}
  else if(t.py>=66&&t.py<96){devControl=1;confirm=true;}
  else if(t.py>=102&&t.py<132){devControl=2;confirm=true;}
  else if(t.py>=138&&t.py<168){devControl=3;confirm=true;}
 }
 if((down&KEY_B)||(confirm&&devControl==3)){officeDebugPause(false);page=2;officeRender(bgGetGfxPtr(topBitmap));drawOfficeControls();return;}
 if(confirm&&(devControl==1||devControl==2)){
  officeDebugPause(false);debugSession=true;
  if(devControl==1||activeNight!=devNight){activeNight=devNight;officeClose();enterOffice();}
  if(devControl==2&&page==2){officeDebugVictory();finishNight();victoryStartMilliseconds-=9000;victoryRender(9000,bgGetGfxPtr(topBitmap),bottomBuffer);presentBottom();}
  return;
 }
 drawDevMenu();
}
#endif
int main(int argc,char **argv)
{
    progressReset(&progress);
    logCheckpoint("main_entered");

    /* One bitmap BG per display; this avoids overlapping text and bitmap VRAM. */
    logCheckpoint("video_modes_begin");
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);
    logCheckpoint("video_modes_ready");

    logCheckpoint("vram_banks_begin");
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    logCheckpoint("vram_banks_ready");

    logCheckpoint("background_init_begin");
    (void)nitroFSInit(NULL);
    topBitmap = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    logCheckpoint("top_background_ready");
    bottomBitmap = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    logCheckpoint("bottom_background_ready");

    /* First boot screen: legal warning above and the supplied START panel below. */
    dmaFillHalfWords(0x8000, bgGetGfxPtr(topBitmap), FRAME_BYTES);
    dmaFillHalfWords(0x8000, bgGetGfxPtr(bottomBitmap), FRAME_BYTES);
    initializeStartupLog();
    loadNitroRaw("title_warning.bin",bgGetGfxPtr(topBitmap),FRAME_BYTES,0);
    page=8;warningBlinkPhase=0;drawWarningStart(warningBlinkPhase);
    logCheckpoint("warning_start_visible");
    logCheckpoint("title_ready_main_loop_begin");

    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        const u32 down = keysDown();
        const u32 held = keysHeld();

        if(page==8) {
            bool startTouched=false;
            if(down&KEY_TOUCH){touchPosition touch;touchRead(&touch);startTouched=touch.px>=193&&touch.px<=255&&touch.py>=162&&touch.py<=191;}
            if(startTouched||(down&KEY_START)) {
                logCheckpoint("warning_start_selected");
                loadProgress(argc>0?argv[0]:NULL);dreamSeenFile(false);
                copyTopFrame(0);page=0;selectedOption=0;drawMenuText();startTitleMusic();
            }
#ifdef DEV_BUILD
        } else if(page==9) {
            updateDevMenu(down,held);
#endif
        } else if(page==7) {
            if(updateContract(down,held)) {
                logCheckpoint("contract_signed_and_applied");
                launchPendingNight();
            }
        } else if(page==0) {
            unsigned count=menuCount(&progress);
            if(down&KEY_UP){selectedOption=(selectedOption+count-1)%count;drawMenuText();}
            if(down&KEY_DOWN){selectedOption=(selectedOption+1)%count;drawMenuText();}
            if(down&(KEY_A|KEY_START))activateOption();
            if(down&KEY_TOUCH)handleMenuTouch();
        } else if(page==2) {
#ifdef DEV_BUILD
            if((held&(KEY_L|KEY_R))==(KEY_L|KEY_R) && (down&(KEY_L|KEY_R)) && !officeStatus()->dead && !officeStatus()->complete){
                devNight=activeNight;devControl=0;devChordReleased=false;
                officeDebugPause(true);page=9;drawDevMenu();
            } else
#endif
            if(down&KEY_START) {
                officeClose();page=0;selectedOption=0;copyTopFrame(0);drawMenuText();startTitleMusic();
            } else {
                unsigned previousHour=officeStatus()->hour;
                bool previousCall=officeStatus()->callActive;
                bool previousCamera=officeCameraMode(),previousDead=officeStatus()->dead;unsigned previousSelection=officeCameraSelected();
                officeUpdate(down,keysHeld());
                if(previousCamera!=officeCameraMode() || previousSelection!=officeCameraSelected()){char message[80];snprintf(message,sizeof(message),"camera_open=%u selected=%u wind_available=%u",officeCameraMode(),officeCameraSelected(),officeCameraMode() && officeCameraSelected()==11);logCheckpoint(message);}
                if(!previousDead && officeStatus()->dead)logCheckpoint("office_jumpscare_started");
                if(previousCall && !officeStatus()->callActive)logCheckpoint(officeStatus()->muted?"office_call_muted":"office_call_finished");
                if(officeStatus()->hour!=previousHour || officeStatus()->ticks%600==0) {
                    char clockLog[80];snprintf(clockLog,sizeof(clockLog),"office_hour=%u elapsed_ms=%lu",officeStatus()->hour,(unsigned long)officeElapsedMilliseconds());logCheckpoint(clockLog);
                }
                if(officeStatus()->complete || (officeStatus()->dead && officeDeathFinished()))finishNight();
                else {officeRender(bgGetGfxPtr(topBitmap));drawOfficeControls();}
            }
        } else if(page==4) {
            unsigned t=frameCounter-introStart;
            if(t>=(introStage?120u:300u) && activeNight<=4 && !(dreamSeenMask&(1u<<(activeNight-1)))){beginDream(activeNight,true);}
            else if(!introStage && t>=300){introStage=1;introStart=frameCounter;char name[32];snprintf(name,sizeof(name),"night_intro_%u",activeNight);officeShowAsset(name,bgGetGfxPtr(topBitmap));}
            else if(introStage && t>=120)enterOffice();
        } else if(page==5) {
            int previousPan=cutscenePan(&dream);
            cutsceneTick(&dream,(held&KEY_LEFT)!=0,(held&KEY_RIGHT)!=0);
            bool golden=cutsceneGoldenVisible(&dream);
            officeCutsceneRender(cutsceneRoomFrame(&dream),cutscenePan(&dream),golden,dream.kind==4,bgGetGfxPtr(topBitmap));
            drawCutsceneTerminal(dream.kind,dream.frames,golden);
            officeCutsceneAudio(dream.kind,dream.frames,(((held&KEY_LEFT)!=0)!=((held&KEY_RIGHT)!=0)) && (previousPan!=cutscenePan(&dream) || (cutscenePan(&dream)>0 && cutscenePan(&dream)<144)));
            if(cutsceneFinished(&dream)){
                officeClose();
                if(dreamReturnsToIntro){dreamSeenMask|=1u<<(dream.kind-1);if(!dreamSeenFile(true))logCheckpoint("dream_seen_write_failed");page=4;introStage=1;introStart=frameCounter;char name[32];snprintf(name,sizeof(name),"night_intro_%u",activeNight);officeShowAsset(name,bgGetGfxPtr(topBitmap));}
                else{
                    if(dream.kind==1){dreamSeenMask|=1u;if(!dreamSeenFile(true))logCheckpoint("dream_seen_write_failed");}
                    page=0;selectedOption=0;copyTopFrame(0);drawMenuText();startTitleMusic();
                }
            }
        } else if(page==3) {
            unsigned elapsed=(unsigned)(officeElapsedMilliseconds()-victoryStartMilliseconds);
            if(endRare && !endStage && elapsed>=1500){officeShowAsset("transition_2",bgGetGfxPtr(topBitmap));officeEndAudioBegin(false);endStage=1;}
            if(!endRare || endStage){officeEndAudioUpdate(false,elapsed);unsigned blink=(elapsed/400)%2;if(blink!=endBlink){endBlink=blink;officeShowAsset("transition_2",bgGetGfxPtr(topBitmap));if(blink)drawStringTo(bgGetGfxPtr(topBitmap),10,171,"GAME OVER",RGB15(31,31,31)|BIT(15));}}
            touchPosition touch={0};if(held&KEY_TOUCH)touchRead(&touch);
            GameOverInput input={.held=(held&KEY_TOUCH)!=0,.pressed=(down&KEY_TOUCH)!=0,
                .x=touch.px,.y=touch.py,.retryKey=(down&(KEY_A|KEY_START))!=0,.menuKey=(down&KEY_B)!=0};
            EndUIAction action=gameOverUpdate(&gameOver,elapsed,input);
            unsigned crtFrame=gameOverFrame(&gameOver,elapsed);
            if(gameOverRenderUI(crtFrame,(gameOver.phase==GO_PRESS || gameOver.phase==GO_CONFIRM)?gameOver.selected:END_UI_NONE,bottomBuffer)){
                if(!gameOverReady() && crtFrame!=GO_BLACK_FRAME)showEndFallback(false);
                presentBottom();
            }
            if(action==END_UI_MENU)endToMenu();
            else if(action==END_UI_PRIMARY){gameOverClose();officeClose();enterOffice();logCheckpoint("gameover_crt_retry_same_night_direct");}
        } else if(page==6) {
            unsigned elapsed=(unsigned)(officeElapsedMilliseconds()-victoryStartMilliseconds);
            officeEndAudioUpdate(true,elapsed);
            touchPosition touch={0};if(held&KEY_TOUCH)touchRead(&touch);
            GameOverInput input={.held=(held&KEY_TOUCH)!=0,.pressed=(down&KEY_TOUCH)!=0,.x=touch.px,.y=touch.py,
                .retryKey=(down&(KEY_A|KEY_START))!=0,.menuKey=(down&KEY_B)!=0};
            EndUIAction action=victoryInteractionUpdate(&victoryInput,elapsed,victoryCanContinue(elapsed),input);
            victorySetSelection(victoryInteractionSelected(&victoryInput));
            if(victoryRender(elapsed,bgGetGfxPtr(topBitmap),bottomBuffer))presentBottom();
            if(!victoryReady()){
                showEndFallback(true);presentBottom();
            }

            if(action==END_UI_MENU)endToMenu();
            else if(action==END_UI_PRIMARY){
                unsigned previousNight=activeNight;
                officeClose();victoryClose();
                activeNight=endUIContinueNight(activeNight);
                if(previousNight<=5){page=4;introStage=1;introStart=frameCounter;char name[32];snprintf(name,sizeof(name),"night_intro_%u",activeNight);officeShowAsset(name,bgGetGfxPtr(topBitmap));dmaFillHalfWords(0x8000,bottomBuffer,FRAME_BYTES);presentBottom();}
                else if(previousNight>=6){page=1;customSelected=0;customMenuFocus=false;customOptions=false;customPresetIndex=0;drawPage();startTitleMusic();}

            }
        } else if(page==1) {
            if(customFeedback)--customFeedback;
            if(down&KEY_X){customMenuFocus=!customMenuFocus;customOptions=false;}
            bool ready=(down&KEY_START)!=0,back=false;
            int adjust=0,preset=0;
            if(customOptions){
                if(down&(KEY_A|KEY_B))customOptions=false;
                if(down&KEY_Y)back=true;
                if(down&KEY_TOUCH){touchPosition t;touchRead(&t);if(t.py>=140&&t.py<170){if(t.px>=132)back=true;else customOptions=false;}}
            }else{
                if(!customMenuFocus){
                    if(down&KEY_LEFT)customSelected=customMove(customSelected,-1,0);
                    if(down&KEY_RIGHT)customSelected=customMove(customSelected,1,0);
                    if(down&KEY_UP)customSelected=customMove(customSelected,0,-1);
                    if(down&KEY_DOWN)customSelected=customMove(customSelected,0,1);
                    adjust=(down&KEY_A)?1:(down&KEY_B)?-1:0;
                }else{
                    if(down&KEY_UP)customControl=(customControl+3)%4;
                    if(down&KEY_DOWN)customControl=(customControl+1)%4;
                    if(customControl==0)preset=(down&(KEY_RIGHT|KEY_A))?1:(down&(KEY_LEFT|KEY_B))?-1:0;
                    if(customControl==1)adjust=(down&(KEY_RIGHT|KEY_A))?1:(down&(KEY_LEFT|KEY_B))?-1:0;
                    if(customControl==2 && (down&KEY_A))ready=true;
                    if(customControl==3 && (down&KEY_A))customOptions=true;
                }
                if(down&KEY_TOUCH){
                    touchPosition t;touchRead(&t);
                    if(t.py>=4&&t.py<34)preset=t.px<128?-1:1;
                    else if(t.py>=46&&t.py<76){if(t.px<58)adjust=-1;else if(t.px>=198)adjust=1;}
                    else if(t.py>=114&&t.py<144){if(t.px<128)ready=true;else customOptions=true;}
                }
            }
            if(adjust){customAdjust(progress.ai,customSelected,adjust);customPresetIndex=0;customDelta=adjust;customFeedback=20;}
            if(preset){customPresetIndex=(customPresetIndex+CUSTOM_PRESETS+preset)%CUSTOM_PRESETS;customPreset(progress.ai,customPresetIndex);}
            if(back){saveProgress();page=0;selectedOption=0;copyTopFrame(0);drawMenuText();}
            else if(ready){saveProgress();debugSession=false;activeNight=7;pendingNewGame=false;launchPendingNight();}
            else drawCustom();
        }
        if(page==0 && (down&KEY_X)) {
            logCheckpoint("audio_restart_requested");
            stopTitleMusic();startTitleMusic();
        }
        if(page!=2 && page!=3 && page!=6 && page!=1 && page!=9 && (down&KEY_SELECT))break;

        ++frameCounter;
        if(page==0 && noticeTicks && --noticeTicks==0){notice=NULL;drawMenuText();}
        if(page==8 && frameCounter%30==0){warningBlinkPhase^=1u;drawWarningStart(warningBlinkPhase);}
        if(page!=0)continue;
        if (glitchFrame > 0) {
            if ((frameCounter % 4) == 0) {
                ++glitchFrame;
                if (glitchFrame > 3) {
                    glitchFrame = 0;
                    glitchTimer = GLITCH_MIN_TICKS + rand() % GLITCH_SPAN_TICKS;
                    copyTopFrame(0);
                } else {
                    copyTopFrame(glitchFrame);
                }
            }
        } else if (glitchTimer > 0) {
            --glitchTimer;
            if (glitchTimer == 0) {
                glitchFrame = 1;
                copyTopFrame(glitchFrame);
            }
        }
    }

    officeClose();
    logCheckpoint("main_loop_exit");
    if(musicChannel>=0)soundKill(musicChannel);
    logCheckpoint("audio_stopped_exit");
    return 0;
}
