/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE test - concurrency tests
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/concurrency/concurrency.h"

using namespace Yuni::Job;
using namespace Antares::Concurrency;

std::unique_ptr<QueueService> createThreadPool(int size)
{
    auto threadPool = std::make_unique<QueueService>();
    threadPool->maximumThreadCount(size);
    threadPool->start();
    return threadPool;
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

class TestException
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
class TestExceptionN
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

BOOST_AUTO_TEST_SUITE_END()
