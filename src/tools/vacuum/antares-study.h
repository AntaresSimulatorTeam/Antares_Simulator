// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_STUDY_H__
#define __ANTARES_STUDY_H__

#include <fswalker/fswalker.h>
#include <mutex>

class AntaresStudy final: public FSWalker::IExtension
{
public:
    AntaresStudy(int64_t dateLimit);

    virtual ~AntaresStudy()
    {
    }

    virtual const char* caption() const;

    virtual FSWalker::OnDirectoryEvent directoryEvent();

    virtual int priority() const
    {
        return 9999;
    } // must be first

    virtual void* userdataCreate(FSWalker::DispatchJobEvent& queue);

    virtual void userdataDestroy(void* userdata);

public:
    uint64_t bytesDeleted;
    uint64_t filesDeleted;
    uint64_t foldersDeleted;

private:
    std::mutex pMutex;
    int64_t pDateLimit;

}; // class AntaresStudy

#endif // __ANTARES_STUDY_H__
