// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "io.h"

using namespace Yuni;

/*extern*/ bool dry = true;

/*extern*/ std::atomic<int> IOBytesDeleted = 0;
/*extern*/ std::atomic<int> IOFilesDeleted = 0;
/*extern*/ std::atomic<int> IOFoldersDeleted = 0;

/*extern*/ std::unordered_set<String> inputFolders;

bool RemoveFile(const String& filename, uint64_t size)
{
    if (dry || IO::errNone == IO::File::Delete(filename))
    {
        ++IOFilesDeleted;
        IOBytesDeleted += size;
        return true;
    }
    return false;
}

bool RemoveDirectoryIfEmpty(const String& folder)
{
    if (inputFolders.count(folder) == 0) // input folders must not be removed
    {
        if (dry)
        {
            ++IOFoldersDeleted;
            return true;
        }
        else
        {
#ifdef YUNI_OS_WINDOWS
            Yuni::WString wpath(folder);
            if (0 != RemoveDirectoryW(wpath.c_str()))
#else
            if (0 == ::rmdir(folder.c_str()))
#endif
            {
                ++IOFoldersDeleted;
                return true;
            }
        }
    }
    return false;
}
