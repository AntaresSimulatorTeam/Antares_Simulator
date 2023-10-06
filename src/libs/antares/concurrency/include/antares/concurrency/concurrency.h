//
// Created by leclercsyl on 05/10/23.
//

#ifndef ANTARES_CONCURRENCY_H
#define ANTARES_CONCURRENCY_H

#include <future>
#include "yuni/job/queue/service.h"

namespace Antares::Concurrency
{

typedef std::function<void()> Task;
typedef std::future<void> TaskFuture;

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
     */
    void join();

private:
    std::vector<TaskFuture> futures_;
};

}


#endif //ANTARES_CONCURRENCY_H
