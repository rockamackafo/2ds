#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <string.h>
#include <string>
#include <vector>

#include "titles.h"
#include "nand.h"
#include "backup.h"
#include "restore.h"
#include "util.h"
#include "ui.h"
#include "global.h"
#include "menu.h"
#include "titledata.h"
#include "archive.h"
#include "button.h"

std::vector<button> nandButtons;

static button prev("<-", 0, 0, 32, 32), next("->", 288, 0, 32, 32), backButton("Back", 0, 208, 48, 32);

static uint16_t page = 0, maxPage = 0, selected = 0;

const titleData * sysTitle = NULL;

void prepNandSelect()
{
    nandButtons.clear();
    maxPage = 0;
    uint i, x = 23, y = 38;
    for(i = 0; i < nandTitle.size(); i++, x += 56)
    {
        if(x > 280)
        {
            x = 23;
            y += 56;
            if(y > 194)
            {
                maxPage++;
                y = 38;
            }
        }

        button newNandButton(&nandTitle[i], x, y);
        nandButtons.push_back(newNandButton);
    }
}

void nandStartSelect()
{
    hidScanInput();

    u32 down = hidKeysDown();

    touchPosition pos;
    hidTouchRead(&pos);

    next.update(pos);
    prev.update(pos);
    backButton.update(pos);

    if((down & KEY_L) || prev.released())
    {
        if(page == 0)
            page = maxPage;
        else
        {
            --page;
            if(selected - 15 < 0)
                selected = 0;
            else
                selected -= 15;
        }
    }
    else if(down & KEY_R || next.released())
    {
        if(page == maxPage)
        {
            page = 0;
            selected = 0;
        }
        else
        {
            page++;
            if((uint)selected + 15 < nandButtons.size())
                selected += 15;
            else
                selected = nandButtons.size() - 1;
        }
    }
    else if(down & KEY_DOWN)
    {
        selected += 5;
    }
    else if(down & KEY_UP)
    {
        selected -= 5;
    }
    else if((down & KEY_LEFT) && (selected > 0))
    {
        --selected;
    }
    else if((down & KEY_RIGHT) && (selected < nandButtons.size() - 1))
    {
        ++selected;
    }
    else if(down & KEY_A)
    {
        sysTitle = &nandTitle[selected];
        state = STATE_NANDBACKUP;
    }
    else if((down & KEY_B) || backButton.released())
    {
        state = STATE_MAINMENU;
        return;
    }

    //nandButtons begin
    u8 disp = 0;
    if((uint)(page * 15 + 15) > nandButtons.size())
    {
        disp = nandButtons.size() - ((page - 1) * 15 + 15);
    }
    else
        disp = 15;


    for(uint16_t i = (page * 15); i < (page * 15) + disp; i++)
    {
        nandButtons[i].update(pos);

        if(selected == i && nandButtons[i].released())
        {
            sysTitle = &nandTitle[selected];
            state = STATE_NANDBACKUP;
        }
        else if(nandButtons[i].released())
            selected = i;

    }
    //nandButtons end

    killApp(down);

    sf2d_start_frame(GFX_TOP, GFX_LEFT);
    bgDrawTop();
    drawTopBar(U"Select System Title");
    nandTitle[selected].printInfo();
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
            sf2d_draw_texture(selFrame, nandButtons[i].getX() - 2, nandButtons[i].getY() - 2);

        nandButtons[i].draw();
    }

    next.draw();
    prev.draw();
    backButton.draw();
    sf2d_end_frame();

    sf2d_swapbuffers();
}

enum sysOpts
{
    expSys,
    impSys,
    expExt,
    impExt,
    expBoss,
    impBoss,
    back
};

static menu nBackupMenu(112, 72, false, true);

void prepNandBackup()
{
    nBackupMenu.addItem("Export System Save");
    nBackupMenu.addItem("Import System Save");
    nBackupMenu.addItem("Export ExtData");
    nBackupMenu.addItem("Import ExtData");
    nBackupMenu.addItem("Export Boss ExtData");
    nBackupMenu.addItem("Import Boss ExtData");
    nBackupMenu.addItem("Back");

    nBackupMenu.autoVert();
}

void nandBackup()
{
    std::u32string info = tou32(sysTitle->name) + U" : NAND";

    hidScanInput();

    u32 down = hidKeysDown();

    nBackupMenu.handleInput(down, 0);

    if(down & KEY_A)
    {
        FS_Archive arch;
        switch(nBackupMenu.getSelected())
        {
            case sysOpts::expSys:
                if(openArchive(&arch, ARCHIVE_SYSTEM_SAVEDATA, *sysTitle, true))
                {
                    createTitleDir(*sysTitle, MODE_SYSSAVE);
                    backupData(*sysTitle, arch, MODE_SYSSAVE, false);
                }
                break;
            case sysOpts::impSys:
                if(openArchive(&arch, ARCHIVE_SYSTEM_SAVEDATA, *sysTitle, true))
                {
                    restoreData(*sysTitle, arch, MODE_SYSSAVE);
                }
                break;
            case sysOpts::expExt:
                if(openArchive(&arch, ARCHIVE_EXTDATA, *sysTitle, true))
                {
                    createTitleDir(*sysTitle, MODE_EXTDATA);
                    backupData(*sysTitle, arch, MODE_EXTDATA, false);
                }
                break;
            case sysOpts::impExt:
                if(openArchive(&arch, ARCHIVE_EXTDATA, *sysTitle, true))
                {
                    restoreData(*sysTitle, arch, MODE_EXTDATA);
                }
                break;
            case sysOpts::expBoss:
                if(openArchive(&arch, ARCHIVE_BOSS_EXTDATA, *sysTitle, true))
                {
                    createTitleDir(*sysTitle, MODE_BOSS);
                    backupData(*sysTitle, arch, MODE_BOSS, false);
                }
                break;
            case sysOpts::impBoss:
                if(openArchive(&arch, ARCHIVE_BOSS_EXTDATA, *sysTitle, true))
                {
                    restoreData(*sysTitle, arch, MODE_BOSS);
                }
                break;
            case sysOpts::back:
                state = states::STATE_NANDSELECT;
                break;
        }
        FSUSER_CloseArchive(arch);
    }
    else if(down & KEY_B)
        state = states::STATE_NANDSELECT;

    killApp(down);

    sf2d_start_frame(GFX_TOP, GFX_LEFT);
    drawTopBar(info);
    nBackupMenu.draw();
    sf2d_end_frame();

    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
    sf2d_end_frame();

    sf2d_swapbuffers();


}
