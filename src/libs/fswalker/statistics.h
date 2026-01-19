// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FS_WALKER_STATISTICS_H__
#define __ANTARES_FS_WALKER_STATISTICS_H__

#include <yuni/yuni.h>

namespace FSWalker
{
class Statistics final
{
public:
    Statistics():
        fileCount(0),
        folderCount(0),
        bytesCopied(0),
        aborted(true)
    {
    }

    void reset()
    {
        fileCount = 0;
        folderCount = 0;
        bytesCopied = 0;
        aborted = true;
    }

public:
    //! The number of file encountered
    uint64_t fileCount;
    //! The number of folders encountered
    uint64_t folderCount;
    //! The amount of bytes that has been copied
    uint64_t bytesCopied;
    //! Flag to determine whether the operation has been aborted or not
    bool aborted;

}; // class Statistics

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_STATISTICS_H__
