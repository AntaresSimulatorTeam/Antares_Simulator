// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/concurrency/concurrency.h"

namespace Antares::Concurrency
{

ThreadPool::ThreadPool(unsigned threadCount)
{
    start(threadCount);
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    // workers_ are std::jthread: they join on destruction, after having
    // drained the remaining tasks (see workerLoop exit condition).
}

void ThreadPool::start(unsigned threadCount)
{
    std::lock_guard lock(mutex_);
    if (!workers_.empty())
    {
        return;
    }
    workers_.reserve(threadCount);
    for (unsigned i = 0; i < threadCount; ++i)
    {
        workers_.emplace_back([this] { workerLoop(); });
    }
}

TaskFuture ThreadPool::add(Task task)
{
    std::packaged_task<void()> packagedTask(std::move(task));
    auto future = packagedTask.get_future();
    {
        std::lock_guard lock(mutex_);
        tasks_.push_back(std::move(packagedTask));
    }
    condition_.notify_one();
    return future;
}

void ThreadPool::workerLoop()
{
    while (true)
    {
        std::packaged_task<void()> task;
        {
            std::unique_lock lock(mutex_);
            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
            if (tasks_.empty())
            {
                // stop_ is necessarily true here: drain complete, exit
                return;
            }
            task = std::move(tasks_.front());
            tasks_.pop_front();
        }
        // Run outside the lock. Exceptions are captured by the packaged_task
        // and delivered through the associated future; the worker survives.
        task();
    }
}

TaskFuture AddTask(ThreadPool& threadPool, const Task& task)
{
    return threadPool.add(task);
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
