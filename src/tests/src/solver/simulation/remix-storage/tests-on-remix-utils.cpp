#define BOOST_TEST_MODULE remix utils

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"
#include "antares/solver/simulation/remix-storage/storage-for-remix.h"

using namespace Antares::Solver::Simulation;

class StorageMock: public IStorageForRemix
{
public:
    StorageMock(int id);
    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) override;
    void update() override;
    const std::vector<double>& initWithdrawal() override;
    std::vector<double>& withdrawal() override;

    int id();

private:
    void checkInput(size_t size) override;

    int id_ = 0;
    std::vector<double> dummy_;
};

StorageMock::StorageMock(int id):
    id_(id)
{
}

int StorageMock::id()
{
    return id_;
}

double StorageMock::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen)
{
    return 0; // Dummy : unused
}

void StorageMock::update()
{
}

const std::vector<double>& StorageMock::initWithdrawal()
{
    return dummy_;
}

std::vector<double>& StorageMock::withdrawal()
{
    return dummy_;
}

void StorageMock::checkInput(size_t size)
{
}

struct SortingStorageFixture
{
    StorageListSort storage_sort;
};

BOOST_FIXTURE_TEST_SUITE(storage_list_sorting, SortingStorageFixture)

BOOST_AUTO_TEST_CASE(only_one_storage_added_to_list___sorting_operation_is_consistent)
{
    storage_sort.add(2., std::make_shared<StorageMock>(2));

    auto listSortedStorage = storage_sort.makeSortedList();

    BOOST_CHECK_EQUAL(listSortedStorage.size(), 1);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[0])->id(), 2.);
}

BOOST_AUTO_TEST_CASE(five_storage_added_to_list___storage_sorted_depending_on_associated_capa)
{
    storage_sort.add(2., std::make_shared<StorageMock>(2));
    storage_sort.add(5., std::make_shared<StorageMock>(5));
    storage_sort.add(1., std::make_shared<StorageMock>(1));
    storage_sort.add(3., std::make_shared<StorageMock>(3));
    storage_sort.add(4., std::make_shared<StorageMock>(4));

    auto listSortedStorage = storage_sort.makeSortedList();

    BOOST_CHECK_EQUAL(listSortedStorage.size(), 5);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[0])->id(), 5.);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[1])->id(), 4.);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[2])->id(), 3.);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[3])->id(), 2.);
    BOOST_CHECK_EQUAL(std::static_pointer_cast<StorageMock>(listSortedStorage[4])->id(), 1.);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(utils)

BOOST_AUTO_TEST_CASE(between_hour_1_and_4___vector_min_is_2)
{
    BOOST_CHECK_EQUAL(min_on_subrange({1, 2, 3, 4, 5}, 1, 4), 2.);
}

BOOST_AUTO_TEST_CASE(between_hour_2_and_4___vector_min_is_half_one)
{
    BOOST_CHECK_EQUAL(min_on_subrange({ 5, 4, 0.6, 1.5, 0.5, 6}, 2, 4), 0.5);
}

BOOST_AUTO_TEST_CASE(hours_h_and_H_are_equals___vector_min_is_defined)
{
    BOOST_CHECK_EQUAL(min_on_subrange({1., 2., 3.}, 2, 2), 3.);
}

BOOST_AUTO_TEST_CASE(vector_is_empty___exception_raised)
{
    std::string err_msg = "call min_on_subrange on an empty vector";
    BOOST_CHECK_EXCEPTION(min_on_subrange({}, 2, 7), std::invalid_argument, checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_h_greater_than_H___exception_raised)
{
    std::string err_msg = "call min_on_subrange with inconsistant hours";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1., 2.}, 1, 0),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_h_negative___exception_raised)
{
    std::string err_msg = "call of min_on_subrange : hour out of bound";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1., 2.}, -1, 1),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_H_too_large___exception_raised)
{
    std::string err_msg = "call of min_on_subrange : hour out of bound";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1., 2.}, -1, 2),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()
