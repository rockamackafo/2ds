#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include <string>
#include <stdio.h>

#include "ui.h"
#include "textbox.h"
#include "button.h"
#include "global.h"

bool confirm(const char *t)
{
    bool ret = false;

    button yes("Yes (A)", 40, 176, 96, 32);
    button no("No (B)", 180, 176, 96, 32);
    textbox back(24, 24, 280, 200, t, "Confirm");
    while(true)
    {
        hidScanInput();

        u32 down = hidKeysDown();

        touchPosition p;
        hidTouchRead(&p);

        yes.update(p), no.update(p);

        if(yes.released() || (down & KEY_A))
        {
            ret = true;
            break;
        }
        else if(no.released() || (down & KEY_B))
        {
            ret = false;
            break;
        }

        sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        back.draw();
        yes.draw();
        no.draw();
        sf2d_end_frame();

        sf2d_swapbuffers();
    }

    return ret;
}

void showMessage(const char *t, const char *head)
{
    button ok("OK (A)", 116, 176, 96, 32);
    textbox back(24, 24, 280, 200, t, head);

    while(true)
    {
        hidScanInput();

        touchPosition p;
        hidTouchRead(&p);

        ok.update(p);

        u32 down = hidKeysDown();

        if(ok.released() || down & KEY_A)
            break;

        sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        back.draw();
        ok.draw();
        sf2d_end_frame();

        sf2d_swapbuffers();
    }
}

void showError(const char *t, unsigned error)
{
    char full[256];
    sprintf(full, "%s : %08X", t, error);
    button ok("OK (A)", 116, 176, 96, 32);
    textbox back(24, 24, 280, 200, full, "Error:");

    while(true)
    {
        hidScanInput();

        touchPosition p;
        hidTouchRead(&p);

        ok.update(p);

        u32 down = hidKeysDown();

        if(ok.released() || down & KEY_A)
            break;

        sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        back.draw();
        ok.draw();
        sf2d_end_frame();

        sf2d_swapbuffers();
    }
}

static sf2d_texture *progFull;

void progressBarInit()
{
    progFull = sfil_load_PNG_file("romfs:/img/progBar.png", SF2D_PLACE_RAM);
}

void progressBarExit()
{
    sf2d_free_texture(progFull);
}

progressBar::progressBar(float _max, const char *t, const char *head)
{
    back = new textbox(24, 24, 280, 200, t, head);
    max = _max;
}

progressBar::~progressBar()
{
    delete back;
}

void progressBar::draw(float cur)
{
    float prog = (float)(cur / max) * 100;
    float xScale = (float)(prog * 128) / 100;

    back->draw();
    sf2d_draw_rectangle(back->X + 8, back->Y + 160, 256, 16, RGBA8(0, 0, 0, 255));
    sf2d_draw_texture_scale(progFull, back->X + 8, back->Y + 160, xScale, 1);
}
