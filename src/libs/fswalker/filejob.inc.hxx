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
#ifndef __ANTARES_FS_WALKER_FILEJOB_H__
#define __ANTARES_FS_WALKER_FILEJOB_H__

using namespace Yuni;

namespace FSWalker
{
class FileJob : public IJob
{
public:
    FileJob(EventsRegistry& events) : events(events)
    {
    }
    virtual ~FileJob()
    {
    }

public:
    //! All files
    String::Vector files;
    //! Parent
    String::Vector parents;
    //! All associated sizes
    std::vector<uint64> filessize;
    //! Modification
    std::vector<sint64> modified;
    //! Reference
    EventsRegistry& events;

protected:
    virtual void onExecute();

}; // class FileJob

void FileJob::onExecute()
{
    for (uint e = 0; e != events.file.access.size(); ++e)
    {
        // event to trigger
        auto access = events.file.access[e];
        void* user = events.file.userdata[e];

        for (uint i = 0; i != files.size(); ++i)
            (*access)(files[i], parents[i], modified[i], filessize[i], user);
    }
}

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_FILEJOB_H__
