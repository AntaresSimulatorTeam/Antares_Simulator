// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - concurrency tests
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/concurrency/concurrency.h"

using namespace Antares::Concurrency;

std::unique_ptr<ThreadPool> createThreadPool(unsigned size)
{
    return std::make_unique<ThreadPool>(size);
}

BOOST_AUTO_TEST_CASE(test_no_error)
{
    auto threadPool = createThreadPool(1);
    int counter = 0;
    Task incrementCounter = [&counter]() { counter++; };
    TaskFuture future = AddTask(*threadPool, incrementCounter);
    future.get();
    BOOST_CHECK(counter == 1);
}

template<class Exc>
Task failingTask()
{
    return []() { throw Exc(); };
}

class TestException final
{
};

BOOST_AUTO_TEST_SUITE(test_concurrency)

BOOST_AUTO_TEST_CASE(test_throw)
{
    auto threadPool = createThreadPool(1);
    TaskFuture future = AddTask(*threadPool, failingTask<TestException>());
    BOOST_CHECK_THROW(future.get(), TestException);
}

BOOST_AUTO_TEST_CASE(test_future_set)
{
    auto threadPool = createThreadPool(4);
    std::atomic<int> counter = 0;
    Task incrementCounter = [&counter]() { counter++; };
    FutureSet futures;
    for (int i = 0; i < 10; i++)
    {
        futures.add(AddTask(*threadPool, incrementCounter));
    }
    futures.join();
    BOOST_CHECK(counter == 10);
}

template<int N>
class TestExceptionN final
{
};

BOOST_AUTO_TEST_CASE(test_future_set_rethrows_first_submitted)
{
    auto threadPool = createThreadPool(2);
    FutureSet futures;
    futures.add(AddTask(*threadPool, failingTask<TestExceptionN<1>>()));
    futures.add(AddTask(*threadPool, failingTask<TestExceptionN<2>>()));
    BOOST_CHECK_THROW(futures.join(), TestExceptionN<1>);
}

struct NonCopyableFunctionObject
{
    NonCopyableFunctionObject() = default;
    NonCopyableFunctionObject(const NonCopyableFunctionObject&) = delete;
    NonCopyableFunctionObject& operator=(const NonCopyableFunctionObject&) = delete;

    bool called = false;

    void operator()()
    {
        called = true;
    }
};

BOOST_AUTO_TEST_CASE(allow_to_use_function_object_pointer)
{
    auto threadPool = createThreadPool(1);
    auto functionObjectPtr = std::make_shared<NonCopyableFunctionObject>();
    BOOST_CHECK(!functionObjectPtr->called);
    TaskFuture future = AddTask(*threadPool, functionObjectPtr);
    future.get();
    BOOST_CHECK(functionObjectPtr->called);
}

BOOST_AUTO_TEST_CASE(destructor_drains_pending_tasks)
{
    std::atomic<int> counter = 0;
    std::vector<TaskFuture> futures;
    {
        ThreadPool threadPool(1);
        for (int i = 0; i < 20; i++)
        {
            futures.push_back(AddTask(threadPool, [&counter]() { counter++; }));
        }
        // Destroying the pool must complete all queued tasks before joining
    }
    for (auto& future: futures)
    {
        BOOST_CHECK_NO_THROW(future.get());
    }
    BOOST_CHECK(counter == 20);
}

BOOST_AUTO_TEST_CASE(worker_survives_task_exception)
{
    auto threadPool = createThreadPool(1);
    TaskFuture failing = AddTask(*threadPool, failingTask<TestException>());
    int counter = 0;
    TaskFuture following = AddTask(*threadPool, [&counter]() { counter++; });
    BOOST_CHECK_THROW(failing.get(), TestException);
    following.get();
    BOOST_CHECK(counter == 1);
}

BOOST_AUTO_TEST_CASE(single_worker_runs_tasks_in_fifo_order)
{
    auto threadPool = createThreadPool(1);
    std::vector<int> order;
    FutureSet futures;
    for (int i = 0; i < 10; i++)
    {
        futures.add(AddTask(*threadPool, [&order, i]() { order.push_back(i); }));
    }
    futures.join();
    for (int i = 0; i < 10; i++)
    {
        BOOST_CHECK(order[i] == i);
    }
}

BOOST_AUTO_TEST_SUITE_END()
