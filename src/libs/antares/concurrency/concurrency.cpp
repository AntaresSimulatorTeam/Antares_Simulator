/*
** Copyright 2007-2023 RTE
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
#include <memory>
#include "yuni/job/job.h"
#include "antares/concurrency/concurrency.h"

namespace Antares::Concurrency
{

namespace {

/*!
* Just wraps an arbitrary task as a yuni job, and allows to retrieve the corresponding future.
*/
class PackagedJob : public Yuni::Job::IJob {
public:
    PackagedJob(const Task& task) : task_(task) {}

    TaskFuture getFuture() {
        return task_.get_future();
    }

protected:
    void onExecute() override {
        task_();
    }

private:
    std::packaged_task<void()> task_;
};

}

std::future<void> AddTask(Yuni::Job::QueueService& threadPool,
                          const Task& task,
                          Yuni::Job::Priority priority) {
    auto job = std::make_unique<PackagedJob>(task);
    auto future = job->getFuture();
    threadPool.add(job.release(), priority);
    return future;
}

void FutureSet::add(TaskFuture&& f) {
    std::lock_guard lock(mutex_);
    futures_.push_back(std::move(f));
}

void FutureSet::join() {
    std::vector<TaskFuture> toBeJoined;
    {
        std::lock_guard lock(mutex_);
        std::swap(futures_, toBeJoined);
    }
    for (auto& f: toBeJoined) {
        f.get();
    }
}

}
