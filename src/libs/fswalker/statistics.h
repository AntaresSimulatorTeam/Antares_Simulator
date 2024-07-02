/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_FS_WALKER_STATISTICS_H__
#define __ANTARES_FS_WALKER_STATISTICS_H__

#include <yuni/yuni.h>

namespace FSWalker
{
class Statistics
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
