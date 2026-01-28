/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 */

#define BOOST_TEST_MODULE test hydro allocation

#include <memory>

#include <boost/test/unit_test.hpp>

#include <antares/study/parts/hydro/allocation.h>
#include <antares/study/study.h>

using namespace Antares::Data;

struct AllocationFixture
{
    std::unique_ptr<Study> study;
    Area* east{nullptr};
    Area* west{nullptr};

    AllocationFixture():
        study(std::make_unique<Study>(false))
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

BOOST_FIXTURE_TEST_CASE(basic_set_get_remove_rename, AllocationFixture)
{
    HydroAllocation alloc;

    // Initially zero
    BOOST_CHECK_EQUAL(alloc.fromArea(*east), 0.0);
    BOOST_CHECK_EQUAL(alloc.fromArea(*west), 0.0);

    // Set values
    alloc.fromArea(*east, 0.3);
    alloc.fromArea(*west, 0.7);

    BOOST_CHECK_CLOSE(alloc.fromArea(*east), 0.3, 1e-12);
    BOOST_CHECK_CLOSE(alloc.fromArea(*west), 0.7, 1e-12);
    BOOST_CHECK_CLOSE(alloc[*east], 0.3, 1e-12);
    BOOST_CHECK_CLOSE(alloc[*west], 0.7, 1e-12);

    // Remove one value
    alloc.fromArea(*east, 0.0);
    BOOST_CHECK_EQUAL(alloc.fromArea(*east), 0.0);

    // Rename remaining key
    AreaName newName = west->id;
    newName += "_new";
    alloc.rename(west->id, newName);
    BOOST_CHECK_EQUAL(alloc.fromArea(*west), 0.0);
    BOOST_CHECK_CLOSE(alloc.fromArea(newName), 0.7, 1e-12);
}

BOOST_FIXTURE_TEST_CASE(prepare_and_eachNonNull, AllocationFixture)
{
    HydroAllocation alloc;

    // Fill values by area name
    alloc.fromArea(*east, 0.4);
    alloc.fromArea(*west, 0.6);

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

    alloc.fromArea(*east, 0.25);
    alloc.fromArea(*west, 0.75);
    alloc.prepareForSolver(areas());

    // Mutate after prepare
    alloc.fromArea(*east, 0.5);

    // eachNonNull still reflects the snapshot created at prepare time
    std::map<uint, double> byIndex;
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });

    BOOST_CHECK_CLOSE(byIndex[east->index], 0.25, 1e-12);
    BOOST_CHECK_CLOSE(byIndex[west->index], 0.75, 1e-12);

    // Re-prepare to reflect updated values
    // Note: prepareForSolver rebuilds from pValues; re-add unchanged west value
    alloc.fromArea(*west, 0.75);
    alloc.prepareForSolver(areas());
    byIndex.clear();
    alloc.eachNonNull([&](uint idx, double v) { byIndex[idx] = v; });

    BOOST_CHECK_CLOSE(byIndex[east->index], 0.5, 1e-12);
    BOOST_CHECK_CLOSE(byIndex[west->index], 0.75, 1e-12);
}

BOOST_AUTO_TEST_SUITE_END()
