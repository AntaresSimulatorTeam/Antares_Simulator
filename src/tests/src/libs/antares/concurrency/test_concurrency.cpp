//
// Created by leclercsyl on 06/10/23.
//
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

class TestException1 {};
class TestException2 {};

BOOST_AUTO_TEST_CASE(test_future_set_rethrows_first)
{
    auto threadPool = createThreadPool(2);
    FutureSet futures;
    futures.add(AddTask(*threadPool, failingTask<TestException1>()));
    futures.add(AddTask(*threadPool, failingTask<TestException2>()));
    BOOST_CHECK_THROW(futures.join(), TestException1);
}
