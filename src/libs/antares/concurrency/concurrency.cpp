// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/concurrency/concurrency.h"

#include <memory>

#include "yuni/job/job.h"

namespace Antares::Concurrency
{

namespace
{

/*!
 * Just wraps an arbitrary task as a yuni job, and allows to retrieve the corresponding future.
 */
class PackagedJob final: public Yuni::Job::IJob
{
public:
    PackagedJob(const Task& task):
        task_(task)
    {
    }

    TaskFuture getFuture()
    {
        return task_.get_future();
    }

protected:
    void onExecute() override
    {
        task_();
    }

private:
    std::packaged_task<void()> task_;
};

} // namespace

std::future<void> AddTask(Yuni::Job::QueueService& threadPool,
                          const Task& task,
                          Yuni::Job::Priority priority)
{
    auto job = std::make_unique<PackagedJob>(task);
    auto future = job->getFuture();
    threadPool.add(job.release(), priority);
    return future;
}

void FutureSet::add(TaskFuture&& f)
{
    std::lock_guard lock(mutex_);
    futures_.push_back(std::move(f));
}

void FutureSet::join()
{
    std::vector<TaskFuture> toBeJoined;
    {
        std::lock_guard lock(mutex_);
        std::swap(futures_, toBeJoined);
    }
    for (auto& f: toBeJoined)
    {
        f.get();
    }
}

} // namespace Antares::Concurrency
