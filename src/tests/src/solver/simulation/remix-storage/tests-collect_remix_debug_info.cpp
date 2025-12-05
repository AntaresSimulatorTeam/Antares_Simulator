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

#include <memory>
#include <sstream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"
#include "antares/solver/simulation/remix-storage/storage-for-remix-interface.h"

using namespace Antares::Solver::Simulation;

// Mock storage implementation for testing
class StorageMockForDebugInfo: public IStorageForRemix
{
public:
    StorageMockForDebugInfo(const std::string& name, const std::vector<double>& withdrawal_data):
        IStorageForRemix(name),
        withdrawal_data_(withdrawal_data)
    {
    }

    [[nodiscard]] double maxExchange(unsigned, unsigned) const override
    {
        return 0.0; // Unused in this test
    }

    void update() override
    {
        // Unused in this test
    }

    [[nodiscard]] const std::vector<double>& initWithdrawal() const override
    {
        return withdrawal_data_;
    }

    std::vector<double>& withdrawal() override
    {
        return withdrawal_data_;
    }

private:
    void checkInput(size_t) override
    {
        // Unused in this test
    }

    std::vector<double> withdrawal_data_;
};

BOOST_AUTO_TEST_SUITE(collect_remix_debug_info_tests)

BOOST_AUTO_TEST_CASE(empty_storage_list_produces_empty_output)
{
    ListStorageForRemix storagesForRemix;
    std::stringstream stream;

    collectRemixDebugInfo(storagesForRemix, stream);

    BOOST_CHECK_EQUAL(stream.str(), "");
}

BOOST_AUTO_TEST_CASE(single_storage_with_empty_withdrawal_data)
{
    ListStorageForRemix storagesForRemix;
    std::vector<double> emptyData;
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("storage1", emptyData));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    BOOST_CHECK_EQUAL(stream.str(), "");
}

BOOST_AUTO_TEST_CASE(single_storage_with_one_hour_of_data)
{
    ListStorageForRemix storagesForRemix;
    std::vector data = {100.5};
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("storage1", data));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    BOOST_CHECK_EQUAL(stream.str(), "storage1 0 100.5\n");
}

BOOST_AUTO_TEST_CASE(single_storage_with_multiple_hours_of_data)
{
    ListStorageForRemix storagesForRemix;
    std::vector data = {100.5, 200.75, 50.25};
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("hydro", data));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    std::string expected = "hydro 0 100.5\n"
                           "hydro 1 200.75\n"
                           "hydro 2 50.25\n";
    BOOST_CHECK_EQUAL(stream.str(), expected);
}

BOOST_AUTO_TEST_CASE(multiple_storages_with_different_data)
{
    ListStorageForRemix storagesForRemix;
    std::vector data1 = {10.0, 20.0};
    std::vector data2 = {30.5, 40.5, 50.5};

    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("storage_a", data1));
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("storage_b", data2));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    std::string expected = "storage_a 0 10\n"
                           "storage_a 1 20\n"
                           "storage_b 0 30.5\n"
                           "storage_b 1 40.5\n"
                           "storage_b 2 50.5\n";
    BOOST_CHECK_EQUAL(stream.str(), expected);
}

BOOST_AUTO_TEST_CASE(storage_with_zero_values)
{
    ListStorageForRemix storagesForRemix;
    std::vector data = {0.0, 0.0, 0.0};
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("zero_storage", data));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    std::string expected = "zero_storage 0 0\n"
                           "zero_storage 1 0\n"
                           "zero_storage 2 0\n";
    BOOST_CHECK_EQUAL(stream.str(), expected);
}

BOOST_AUTO_TEST_CASE(storage_with_negative_values)
{
    ListStorageForRemix storagesForRemix;
    std::vector data = {-10.5, 20.0, -5.25};
    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("mixed_storage", data));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    std::string expected = "mixed_storage 0 -10.5\n"
                           "mixed_storage 1 20\n"
                           "mixed_storage 2 -5.25\n";
    BOOST_CHECK_EQUAL(stream.str(), expected);
}

BOOST_AUTO_TEST_CASE(hour_counter_increments_correctly)
{
    ListStorageForRemix storagesForRemix;
    std::vector data(24, 50.0); // 24 hours of data

    storagesForRemix.push_back(std::make_shared<StorageMockForDebugInfo>("day_storage", data));

    std::stringstream stream;
    collectRemixDebugInfo(storagesForRemix, stream);

    std::string output = stream.str();

    // Verify that hour numbers go from 0 to 23
    for (unsigned i = 0; i < 24; ++i)
    {
        BOOST_CHECK(output.find("day_storage " + std::to_string(i)) != std::string::npos);
    }

    // Count the number of lines
    int line_count = std::count(output.begin(), output.end(), '\n');
    BOOST_CHECK_EQUAL(line_count, 24);
}

BOOST_AUTO_TEST_SUITE_END()
