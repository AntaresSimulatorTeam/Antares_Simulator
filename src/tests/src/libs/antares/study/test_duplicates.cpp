// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#define WIN32_LEAN_AND_MEAN
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/study/duplicates.h>
#include <antares/study/study.h>

using Antares::logs;
using Antares::Check::checkForDuplicates;
using Antares::Data::RenewableCluster;
using Antares::Data::Study;
using Antares::Data::ThermalCluster;
using namespace Antares::Data::ShortTermStorage;

BOOST_AUTO_TEST_SUITE(study_duplicates)

struct DuplicateFixture: public Antares::UnitTests::CaptureAntaresLogs
{
    std::unique_ptr<Study> study = std::make_unique<Study>();
    Data::Area* areaA = addAreaToListOfAreas(study->areas, "A");
    Data::Area* areaB = addAreaToListOfAreas(study->areas, "B");

    void addBindingConstraint(const std::string& name)
    {
        study->bindingConstraints.add(name);
    }
};

void addThermalCluster(Data::Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

void addRenewableCluster(Data::Area* area, const std::string& name)
{
    auto c = std::make_shared<RenewableCluster>(area);
    c->setName(name);
    area->renewable.list.addToCompleteList(c);
}

void addShortTermStorage(Data::Area* area, const std::string& name)
{
    STStorageCluster cluster;
    cluster.properties.name = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

BOOST_FIXTURE_TEST_CASE(empty_study, DuplicateFixture)
{
    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE(getErrors().empty());
}

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaA, "cluster");
    addThermalCluster(areaA, "cluster");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 1);
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaA, "cluster");
    addThermalCluster(areaA, "cluster");

    addThermalCluster(areaA, "cluster_2");
    addThermalCluster(areaA, "cluster_2");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 2); // Stops after first error
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaA, "cluster_1");
    addThermalCluster(areaA, "cluster_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE(getErrors().empty());
}

BOOST_FIXTURE_TEST_CASE(two_areas_two_duplicate_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaB, "cluster");
    addThermalCluster(areaB, "cluster");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 1);
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster` found in area `b`"));
}

BOOST_FIXTURE_TEST_CASE(two_areas_two_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaA, "cluster_1");
    addThermalCluster(areaA, "cluster_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(two_areas_four_thermal_clusters, DuplicateFixture)
{
    addThermalCluster(areaA, "cluster_1");
    addThermalCluster(areaA, "cluster_2");
    addThermalCluster(areaB, "cluster_1");
    addThermalCluster(areaB, "cluster_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(two_areas_eight_duplicates_thermal_clusters, DuplicateFixture)
{
    for (auto* area: {areaA, areaB})
    {
        addThermalCluster(area, "cluster_1");
        addThermalCluster(area, "cluster_2");
        addThermalCluster(area, "cluster_1");
        addThermalCluster(area, "cluster_2");
    }

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 4);
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster_1` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster_2` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster_1` found in area `b`"));
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `cluster_2` found in area `b`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_renewable_clusters, DuplicateFixture)
{
    addRenewableCluster(areaA, "cluster");
    addRenewableCluster(areaA, "cluster");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 1);
    BOOST_CHECK(getErrors().contains("Duplicate renewable cluster `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_renewable_clusters, DuplicateFixture)
{
    addRenewableCluster(areaA, "cluster");
    addRenewableCluster(areaA, "cluster");

    addRenewableCluster(areaA, "cluster_2");
    addRenewableCluster(areaA, "cluster_2");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 2);
    BOOST_CHECK(getErrors().contains("Duplicate renewable cluster `cluster` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate renewable cluster `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_renewable_clusters, DuplicateFixture)
{
    addRenewableCluster(areaA, "cluster_1");
    addRenewableCluster(areaA, "cluster_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_STS_clusters, DuplicateFixture)
{
    addShortTermStorage(areaA, "cluster");
    addShortTermStorage(areaA, "cluster");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 1);
    BOOST_CHECK(getErrors().contains("Duplicate short term storage `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_STS_clusters, DuplicateFixture)
{
    addShortTermStorage(areaA, "cluster");
    addShortTermStorage(areaA, "cluster");

    addShortTermStorage(areaA, "cluster_2");
    addShortTermStorage(areaA, "cluster_2");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 2);
    BOOST_CHECK(getErrors().contains("Duplicate short term storage `cluster` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate short term storage `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_STS_clusters, DuplicateFixture)
{
    addShortTermStorage(areaA, "cluster_1");
    addShortTermStorage(areaA, "cluster_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE(getErrors().empty());
}

BOOST_FIXTURE_TEST_CASE(detection_of_duplicate_constraints, DuplicateFixture)
{
    addBindingConstraint("dummy");
    addBindingConstraint("dummy");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 1);
    BOOST_CHECK(getErrors().contains("Duplicate binding constraint `dummy` found in study"));
}

BOOST_FIXTURE_TEST_CASE(detection_of_more_duplicate_constraints, DuplicateFixture)
{
    addBindingConstraint("dummy");
    addBindingConstraint("dummy");

    addBindingConstraint("dummy_2");
    addBindingConstraint("dummy_2");

    BOOST_CHECK(!checkForDuplicates(*study));
    BOOST_REQUIRE_EQUAL(getErrors().size(), 2);
    BOOST_CHECK(getErrors().contains("Duplicate binding constraint `dummy` found in study"));
    BOOST_CHECK(getErrors().contains("Duplicate binding constraint `dummy_2` found in study"));
}

BOOST_FIXTURE_TEST_CASE(detection_of_non_duplicate_constraints, DuplicateFixture)
{
    addBindingConstraint("dummy_1");
    addBindingConstraint("dummy_2");

    BOOST_CHECK(checkForDuplicates(*study));
    BOOST_REQUIRE(getErrors().empty());
}

BOOST_FIXTURE_TEST_CASE(many_duplicates, DuplicateFixture)
{
    for (int idx = 0; idx < 10; idx++)
    {
        addRenewableCluster(areaA, "renewable");
        addThermalCluster(areaB, "thermal");
        addShortTermStorage(areaA, "storage");
        addBindingConstraint("binding_constraint");
    }
    BOOST_CHECK(!checkForDuplicates(*study));
    // Check logs
    BOOST_CHECK_EQUAL(getErrors().size(), 4);
    BOOST_CHECK(getErrors().contains("Duplicate renewable cluster `renewable` found in area `a`"));
    BOOST_CHECK(getErrors().contains("Duplicate thermal cluster `thermal` found in area `b`"));
    BOOST_CHECK(getErrors().contains("Duplicate short term storage `storage` found in area `a`"));
    BOOST_CHECK(
      getErrors().contains("Duplicate binding constraint `binding_constraint` found in study"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(loading_error)

BOOST_AUTO_TEST_CASE(message)
{
    Antares::Error::Duplicates dupl;
    BOOST_CHECK_EQUAL(dupl.what(), "One or more duplicates found.");
}
BOOST_AUTO_TEST_SUITE_END()
