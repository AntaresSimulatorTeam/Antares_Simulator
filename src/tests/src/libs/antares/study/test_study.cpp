/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#define BOOST_TEST_MODULE study
#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include "antares/study/study.h"

using namespace Antares::Data;

/*!
 * Study with one area named "A"
 */
struct OneAreaStudy
{
    OneAreaStudy()
    {
        study = std::make_unique<Study>();
        areaA = study->areaAdd("A");
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
    }

    std::unique_ptr<Study> study;
    Area* areaA;
};

BOOST_AUTO_TEST_SUITE(areas_operations)

BOOST_AUTO_TEST_CASE(area_add)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    BOOST_CHECK(areaA != nullptr);
    BOOST_CHECK_EQUAL(areaA->name, "A");
    BOOST_CHECK_EQUAL(areaA->id, "a");
}

BOOST_FIXTURE_TEST_CASE(area_rename, OneAreaStudy)
{
    BOOST_CHECK(study->areaRename(areaA, "B"));
    BOOST_CHECK(areaA->name == "B");
    BOOST_CHECK(areaA->id == "b");
}

BOOST_FIXTURE_TEST_CASE(area_delete, OneAreaStudy)
{
    BOOST_CHECK_EQUAL(study->areas.size(), 1);
    BOOST_CHECK(study->areaDelete(areaA));
    BOOST_CHECK(study->areas.empty());
}

BOOST_AUTO_TEST_SUITE_END() // areas

// Check that disabled objects are indeed removed from computations
BOOST_AUTO_TEST_SUITE(remove_disabled)

BOOST_FIXTURE_TEST_CASE(thermal_cluster_delete, OneAreaStudy)
{
    auto disabledCluster = std::make_shared<ThermalCluster>(areaA);
    disabledCluster->setName("Cluster1");
    disabledCluster->enabled = false;
    auto enabledCluster = std::make_shared<ThermalCluster>(areaA);
    enabledCluster->setName("Cluster2");
    enabledCluster->enabled = true;

    areaA->thermal.list.addToCompleteList(disabledCluster);
    areaA->thermal.list.addToCompleteList(enabledCluster);

    // Check that "Cluster1" isn't found
    for (const auto& c: areaA->thermal.list.each_enabled())
    {
        BOOST_CHECK(c->name() != "Cluster1");
    }
}

BOOST_FIXTURE_TEST_CASE(renewable_cluster_delete, OneAreaStudy)
{
    auto disabledCluster = std::make_shared<RenewableCluster>(areaA);
    disabledCluster->setName("Cluster1");
    disabledCluster->enabled = false;
    auto enabledCluster = std::make_shared<RenewableCluster>(areaA);
    enabledCluster->setName("Cluster2");
    enabledCluster->enabled = true;

    areaA->renewable.list.addToCompleteList(disabledCluster);
    areaA->renewable.list.addToCompleteList(enabledCluster);

    // Check that "Cluster1" isn't found
    for (const auto& c: areaA->renewable.list.each_enabled())
    {
        BOOST_CHECK(c->name() != "Cluster1");
    }
}

BOOST_FIXTURE_TEST_CASE(short_term_storage_delete, OneAreaStudy)
{
    auto& sts = areaA->shortTermStorage.storagesByIndex;

    auto addSTS = [&sts](std::string&& name, bool enabled)
    {
        ShortTermStorage::STStorageCluster cluster;
        cluster.properties.name = name;
        cluster.properties.enabled = enabled;
        sts.push_back(cluster);
    };

    addSTS("Cluster1", true);
    addSTS("Cluster2", false);

    auto findDisabledCluster = [&sts](std::string&& name)
    {
        return std::find_if(sts.begin(),
                            sts.end(),
                            [&name](ShortTermStorage::STStorageCluster& c)
                            { return c.properties.name == name; });
    };

    // Check that "Cluster1" and "Cluster2" are found
    BOOST_CHECK(findDisabledCluster("Cluster1") != sts.end());
    BOOST_CHECK(findDisabledCluster("Cluster2") != sts.end());

    study->initializeRuntimeInfos();

    // Check that only "Cluster1" is found
    BOOST_CHECK(findDisabledCluster("Cluster1") != sts.end());
    BOOST_CHECK(findDisabledCluster("Cluster2") == sts.end());

    // operator<< doesn't exist for iterators, Boost can't generate output in case of failure, so we
    // use BOOST_CHECK instead of BOOST_CHECK_EQUAL
}

BOOST_AUTO_TEST_SUITE_END() // remove_disabled

BOOST_AUTO_TEST_SUITE(thermal_clusters_operations)

BOOST_FIXTURE_TEST_CASE(thermal_cluster_add, OneAreaStudy)
{
    auto newCluster = std::make_shared<ThermalCluster>(areaA);
    newCluster->setName("Cluster");
    BOOST_CHECK_EQUAL(newCluster->name(), "Cluster");
    BOOST_CHECK_EQUAL(newCluster->id(), "cluster");

    areaA->thermal.list.addToCompleteList(newCluster);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster"), newCluster.get());
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("Cluster"), nullptr);
}

/*!
 * Study with:
 *  - one area named "A"
 *  - one thermal cluster named "Cluster"
 */
struct ThermalClusterStudy: public OneAreaStudy
{
    ThermalClusterStudy()
    {
        auto newCluster = std::make_shared<ThermalCluster>(areaA);
        newCluster->setName("Cluster");
        areaA->thermal.list.addToCompleteList(newCluster);
        cluster = newCluster.get();
    }

    ThermalCluster* cluster;
};

BOOST_FIXTURE_TEST_CASE(thermal_cluster_rename, ThermalClusterStudy)
{
    BOOST_CHECK(study->clusterRename(cluster, "Renamed"));
    BOOST_CHECK_EQUAL(cluster->name(), "Renamed");
    BOOST_CHECK_EQUAL(cluster->id(), "renamed");
}

BOOST_FIXTURE_TEST_CASE(thermal_cluster_delete, ThermalClusterStudy)
{
    // gp : remove() only used in GUI (will go away when removing the GUI)
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster"), cluster);
    areaA->thermal.list.remove("cluster");
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster"), nullptr);
    BOOST_CHECK(areaA->thermal.list.empty());
}

// Custom macro
#define BOOST_CHECK_EQUAL_MESSAGE(L, R, M) \
    {                                      \
        BOOST_TEST_INFO(M);                \
        BOOST_CHECK_EQUAL(L, R);           \
    }

BOOST_FIXTURE_TEST_CASE(WithForceNoGenOptionTimeSeriesNotGeneratedForReverseSpinning,
                        ThermalClusterStudy)
{
    cluster->tsGenBehavior = LocalTSGenerationBehavior::forceNoGen;
    cluster->spinning = 0.9; // An arbitrary value != 1.0 is chosen here, otherwise the reverse
                             // calculation of spinning does nothing
    auto& ts = cluster->series.timeSeries;

    ts.resize(1, 8760);
    ts.fill(100);
    cluster->reverseCalculationOfSpinning();

    for (unsigned i = 0; i < ts.width; ++i)
    {
        for (unsigned j = 0; j < ts.height; ++j)
        {
            BOOST_CHECK_EQUAL_MESSAGE(ts[i][j],
                                      100,
                                      "Error at " + std::to_string(i) + ", " + std::to_string(j));
        }
    }
}

#undef BOOST_CHECK_EQUAL_MESSAGE

BOOST_AUTO_TEST_SUITE_END() // thermal clusters

BOOST_AUTO_TEST_SUITE(renewable_clusters_operations)

BOOST_FIXTURE_TEST_CASE(renewable_cluster_add, OneAreaStudy)
{
    auto newCluster = std::make_shared<RenewableCluster>(areaA);
    newCluster->setName("WindCluster");
    BOOST_CHECK(newCluster->name() == "WindCluster");
    BOOST_CHECK(newCluster->id() == "windcluster");

    areaA->renewable.list.addToCompleteList(newCluster);
    BOOST_CHECK(areaA->renewable.list.findInAll("windcluster") == newCluster.get());
    BOOST_CHECK(areaA->renewable.list.findInAll("WindCluster") == nullptr);
}

/*!
 * Study with:
 *  - one area named "A"
 *  - one renewable cluster named "WindCluster"
 */
struct RenewableClusterStudy: public OneAreaStudy
{
    RenewableClusterStudy()
    {
        areaA = study->areaAdd("A");
        auto newCluster = std::make_shared<RenewableCluster>(areaA);
        newCluster->setName("WindCluster");
        areaA->renewable.list.addToCompleteList(newCluster);
        cluster = newCluster.get();
    }

    RenewableCluster* cluster;
};

BOOST_FIXTURE_TEST_CASE(renewable_cluster_rename, RenewableClusterStudy)
{
    BOOST_CHECK(study->clusterRename(cluster, "Renamed"));
    BOOST_CHECK(cluster->name() == "Renamed");
    BOOST_CHECK(cluster->id() == "renamed");
}

BOOST_FIXTURE_TEST_CASE(renewable_cluster_delete, RenewableClusterStudy)
{
    // gp : remove() only used in GUI (will go away when removing the GUI)
    BOOST_CHECK(areaA->renewable.list.findInAll("windcluster") == cluster);
    BOOST_CHECK(areaA->renewable.list.remove("windcluster"));
    BOOST_CHECK(areaA->renewable.list.findInAll("windcluster") == nullptr);
    BOOST_CHECK(areaA->renewable.list.empty());
}

BOOST_AUTO_TEST_SUITE_END() // renewable clusters

BOOST_AUTO_TEST_SUITE(studyVersion_class)

BOOST_AUTO_TEST_CASE(version_comparison)
{
    StudyVersion v1(7, 2), v2(8, 0), v3;
    v3.fromString("8.0");
    BOOST_CHECK(v1 < v2);
    BOOST_CHECK(!(v1 > v2));
    BOOST_CHECK(v1 != v2);
    BOOST_CHECK(v2 == v3);
    BOOST_CHECK(StudyVersion(12, 3) > StudyVersion(1, 23));

    BOOST_CHECK_EQUAL(StudyVersion(7, 5).toString(), "7.5");
}

BOOST_AUTO_TEST_CASE(version_parsing)
{
    StudyVersion v;
    v.fromString("7.2");
    BOOST_CHECK(v == StudyVersion(7, 2));
    BOOST_CHECK(!v.fromString("abc"));
    BOOST_CHECK(v == StudyVersion::unknown());
    BOOST_CHECK(!v.fromString("a8.7"));
    BOOST_CHECK(!v.fromString("8.b7"));

    // legacy format
    BOOST_CHECK(!v.fromString("8a60"));
    BOOST_CHECK(!v.fromString("a860"));
    v.fromString("860");
    BOOST_CHECK(v == StudyVersion(8, 6));

    v.fromString("8.8");
    BOOST_CHECK(v == StudyVersion(8, 8));
    BOOST_CHECK(!v.fromString("8..6"));

    // 4.5 is not in the list of supported versions, thus failing
    BOOST_CHECK(!v.fromString("4.5"));
    BOOST_CHECK(v == StudyVersion::unknown());
}

BOOST_FIXTURE_TEST_CASE(check_filename_limit, OneAreaStudy)
{
    auto s = std::make_unique<Study>();
    BOOST_CHECK(s->checkForFilenameLimits(true)); // empty areas should return true

    BOOST_CHECK(study->checkForFilenameLimits(true));
    BOOST_CHECK(study->checkForFilenameLimits(false));
    BOOST_CHECK(study->checkForFilenameLimits(true, "abc"));

#ifdef YUNI_OS_WINDOWS
    std::string area1name(128, 'a');
    std::string area2name(128, 'b');
    auto areaB = study->areaAdd(area1name);
    auto areaC = study->areaAdd(area2name);
    AreaAddLinkBetweenAreas(areaB, areaC);
    BOOST_CHECK(!study->checkForFilenameLimits(true));
#endif
}

BOOST_FIXTURE_TEST_CASE(cpu_count, OneAreaStudy)
{
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(75, ncMin), 1);
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(10, ncLow), 3);
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(6, ncAvg), 3);
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(16, ncHigh), 12);
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(128, ncMax), 128);

    // error cases
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(0, ncMax), 0);
    BOOST_CHECK_EQUAL(study->getNumberOfCoresPerMode(10, 120), 0);
}

BOOST_AUTO_TEST_SUITE_END() // version
