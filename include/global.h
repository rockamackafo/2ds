#ifndef GLOBAL_H
#define GLOBAL_H

#include <sf2d.h>
#include <sftd.h>

#include <string>
#include <vector>

#include "titledata.h"

//this is for backup/restore modes
#define MODE_SAVE 0
#define MODE_EXTDATA 1
#define MODE_SYSSAVE 2
#define MODE_BOSS 3
#define MODE_SHARED 4

#define BUILD_DATE std::u32string(U"10-21-2016")

//Needed for bank workaround / ASLR
#define POKEBANK 0x00040000000C9B00

#define COL_BLACK RGBA8(0, 0, 0, 255)

typedef unsigned int uint;

extern int state, prevState;
extern const titleData *curTitle;
extern u8 sysLanguage;
extern u64 runningID;

extern sf2d_texture *selFrame;

enum states
{
    STATE_MAINMENU,
    STATE_CARTMENU,
    STATE_CIAMENU,
    STATE_BACKUPMENU,
    STATE_SAVEMENU,
    STATE_EXTMENU,
    STATE_NANDSELECT,
    STATE_NANDBACKUP,
    STATE_EXTRAS,
    STATE_SHARED,
    STATE_SHAREDBACKUP,
    STATE_DEVMENU
};

void handleState();

//buffer size
extern unsigned buff_size;

//This is the font used to print everything
extern sftd_font *font;

//This is for the bar you see at the top
void globalImgsInit();
void globalImgsExit();

//bg
void bgDrawTop();
void bgDrawBottom();

//SDMC Archive. Seems to write faster using this. Might just be me though
extern FS_Archive sdArch;

//This draws the bar you see at the top
void drawTopBar(const std::u32string nfo);

extern bool devMode, hbl, kill;
//config
extern bool centered, autoBack, useLang;

//Allows app to be killed by hitting start
void killApp(u32 up);

//got tired of black, gray, and green all the time
extern u8 clearColor[3];
extern u8 selColor[3];
extern u8 unSelColor[3];

void mainMenu();

#endif
