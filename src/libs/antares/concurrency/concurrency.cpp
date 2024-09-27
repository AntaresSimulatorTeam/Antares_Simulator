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
class PackagedJob: public Yuni::Job::IJob
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
