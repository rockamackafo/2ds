#include <3ds.h>

#include "titledata.h"
#include "archive.h"
#include "ui.h"

//All the information used here was found on 3dbrew.org
//thank them too.

bool openArchive(FS_Archive *out, FS_ArchiveID id, const titleData dat, bool error)
{
    Result res = 0;
    bool ret = false;

    std::string errorArch = "";

    switch(id)
    {
        case ARCHIVE_SAVEDATA:
            errorArch = "ARCHIVE_SAVEDATA";
            res = FSUSER_OpenArchive(out, ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, ""));
            if(res == 0) ret = true;
            break;

        case ARCHIVE_USER_SAVEDATA:
            {
                errorArch = "ARCHIVE_USER_SAVEDATA";
                u32 path[3] = {dat.media, dat.low, dat.high};
                res = FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, path});
                if(res == 0) ret = true;
            }
            break;

        case ARCHIVE_EXTDATA:
            {
                errorArch = "ARCHIVE_EXTDATA";
                u32 path[3] = {MEDIATYPE_SD, dat.extdata, 0};
                res = FSUSER_OpenArchive(out, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 0xC, path});
                if(res == 0) ret = true;
            }
            break;

        case ARCHIVE_SHARED_EXTDATA:
            {
                errorArch = "ARCHIVE_SHARED_EXTDATA";
                u32 path[3] = {MEDIATYPE_NAND, dat.extdata, 0x00048000};
                res = FSUSER_OpenArchive(out, ARCHIVE_SHARED_EXTDATA, (FS_Path){PATH_BINARY, 0xC, path});
                if(res == 0) ret = true;
            }
            break;

        case ARCHIVE_BOSS_EXTDATA:
            {
                errorArch = "ARCHIVE_BOSS_EXTDATA";
                u32 path[3] = {MEDIATYPE_SD, dat.extdata, 0};
                res = FSUSER_OpenArchive(out, ARCHIVE_BOSS_EXTDATA, (FS_Path){PATH_BINARY, 0xC, path});
                if(res == 0) ret = true;
            }
            break;

        case ARCHIVE_SYSTEM_SAVEDATA:
            {
                errorArch = "ARCHIVE_SYSTEM_SAVEDATA";
                u32 path[2] = {MEDIATYPE_NAND, 0x00020000 | dat.unique};
                res = FSUSER_OpenArchive(out, ARCHIVE_SYSTEM_SAVEDATA, (FS_Path){PATH_BINARY, 0x8, path});
                if(res == 0) ret = true;
            }
            break;

        case ARCHIVE_SDMC:
            {
                errorArch = "ARCHIVE_SDMC";
                res = FSUSER_OpenArchive(out, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
                if(res == 0) ret = true;
            }

        default:
            break;

    }

    if(res != 0 && error)
    {
        std::string fullError = "Error opening " + errorArch;
        showError(fullError.c_str(), (unsigned int)res);
    }

    return ret;
}
