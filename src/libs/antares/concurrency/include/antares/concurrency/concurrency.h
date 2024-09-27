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
#ifndef ANTARES_CONCURRENCY_H
#define ANTARES_CONCURRENCY_H

#include <future>

#include "yuni/job/queue/service.h"

namespace Antares::Concurrency
{

using Task = std::function<void()>;
using TaskFuture = std::future<void>;

/*!
 * \brief Queues the provided function and returns the corresponding std::future.
 *
 * This allows to handle exceptions occuring in the underlying task,
 * as opposite to Yuni::Job::QueueService::add which swallows them.
 */
[[nodiscard]] TaskFuture AddTask(Yuni::Job::QueueService& threadPool,
                                 const Task& task,
                                 Yuni::Job::Priority priority = Yuni::Job::priorityDefault);

/*!
 * \brief Queues the provided function objects and returns the corresponding std::future.
 *
 * T must define operator ().
 *
 * This allows to handle exceptions occuring in the underlying task,
 * as opposite to Yuni::Job::QueueService::add which swallows them.
 */
template<class T>
[[nodiscard]] TaskFuture AddTask(Yuni::Job::QueueService& threadPool,
                                 const std::shared_ptr<T>& task,
                                 Yuni::Job::Priority priority = Yuni::Job::priorityDefault);

/*!
 * \brief Utility class to gather futures to wait for.
 */
class FutureSet
{
public:
    FutureSet() = default;
    ~FutureSet() = default;

    FutureSet(const FutureSet&) = delete;
    FutureSet& operator=(const FutureSet&) = delete;
    FutureSet(FutureSet&&) = delete;
    FutureSet& operator=(FutureSet&&) = delete;

    /*!
     * \brief Adds one future to be monitored by this set.
     *
     * Note: the provided future will be left in "moved from" state.
     */
    void add(TaskFuture&& f);

    /*!
     * \brief Waits for completion of all added futures.
     *
     * If one of the future ends on exception, re-throws the first encountered exception.
     * Note that futures cannot be added while some thread is waiting for completion.
     *
     * Joining also resets the list of tasks to wait for.
     */
    void join();

private:
    std::mutex mutex_;
    std::vector<TaskFuture> futures_;
};

namespace Detail
{ // implementation details

/*!
 * Utility class to wrap a callable object pointer
 * into a copyable callable object.
 *
 * @tparam T the underlying callable type
 */
template<class T>
class CopyableCallable
{
public:
    explicit CopyableCallable(const std::shared_ptr<T>& functionObject):
        functionObject_(functionObject)
    {
    }

    void operator()()
    {
        (*functionObject_)();
    }

private:
    std::shared_ptr<T> functionObject_;
};

} // namespace Detail

template<class T>
TaskFuture AddTask(Yuni::Job::QueueService& threadPool,
                   const std::shared_ptr<T>& task,
                   Yuni::Job::Priority priority)
{
    Task wrappedTask = Detail::CopyableCallable<T>(task);
    return AddTask(threadPool, wrappedTask, priority);
}

} // namespace Antares::Concurrency

#endif // ANTARES_CONCURRENCY_H
