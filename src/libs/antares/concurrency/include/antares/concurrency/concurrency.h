//
// Created by leclercsyl on 05/10/23.
//

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
TaskFuture AddTask(Yuni::Job::QueueService& threadPool,
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
     */
    void join();

private:
    std::mutex mutex_;
    std::vector<TaskFuture> futures_;
};

}


#endif //ANTARES_CONCURRENCY_H
