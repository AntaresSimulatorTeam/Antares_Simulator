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

    // Clear and re-add for west area with new coefficient
    AreaName newName = "new_west_area";
    alloc.fromArea(*west, 0.0);
    alloc.fromArea(newName, 0.7);
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

BOOST_FIXTURE_TEST_CASE(remove_erases_existing_coefficient, AllocationFixture)
{
    HydroAllocation alloc;
    alloc.fromArea(*east, 0.5);
    BOOST_CHECK_CLOSE(alloc.fromArea(*east), 0.5, 1e-12);

    alloc.remove(east->id);
    BOOST_CHECK_EQUAL(alloc.fromArea(*east), 0.0);

    // removing an id that was never present is a safe no-op
    alloc.remove("does_not_exist");
}

BOOST_FIXTURE_TEST_CASE(fromArea_pointer_overloads, AllocationFixture)
{
    HydroAllocation alloc;

    // getter/setter with a null pointer are safe no-ops
    BOOST_CHECK_EQUAL(alloc.fromArea(static_cast<const Area*>(nullptr)), 0.0);
    alloc.fromArea(static_cast<const Area*>(nullptr), 5.0);
    BOOST_CHECK(alloc.coefficients().empty());

    const Area* eastPtr = east;
    alloc.fromArea(eastPtr, 0.42);
    BOOST_CHECK_CLOSE(alloc.fromArea(eastPtr), 0.42, 1e-12);
}

BOOST_FIXTURE_TEST_CASE(operator_bracket_by_areaname, AllocationFixture)
{
    HydroAllocation alloc;
    BOOST_CHECK_EQUAL(alloc[east->id], 0.0);

    alloc.fromArea(*east, 0.33);
    BOOST_CHECK_CLOSE(alloc[east->id], 0.33, 1e-12);
}

BOOST_FIXTURE_TEST_CASE(clear_removes_all_coefficients, AllocationFixture)
{
    HydroAllocation alloc;
    alloc.fromArea(*east, 0.5);
    alloc.fromArea(*west, 0.5);
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
    BOOST_CHECK_CLOSE(alloc.fromArea("east"), 1.0, 1e-12);
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

    BOOST_CHECK_CLOSE(alloc.fromArea("east"), 0.6, 1e-12); // key lowercased before storing
    BOOST_CHECK_EQUAL(alloc.fromArea("west"), 0.0); // zero coefficient is skipped, not stored
    BOOST_CHECK_CLOSE(alloc.fromArea("some_area"), 0.9, 1e-12);
    BOOST_CHECK_EQUAL(alloc.coefficients().size(), 2u);
}

BOOST_FIXTURE_TEST_CASE(saveToFile_empty_creates_empty_file, AllocationFixture)
{
    HydroAllocation alloc;
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto path = dir / "out.ini";

    BOOST_CHECK(alloc.saveToFile(path.string()));
    BOOST_CHECK(fs::exists(path));
    BOOST_CHECK_EQUAL(fs::file_size(path), 0u);
}

BOOST_FIXTURE_TEST_CASE(saveToFile_then_loadFromFile_roundtrip, AllocationFixture)
{
    HydroAllocation alloc;
    alloc.fromArea(*east, 0.5);
    alloc.fromArea(*west, 1.0);

    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto path = dir / "out.ini";
    BOOST_CHECK(alloc.saveToFile(path.string()));

    HydroAllocation reloaded;
    BOOST_CHECK(reloaded.loadFromFile("east", path));
    BOOST_CHECK_CLOSE(reloaded.fromArea(*east), 0.5, 1e-9);
    BOOST_CHECK_CLOSE(reloaded.fromArea(*west), 1.0, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(copyFrom_copies_matching_areas, AllocationFixture)
{
    // A separate "source" study, with its own east/west areas
    auto sourceStudy = std::make_unique<Study>();
    auto srcEast = new Area("east"); // freed by ~AreaList
    sourceStudy->areas.add(srcEast);
    auto srcWest = new Area("west"); // freed by ~AreaList
    sourceStudy->areas.add(srcWest);

    HydroAllocation source;
    source.fromArea(*srcEast, 0.3);
    source.fromArea(*srcWest, 0.7);

    HydroAllocation dest;
    dest.fromArea(*east, 99.0); // pre-existing value: copyFrom must clear it first

    AreaNameMapping mapping; // empty: FindMappedAreaName falls back to the source area's own id
    dest.copyFrom(source, *sourceStudy, mapping, *study);

    BOOST_CHECK_CLOSE(dest.fromArea(*east), 0.3, 1e-12);
    BOOST_CHECK_CLOSE(dest.fromArea(*west), 0.7, 1e-12);
    BOOST_CHECK_EQUAL(dest.coefficients().size(), 2u);
}

BOOST_FIXTURE_TEST_CASE(copyFrom_skips_unmapped_source_area, AllocationFixture)
{
    auto sourceStudy = std::make_unique<Study>();
    auto srcEast = new Area("east"); // freed by ~AreaList
    sourceStudy->areas.add(srcEast);
    auto srcOrphan = new Area("orphan"); // freed by ~AreaList
    sourceStudy->areas.add(srcOrphan);

    HydroAllocation source;
    source.fromArea(*srcEast, 0.5);
    source.fromArea(*srcOrphan, 0.9); // no counterpart in the destination study

    HydroAllocation dest;
    AreaNameMapping mapping;
    dest.copyFrom(source, *sourceStudy, mapping, *study);

    BOOST_CHECK_CLOSE(dest.fromArea(*east), 0.5, 1e-12);
    BOOST_CHECK_EQUAL(dest.coefficients().size(), 1u); // orphan silently skipped
}

BOOST_FIXTURE_TEST_CASE(copyFrom_uses_mapping_to_rename_areas, AllocationFixture)
{
    auto sourceStudy = std::make_unique<Study>();
    auto srcNorth = new Area("north"); // freed by ~AreaList
    sourceStudy->areas.add(srcNorth);

    HydroAllocation source;
    source.fromArea(*srcNorth, 0.4);

    HydroAllocation dest;
    AreaNameMapping mapping;
    mapping["north"] = "east"; // FindMappedAreaName is keyed by the source area's own name
    dest.copyFrom(source, *sourceStudy, mapping, *study);

    BOOST_CHECK_CLOSE(dest.fromArea(*east), 0.4, 1e-12);
    BOOST_CHECK_EQUAL(dest.coefficients().size(), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
