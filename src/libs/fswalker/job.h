// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FS_WALKER_JOB_H__
#define __ANTARES_FS_WALKER_JOB_H__

#include <atomic>
#include <memory>

#include <yuni/yuni.h>
#include <yuni/job/job.h>

namespace FSWalker
{
// Forward declaration
class Walker;
class WalkerThread;

//! Internal job counter
using InternalJobCounter = std::shared_ptr<std::atomic<int>>;

class IJob: public Yuni::Job::IJob
{
public:
    //! The most suitable smart pointer for the class
    using Ptr = Yuni::Job::IJob::Ptr::Promote<IJob>::Ptr;

public:
    IJob()
    {
    }

    virtual ~IJob();

protected:
    void decrementCounter();

protected:
    InternalJobCounter pJobCounter;
    friend class WalkerThread;
};

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_JOB_H__
