// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FS_WALKER_FILEJOB_H__
#define __ANTARES_FS_WALKER_FILEJOB_H__

#include "registry.inc.hxx"

using namespace Yuni;

namespace FSWalker
{
class FileJob final: public IJob
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
