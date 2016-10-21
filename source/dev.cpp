#include <3ds.h>
#include <sf2d.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "dev.h"
#include "menu.h"
#include "util.h"
#include "global.h"

#include "button.h"
#include "titles.h"
#include "ui.h"

//This is just for testing stuff.

static menu devMenu(0, 0, false, true);

void prepDevMenu()
{
    devMenu.addItem("fsStart");
    devMenu.addItem("fsEnd");
    devMenu.addItem("icnTest");
    devMenu.addItem("Back");

    devMenu.autoVert();
}

enum devOpts
{
    _fsStart,
    _fsEnd,
    _icnTest,
    _back
};

void keyWait(uint32_t key)
{

    while(1)
    {
        hidScanInput();

        if(hidKeysDown() & key)
            break;
    }
}

extern std::vector<button> titleButtons;

void icnTest()
{
    for(uint i = 0; i < titleButtons.size(); i++)
    {
        sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
            titleButtons[i].draw();
        sf2d_end_frame();

        sf2d_swapbuffers();

        keyWait(KEY_A);
    }
}

void showDevMenu()
{
    hidScanInput();

    u32 down = hidKeysDown();

    devMenu.handleInput(down, 0);

    if(down & KEY_A)
    {
        switch(devMenu.getSelected())
        {
            case devOpts::_fsStart:
                fsStart();
                break;
            case devOpts::_fsEnd:
                fsEnd();
                break;
            case devOpts::_icnTest:
                icnTest();
                break;
            case devOpts::_back:
                state = STATE_MAINMENU;
                break;
        }
    }
    else if(down & KEY_B)
        state = STATE_MAINMENU;

    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        drawTopBar(U"DevMenu");
        devMenu.draw();
    sf2d_end_frame();

    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
    sf2d_end_frame();

    sf2d_swapbuffers();
}
