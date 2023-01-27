/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_FS_WALKER_STATISTICS_H__
#define __ANTARES_FS_WALKER_STATISTICS_H__

#include <yuni/yuni.h>

namespace FSWalker
{
class Statistics
{
public:
    Statistics() : fileCount(0), folderCount(0), bytesCopied(0), aborted(true)
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
    yuint64 fileCount;
    //! The number of folders encountered
    yuint64 folderCount;
    //! The amount of bytes that has been copied
    yuint64 bytesCopied;
    //! Flag to determine whether the operation has been aborted or not
    bool aborted;

}; // class Statistics

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_STATISTICS_H__
