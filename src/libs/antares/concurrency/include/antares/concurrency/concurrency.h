// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_CONCURRENCY_H
#define ANTARES_CONCURRENCY_H

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace Antares::Concurrency
{

using Task = std::function<void()>;
using TaskFuture = std::future<void>;

/*!
 * \brief A fixed-size pool of worker threads consuming tasks in FIFO order.
 *
 * Worker threads are spawned by the constructor and run until destruction.
 * The destructor completes all queued tasks before joining the workers.
 */
class ThreadPool
{
public:
    explicit ThreadPool(unsigned threadCount);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /*!
     * \brief Queues the provided task and returns the corresponding std::future.
     *
     * Exceptions thrown by the task are delivered through the future.
     */
    [[nodiscard]] TaskFuture add(Task task);

private:
    void workerLoop();

    std::mutex mutex_;
    std::condition_variable condition_;
    std::deque<std::packaged_task<void()>> tasks_;
    bool stop_ = false;
    // Declared last so that workers join before the other members are destroyed
    std::vector<std::jthread> workers_;
};

/*!
 * \brief Queues the provided function and returns the corresponding std::future.
 *
 * This allows to handle exceptions occuring in the underlying task.
 */
[[nodiscard]] TaskFuture AddTask(ThreadPool& threadPool, const Task& task);

/*!
 * \brief Queues the provided function objects and returns the corresponding std::future.
 *
 * T must define operator ().
 *
 * This allows to handle exceptions occuring in the underlying task.
 */
template<class T>
[[nodiscard]] TaskFuture AddTask(ThreadPool& threadPool, const std::shared_ptr<T>& task);

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
TaskFuture AddTask(ThreadPool& threadPool, const std::shared_ptr<T>& task)
{
    Task wrappedTask = Detail::CopyableCallable<T>(task);
    return AddTask(threadPool, wrappedTask);
}

} // namespace Antares::Concurrency

#endif // ANTARES_CONCURRENCY_H
