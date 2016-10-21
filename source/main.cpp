#include <3ds.h>
#include <stdio.h>
#include <sf2d.h>
#include <sftd.h>
#include <string>

#include "sys.h"
#include "global.h"
#include "util.h"
#include "3dsx.h"
#include "titles.h"
#include "hbfilter.h"

int main(int argc, const char * argv[])
{
    hidInit();
    hidScanInput();
    u32 held = hidKeysHeld();
    if((held & KEY_R) && (held & KEY_L))
        devMode = true;
    //This is for making sure I didn't butcher the font
    else if(held & KEY_R)
        sysLanguage = CFG_LANGUAGE_JP;

    sysInit();

    if(runningUnder() && !devMode && !isPogeybank())
    {
        hbl = true;
        start3dsxMode();
    }
    else
    {
        loadFilterList();

        sdTitlesInit();
        nandTitlesInit();

        if(isPogeybank())
        {
            hbl = true;
            fsStart();
        }

        while(aptMainLoop() && !kill)
        {
            handleState();
        }

        if(isPogeybank())
            fsEnd();
    }

    sysExit();
    return 0;
}
