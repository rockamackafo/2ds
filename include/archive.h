#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "titledata.h"

bool openArchive(FS_Archive *out, FS_ArchiveID id, const titleData dat, bool error);

#endif // ARCHIVE_H
