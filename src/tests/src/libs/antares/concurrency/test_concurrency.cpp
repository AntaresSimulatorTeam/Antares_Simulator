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
#define BOOST_TEST_MODULE test-concurrency tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

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
    Task incrementCounter = [&counter]() {
        counter++;
    };
    TaskFuture future = AddTask(*threadPool, incrementCounter);
    future.get();
    BOOST_CHECK(counter == 1);
}


template <class Exc>
Task failingTask() {
    return []() {
        throw Exc();
    };
}

class TestException {};

BOOST_AUTO_TEST_CASE(test_throw)
{
    auto threadPool = createThreadPool(1);
    TaskFuture future = AddTask(*threadPool, failingTask<TestException>());
    BOOST_CHECK_THROW(future.get(), TestException);
}

BOOST_AUTO_TEST_CASE(test_future_set)
{
    auto threadPool = createThreadPool(4);
    int counter = 0;
    Task incrementCounter = [&counter]() {
        counter++;
    };
    FutureSet futures;
    for (int i = 0; i < 10; i++) {
        futures.add(AddTask(*threadPool, incrementCounter));
    }
    futures.join();
    BOOST_CHECK(counter == 10);
}

template <int N>
class TestExceptionN {};

BOOST_AUTO_TEST_CASE(test_future_set_rethrows_first_submitted)
{
    auto threadPool = createThreadPool(2);
    FutureSet futures;
    futures.add(AddTask(*threadPool, failingTask<TestExceptionN<1>>()));
    futures.add(AddTask(*threadPool, failingTask<TestExceptionN<2>>()));
    BOOST_CHECK_THROW(futures.join(), TestExceptionN<1>);
}
