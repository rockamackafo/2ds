#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include <stdio.h>
#include <string>

#include "global.h"
#include "date.h"
#include "sys.h"
#include "menu.h"
#include "cart.h"
#include "sd_cia.h"
#include "titledata.h"
#include "backupmenu.h"
#include "savemenu.h"
#include "extmenu.h"
#include "titles.h"
#include "nand.h"
#include "hbfilter.h"
#include "extra.h"
#include "shared.h"
#include "dev.h"
#include "ui.h"

//Transfer buffer size
unsigned buff_size = 0x10000;

//font
sftd_font * font;

//top bar texture
static sf2d_texture * bar, *bgTop, *bgBot;

//selected frame. Needed for both cia and nand, so it goes here.
sf2d_texture *selFrame;

//ID the program is using
u64 runningID;

//SDMC archive
FS_Archive sdArch;

/*
hbl = homebrew loader mode
devMode = devMode
kill = kill program
*/
bool hbl = false, devMode = false, kill = false;

//config
bool centered = true, autoBack = false, useLang = false;
u8 sysLanguage = 1;

//default colors
u8 clearColor[3] = {0, 0, 0};
u8 selColor[3] = {0, 255, 0};
u8 unSelColor[3] = {128, 128, 128};

//controls menus shown. Used so home button works.
int state = states::STATE_MAINMENU, prevState = states::STATE_MAINMENU;

//Pointer to sdTitle being used.
const titleData *curTitle = NULL;

void globalImgsInit()
{
    bar = sfil_load_PNG_file("romfs:/img/topBar.png", SF2D_PLACE_RAM);
    bgTop = sfil_load_PNG_file("romfs:/img/backTop.png", SF2D_PLACE_RAM);
    bgBot = sfil_load_PNG_file("romfs:/img/backBottom.png", SF2D_PLACE_RAM);
    selFrame = sfil_load_PNG_file("romfs:/img/selFrame.png", SF2D_PLACE_RAM);
}

void globalImgsExit()
{
    sf2d_free_texture(bar);
    sf2d_free_texture(bgTop);
    sf2d_free_texture(bgBot);
    sf2d_free_texture(selFrame);
}

void bgDrawTop()
{
    sf2d_draw_texture(bgTop, 0, 0);
}

void bgDrawBottom()
{
    sf2d_draw_texture(bgBot, 0, 0);
}

//draws the bar shown up top
void drawTopBar(const std::u32string nfo)
{
    //the bar
    sf2d_draw_texture_scale(bar, 0, 0, 25, 1);

    //nfo is wide text, UTF32
    sftd_draw_wtext(font, 0, 0, RGBA8(0, 0, 0, 255), 12, (wchar_t *)nfo.data());

    //time
    sftd_draw_text(font, 360, 0, RGBA8(0, 0, 0, 255), 12, RetTime().c_str());
}

//I needed a quick way to get most of it under one loop without having to completely rewrite it
//This is what I came up with.
void handleState()
{
    switch(state)
    {
        case states::STATE_MAINMENU:
            mainMenu();
            break;
        case states::STATE_CARTMENU:
            cartManager();
            break;
        case states::STATE_CIAMENU:
            sdStartSelect();
            break;
        case states::STATE_BACKUPMENU:
            backupMenu();
            break;
        case states::STATE_SAVEMENU:
            showSaveMenu();
            break;
        case states::STATE_EXTMENU:
            showExtMenu();
            break;
        case states::STATE_NANDSELECT:
            nandStartSelect();
            break;
        case states::STATE_NANDBACKUP:
            nandBackup();
            break;
        case states::STATE_EXTRAS:
            extrasMenu();
            break;
        case states::STATE_SHARED:
            sharedExtManager();
            break;
        case states::STATE_SHAREDBACKUP:
            sharedBackupMenu();
            break;
        case states::STATE_DEVMENU:
            showDevMenu();
            break;
        default:
            showMessage("This shouldn't happen.", "Umm...");
            state = states::STATE_MAINMENU;
            break;
    }
}

void killApp(u32 up)
{
    if(up & KEY_START)
        kill = true;
}

enum mMenuOpts
{
    cart,
    cia,
    sys,
    shared,
    ref,
    filter,
    extra,
    exit,
    dev
};

static menu mMenu(136, 80, false, true);

void prepMain()
{
    mMenu.addItem("Cartridge");
    mMenu.addItem("SD/CIA");
    mMenu.addItem("System Titles");
    mMenu.addItem("Shared ExtData");
    mMenu.addItem("Refresh Games");
    mMenu.addItem("Download Filter");
    mMenu.addItem("Config/Extras");
    mMenu.addItem("Exit");

    if(devMode)
        mMenu.addItem("Dev");

    mMenu.autoVert();
}

void mainMenu()
{

    hidScanInput();

    u32 down = hidKeysDown();

    mMenu.handleInput(down, 0);
    if(down & KEY_A)
    {
        switch(mMenu.getSelected())
        {
            case mMenuOpts::cart:
                state = states::STATE_CARTMENU;
                break;
            case mMenuOpts::cia:
                state = states::STATE_CIAMENU;
                break;
            case mMenuOpts::sys:
                state = states::STATE_NANDSELECT;
                break;
            case mMenuOpts::shared:
                state = states::STATE_SHARED;
                break;
            case mMenuOpts::ref:
                remove("titles");
                sdTitlesInit();
                break;
            case mMenuOpts::filter:
                remove("filter.txt");
                loadFilterList();
                break;
            case mMenuOpts::extra:
                state = STATE_EXTRAS;
                break;
            case mMenuOpts::exit:
                kill = true;
                break;
            case mMenuOpts::dev:
                state = STATE_DEVMENU;
                break;
        }
    }
    else if(down & KEY_B)
        kill = true;

    killApp(down);

    sf2d_start_frame(GFX_TOP, GFX_LEFT);
    bgDrawTop();
    drawTopBar(std::u32string(U"JKSV - " + BUILD_DATE));
    mMenu.draw();
    sf2d_end_frame();

    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
    bgDrawBottom();
    sf2d_end_frame();

    sf2d_swapbuffers();
}
