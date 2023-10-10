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

}


#endif //ANTARES_CONCURRENCY_H
