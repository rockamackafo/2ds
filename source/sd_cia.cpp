#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "titles.h"
#include "menu.h"
#include "global.h"
#include "util.h"
#include "ui.h"
#include "button.h"
#include "backup.h"
#include "restore.h"

std::vector<button> titleButtons;
static button next("->", 288, 0, 32, 32), prev("<-", 0, 0, 32, 32), backButton("Back", 0, 208, 48, 32);

static const std::string helpText = "Press L to select multiple. Press R to select all. Press Y to backup selected. Press X to restore selected.";

static uint16_t page = 0, maxPages = 0, selected = 0;

void prepSDSelect()
{
    titleButtons.clear();
    unsigned int i, x = 20, y = 36;
    for(i = 0; i < sdTitle.size(); i++, x += 56)
    {
        if(x > 280)
        {
            x = 20;
            y += 56;
            if(y > 194)
            {
                y = 36;
                maxPages++;
            }
        }

        button newTitleButton(&sdTitle[i], x, y);
        titleButtons.push_back(newTitleButton);
    }
}

void sdStartSelect()
{
    if(sdTitle.size() == 0)
    {
        showMessage("No installed titles were found!", "Nope...");
        state = states::STATE_MAINMENU;
        return;
    }

    hidScanInput();

    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();

    touchPosition pos;
    hidTouchRead(&pos);

    next.update(pos);
    prev.update(pos);
    backButton.update(pos);

    if(down & KEY_L || prev.released())
    {
        if(page  == 0)
            page = maxPages;
        else
            --page;
    }
    else if(down & KEY_R || next.released())
    {
        if(page == maxPages)
            page = 0;
        else
            ++page;
    }
    else if(down & KEY_DOWN)
    {
        selected += 5;
    }
    else if(down & KEY_UP)
    {
        selected -= 5;
    }
    else if((down & KEY_LEFT) && selected > 0)
    {
        selected--;
    }
    else if((down & KEY_RIGHT) && selected < titleButtons.size())
    {
        selected++;
    }
    else if(down & KEY_A)
    {
        curTitle = titleButtons[selected].getDat();
        prevState = STATE_CIAMENU;
        state = STATE_BACKUPMENU;
    }
    else if((down & KEY_B) || backButton.released())
    {
        state = STATE_MAINMENU;
        return;
    }


    //titleButtons begin

    //number of titles to display
    u8 disp;
    if((uint)(page * 15 + 15) > titleButtons.size())
    {
        disp = titleButtons.size() - ((page - 1) * 15 + 15);
    }
    else
        disp = 15;

    for(uint16_t i = (page * 15); i < (page * 15) + disp; i++)
    {
        titleButtons[i].update(pos);

        if(selected == i && titleButtons[i].released())
        {
            curTitle = titleButtons[i].getDat();
            prevState = STATE_CIAMENU;
            state = STATE_BACKUPMENU;
        }
        else if(titleButtons[i].released())
            selected = i;
    }

    //titleButtons end

    killApp(down);

    sf2d_start_frame(GFX_TOP, GFX_LEFT);
    bgDrawTop();
    drawTopBar(U"Select a title");
    sdTitle[selected].printInfo();
    if(devMode)
    {
        sftd_draw_textf(font, 0, 16, COL_BLACK, 12, "Page: %u", page);
        sftd_draw_textf(font, 0, 32, COL_BLACK, 12, "FPS: %f", sf2d_get_fps());
    }
    sf2d_end_frame();

    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
    bgDrawBottom();
    for(uint16_t i = (page * 15); i < (page * 15) + disp; i++)
    {
        if(selected == i)
            sf2d_draw_texture(selFrame, titleButtons[i].getX() - 2, titleButtons[i].getY() - 2);
        titleButtons[i].draw();
    }
    next.draw();
    prev.draw();
    backButton.draw();
    sf2d_end_frame();

    sf2d_swapbuffers();
}
