/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#define BOOST_TEST_MODULE remix utils

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/storage-for-remix-interface.h"

using namespace Antares::Solver::Simulation;

class StorageMock: public IStorageForRemix
{
public:
    StorageMock(int id);
    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void update() override;
    const std::vector<double>& initWithdrawal() const override;
    std::vector<double>& withdrawal() override;
    int id();

private:
    void checkInput(size_t size) override;

    int id_ = 0;
    std::vector<double> dummy_;
};

StorageMock::StorageMock(int id):
    IStorageForRemix("whatever name"),
    id_(id)
{
}

int StorageMock::id()
{
    return id_;
}

double StorageMock::maxExchange(unsigned, unsigned) const
{
    return 0; // Dummy : unused
}

void StorageMock::update()
{
}

const std::vector<double>& StorageMock::initWithdrawal() const
{
    return dummy_;
}

std::vector<double>& StorageMock::withdrawal()
{
    return dummy_;
}

void StorageMock::checkInput(size_t)
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

BOOST_AUTO_TEST_SUITE(min_on_subrange_unit_tests)

BOOST_AUTO_TEST_CASE(when_range_increases_min_on_subrange_takes_first_element_of_subrange)
{
    BOOST_CHECK_EQUAL(min_on_subrange({1, 2, 3, 4, 5}, 1, 4), 2);
}

BOOST_AUTO_TEST_CASE(when_range_decreases_then_min_on_subrange_excludes_last_element_of_range)
{
    BOOST_CHECK_EQUAL(min_on_subrange({5, 4, 3, 2, 1}, 0, 2), 4);
}

BOOST_AUTO_TEST_CASE(min_on_subrange_exclude_last_element_of_range)
{
    BOOST_CHECK_EQUAL(min_on_subrange({5, 4, 0.6, 1.5, 0.5, 6}, 2, 4), 0.6);
}

BOOST_AUTO_TEST_CASE(vector_is_empty___exception_raised)
{
    std::string err_msg = "call min_on_subrange on an empty vector";
    BOOST_CHECK_EXCEPTION(min_on_subrange({}, 2, 7), std::invalid_argument, checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hours_h_and_H_are_equals___vector_min_is_not_defined)
{
    std::string err_msg = "call min_on_subrange with inconsistant hours";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1, 2, 3}, 2, 2),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_h_greater_than_H___exception_raised)
{
    std::string err_msg = "call min_on_subrange with inconsistant hours";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1, 2}, 1, 0),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_h_negative___exception_raised)
{
    std::string err_msg = "call of min_on_subrange : hour out of bound";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1, 2}, -1, 1),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(hour_H_too_large___exception_raised)
{
    std::string err_msg = "call of min_on_subrange : hour out of bound";
    BOOST_CHECK_EXCEPTION(min_on_subrange({1, 2}, -1, 2),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cyclic_iterator_unit_tests)

BOOST_AUTO_TEST_CASE(when_cyclic_iterator_points_to_empty_container)
{
    std::vector<int> v; // Empty
    CyclicIterator<int> cyclic_it(v);
    BOOST_CHECK(cyclic_it == v.begin());
}

BOOST_AUTO_TEST_CASE(at_construction__cyclic_iterator_points_to_first_element)
{
    std::vector<int> v = {1, 2, 3};
    CyclicIterator<int> cyclic_it(v);
    BOOST_CHECK_EQUAL(*cyclic_it, v[0]);
}

BOOST_AUTO_TEST_CASE(calling_star_operator_allows_changing_underlying_element)
{
    std::vector<int> v = {1, 2, 3};
    CyclicIterator<int> cyclic_it(v);
    *cyclic_it = 7;
    BOOST_CHECK_EQUAL(v[0], 7);
}

BOOST_AUTO_TEST_CASE(incrementing_iterator_makes_it_point_on_next_element)
{
    std::vector<int> v = {1, 2, 3};
    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    BOOST_CHECK_EQUAL(*cyclic_it, v[1]);
}

BOOST_AUTO_TEST_CASE(incrementing_iterator_enough_times_makes_it_point_back_to_vector_begin)
{
    std::vector<int> v = {1, 2};
    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++;

    BOOST_CHECK_EQUAL(*cyclic_it, v[0]);
}

BOOST_AUTO_TEST_CASE(incrementing_as_much_cyclic_than_classic_iterators_makes_them_equal)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7};

    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++;
    cyclic_it++;
    cyclic_it++;

    std::vector<int>::iterator classic_it = v.begin();
    classic_it++;
    classic_it++;
    classic_it++;
    classic_it++;

    BOOST_CHECK(cyclic_it == classic_it);
}

BOOST_AUTO_TEST_CASE(incrementing_cyclic_and_classic_iterators_differnetly_makes_them_different)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6};

    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++;
    cyclic_it++;

    std::vector<int>::iterator classic_it = v.begin();
    classic_it++;
    classic_it++;

    BOOST_CHECK(cyclic_it != classic_it);
}

BOOST_AUTO_TEST_CASE(calling_delete_current_on_fresh_iterator_deletes_the_first_element)
{
    std::vector<int> v = {1, 2, 3};
    CyclicIterator<int> cyclic_it(v);
    delete_current<int>(cyclic_it);

    std::vector<int> expected = {2, 3};
    BOOST_CHECK(v == expected);
    BOOST_CHECK_EQUAL(*cyclic_it, 2);
}

BOOST_AUTO_TEST_CASE(when_calling_delete_current_on_last_element)
{
    std::vector<int> v = {1, 2, 3};
    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++; // Now cyclic iterator point at last element
    delete_current<int>(cyclic_it);

    std::vector<int> expected = {1, 2};
    BOOST_CHECK(v == expected);
    BOOST_CHECK_EQUAL(*cyclic_it, v[0]);
}

BOOST_AUTO_TEST_CASE(deleting_an_element_in_the_middle_of_the_vector)
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++;
    delete_current<int>(cyclic_it);

    std::vector<int> expected = {1, 2, 4, 5};
    BOOST_CHECK(v == expected);
    BOOST_CHECK_EQUAL(*cyclic_it, 4);
}

BOOST_AUTO_TEST_CASE(deleting_an_element_with_iterator_keeps_this_iterator_valid)
{
    std::vector<int> v = {1, 2, 3, 4};
    CyclicIterator<int> cyclic_it(v);
    cyclic_it++;
    cyclic_it++;
    delete_current<int>(cyclic_it);

    // Now v = {1, 2, 4} and cyclic_it points to 4
    *cyclic_it = 7;

    std::vector<int> expected = {1, 2, 7};
    BOOST_CHECK(v == expected);
}

BOOST_AUTO_TEST_SUITE_END()
