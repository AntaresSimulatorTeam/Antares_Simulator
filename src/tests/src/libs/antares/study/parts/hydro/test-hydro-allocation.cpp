/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 */

#define BOOST_TEST_MODULE test hydro allocation

#include <files-system.h>
#include <fstream>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <antares/study/parts/hydro/allocation.h>
#include <antares/study/study.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

struct AllocationFixture
{
    std::unique_ptr<Study> study;
    Area* east{nullptr};
    Area* west{nullptr};

    AllocationFixture():
        study(std::make_unique<Study>())
    {
        east = new Area("east"); // freed by ~AreaList
        west = new Area("west"); // freed by ~AreaList
        study->areas.add(east);
        study->areas.add(west);
        study->areas.rebuildIndexes();
    }

    AreaList& areas()
    {
        return study->areas;
    }
};

BOOST_AUTO_TEST_SUITE(hydro_allocation)

BOOST_FIXTURE_TEST_CASE(fromArea_sets_removes_and_clears_coefficients, AllocationFixture)
{
    HydroAllocation alloc;

    // Set values
    alloc.fromArea(east->id, 0.3);
    alloc.fromArea(west->id, 0.7);
    BOOST_CHECK_CLOSE(alloc.coefficients().at(east->id), 0.3, 1e-12);
    BOOST_CHECK_CLOSE(alloc.coefficients().at(west->id), 0.7, 1e-12);

    // Setting a zero coefficient erases the entry
    alloc.fromArea(east->id, 0.0);
    BOOST_CHECK_EQUAL(alloc.coefficients().count(east->id), 0u);
    BOOST_CHECK_EQUAL(alloc.coefficients().size(), 1u);

    // clear() drops everything
    alloc.clear();
    BOOST_CHECK(alloc.coefficients().empty());
}

BOOST_FIXTURE_TEST_CASE(prepare_and_eachNonNull, AllocationFixture)
{
    HydroAllocation alloc;

    // Fill values by area name
    alloc.fromArea(east->id, 0.4);
    alloc.fromArea(west->id, 0.6);

    // Prepare mapping by index
    alloc.prepareForSolver(areas());

    // After prepareForSolver, coefficients() should be empty
    BOOST_CHECK(alloc.coefficients().empty());

    // eachNonNull should iterate entries by area index
    std::map<uint, double> byIndex;
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });

    BOOST_REQUIRE_EQUAL(byIndex.size(), 2u);
    BOOST_CHECK_CLOSE(byIndex[east->index], 0.4, 1e-12);
    BOOST_CHECK_CLOSE(byIndex[west->index], 0.6, 1e-12);
}

BOOST_FIXTURE_TEST_CASE(post_prepare_mutation_requires_reprepare, AllocationFixture)
{
    HydroAllocation alloc;

    alloc.fromArea(east->id, 0.25);
    alloc.fromArea(west->id, 0.75);
    alloc.prepareForSolver(areas());

    // Mutate after prepare
    alloc.fromArea(east->id, 0.5);

    // eachNonNull still reflects the snapshot created at prepare time
    std::map<uint, double> byIndex;
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });

    BOOST_CHECK_CLOSE(byIndex[east->index], 0.25, 1e-12);
    BOOST_CHECK_CLOSE(byIndex[west->index], 0.75, 1e-12);

    // Re-prepare to reflect updated values
    // Note: prepareForSolver rebuilds from pValues; re-add unchanged west value
    alloc.fromArea(west->id, 0.75);
    alloc.prepareForSolver(areas());
    byIndex.clear();
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });

    BOOST_CHECK_CLOSE(byIndex[east->index], 0.5, 1e-12);
    BOOST_CHECK_CLOSE(byIndex[west->index], 0.75, 1e-12);
}

BOOST_FIXTURE_TEST_CASE(clear_removes_all_coefficients, AllocationFixture)
{
    HydroAllocation alloc;
    alloc.fromArea(east->id, 0.5);
    alloc.fromArea(west->id, 0.5);
    alloc.prepareForSolver(areas());

    alloc.clear();

    BOOST_CHECK(alloc.coefficients().empty());
    std::map<uint, double> byIndex;
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });
    BOOST_CHECK(byIndex.empty());
}

BOOST_FIXTURE_TEST_CASE(loadFromFile_missing_file_falls_back_to_full_allocation, AllocationFixture)
{
    HydroAllocation alloc;
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto missingPath = dir / "does_not_exist.ini";

    BOOST_CHECK(alloc.loadFromFile("east", missingPath));
    BOOST_CHECK_CLOSE(alloc.coefficients().at("east"), 1.0, 1e-12);
    BOOST_CHECK_EQUAL(alloc.coefficients().size(), 1u);
}

BOOST_FIXTURE_TEST_CASE(loadFromFile_empty_file_yields_no_coefficients, AllocationFixture)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto path = dir / "allocation.ini";
    std::ofstream(path).close(); // empty file

    HydroAllocation alloc;
    BOOST_CHECK(alloc.loadFromFile("east", path));
    BOOST_CHECK(alloc.coefficients().empty());
}

BOOST_FIXTURE_TEST_CASE(loadFromFile_parses_nonzero_lowercased_coefficients, AllocationFixture)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto path = dir / "allocation.ini";
    std::ofstream file(path);
    file << "[allocation]\nEast = 0.6\nwest = 0\nsome_area = 0.9\n";
    file.close();

    HydroAllocation alloc;
    BOOST_CHECK(alloc.loadFromFile("east", path));

    BOOST_CHECK_CLOSE(alloc.coefficients().at("east"), 0.6, 1e-12); // key lowercased before storing
    BOOST_CHECK_EQUAL(alloc.coefficients().count("west"), 0u);      // zero coefficient is skipped
    BOOST_CHECK_CLOSE(alloc.coefficients().at("some_area"), 0.9, 1e-12);
    BOOST_CHECK_EQUAL(alloc.coefficients().size(), 2u);
}

BOOST_AUTO_TEST_SUITE_END()
