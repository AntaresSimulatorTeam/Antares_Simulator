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
    const std::vector<double>& initialGen() override;
    std::vector<double>& generation() override;

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

const std::vector<double>& StorageMock::initialGen()
{
    return dummy_;
}

std::vector<double>& StorageMock::generation()
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
