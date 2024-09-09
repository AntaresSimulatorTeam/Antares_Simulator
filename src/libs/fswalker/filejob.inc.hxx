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
#ifndef __ANTARES_FS_WALKER_FILEJOB_H__
#define __ANTARES_FS_WALKER_FILEJOB_H__

#include "registry.inc.hxx"

using namespace Yuni;

namespace FSWalker
{
class FileJob: public IJob
{
public:
    FileJob(EventsRegistry& events):
        events(events)
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
    std::vector<uint64_t> filessize;
    //! Modification
    std::vector<int64_t> modified;
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
        {
            (*access)(files[i], parents[i], modified[i], filessize[i], user);
        }
    }
}

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_FILEJOB_H__
