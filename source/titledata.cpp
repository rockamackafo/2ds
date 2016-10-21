#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include <citro3d.h>
#include <stdio.h>
#include <string>

#include "titledata.h"
#include "global.h"
#include "smdh.h"
#include "util.h"
#include "ui.h"

//frame around icons
static sf2d_texture *frame;
void frameInit()
{
    frame = sfil_load_PNG_file("romfs:/img/frame.png", SF2D_PLACE_RAM);
}

void frameFree()
{
    sf2d_free_texture(frame);
}

u32 extdataRedirect(u32 low)
{
    //Pokemon Y
    if(low == 0x00055E00)
        return 0x0000055D;
    //Pokemon OR
    else if(low == 0x0011C400)
        return 0x000011C5;
    //Fire Emblem Conquest + SE NA
    else if(low == 0x00179600 || low == 0x00179800)
        return 0x00001794;
    //FE Conquest + SE EURO
    else if(low == 0x00179700 || low == 0x0017A800)
        return 0x00001795;
    //FE If/JPN
    else if(low == 0x0012DC00 || low == 0x0012DE00 || low == 0x0016B200)
        return 0x000012DC;

    return (low >> 8);
}

//From bch2obj.py
u8 tileOrder[]={0,1,8,9,2,3,10,11,16,17,24,25,18,19,26,27,4,5,12,13,6,7,14,15,20,21,28,29,22,23,30,31,32,33,40,41,34,35,42,43,48,49,56,57,50,51,58,59,36,37,44,45,38,39,46,47,52,53,60,61,54,55,62,63};

//From 3ds_hb_menu
sf2d_texture *getIcon(smdh_s *s)
{
    sf2d_texture *ret = sf2d_create_texture(48, 48, TEXFMT_RGBA8, SF2D_PLACE_RAM);

    uint16_t *icn = s->bigIconData;
    if(!*icn)
    {
        memset(ret->tex.data, 0, ret->tex.size);
        sf2d_texture_tile32(ret);
        return ret;
    }

    int i, j, l;
    for(i = 0; i < 48; i += 8)
    {
        for(j = 0; j < 48; j += 8)
        {
            for(l = 0; l < 64; l++)
            {
                uint16_t u16Col = *icn++;
                u8 col[3];
                col[2] = (u16Col & 0x1F) << 3;
                col[1] = ((u16Col >> 5) & 0x3F) << 2;
                col[0] = ((u16Col >> 11) & 0x1F) << 3;

                u8 x = tileOrder[l] & 0x7;
                u8 y = tileOrder[l] >> 3;

                sf2d_set_pixel(ret, j + x, i + y + 16, RGBA8(col[0], col[1], col[2], 0xFF));
            }
        }
    }

    sf2d_texture_tile32(ret);

    return ret;
}

bool titleData::init(u64 _id, FS_MediaType mediaType)
{
    id = _id;
    media = mediaType;

    //split id and setup other ids
    low = (u32)id;
    high = (u32)(id >> 32);
    unique = (low >> 8);

    //some titles use a different id for extdata
    extdata = extdataRedirect(low);
    smdh_s *smdh = loadSMDH(low, high, media);
    if(smdh == NULL)
        return false;

    name = (char16_t *)smdh->applicationTitles[sysLanguage].shortDescription;
    //Default to english if title is empty for language
    if(name.empty())
        name = (char16_t *)smdh->applicationTitles[1].shortDescription;
    u32Name = tou32(name);
    nameSafe = safeString(name);
    icn = getIcon(smdh);

    //Product code
    char tmp[16];
    AM_GetTitleProductCode(media, id, tmp);
    prodCode.assign(tmp);

    initd = true;

    return true;
}

void titleData::initId()
{
    high = id >> 32;
    low = id;

    unique = (low >> 8);

    extdata = extdataRedirect(low);

    media = MEDIATYPE_SD;

    nameSafe = safeString(name);
    u32Name = tou32(name);

    initd = true;
}

void titleData::freeIcn()
{
    sf2d_free_texture(icn);
}

void titleData::printInfo()
{
    //Title
    int titleWidth = sftd_get_wtext_width(font, 12, (wchar_t *)u32Name.data());
    int titleX = (400 / 2) - (titleWidth / 2);
    sftd_draw_wtext(font, titleX, 98, COL_BLACK, 12, (wchar_t *)u32Name.data());

    //ID
    char tmp[16];
    sprintf(tmp, "%16llX", id);
    int idWidth = sftd_get_text_width(font, 12, tmp);
    int idX = (400 / 2) - (idWidth /2);
    sftd_draw_text(font, idX, 114, COL_BLACK, 12, tmp);

    //prod code
    int prodWidth = sftd_get_text_width(font, 12, prodCode.c_str());
    int prodX = (400 / 2) - (prodWidth / 2);
    sftd_draw_text(font, prodX, 130, COL_BLACK, 12, prodCode.c_str());
}

void titleData::drawIcn(int x, int y)
{
    sf2d_draw_texture(frame, x, y);
    sf2d_draw_texture_part_scale(icn, x + 2, y + 50, 0, 0, 48, 48, 1, -1);
}
