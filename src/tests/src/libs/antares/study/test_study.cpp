/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#define BOOST_TEST_MODULE study
#define BOOST_TEST_DYN_LINK
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
        areaA = study.areaAdd("A");
        study.parameters.simulationDays.first = 0;
        study.parameters.simulationDays.end = 7;
    }

    auto study = std::make_unique<Study>();
    Area* areaA;
};

BOOST_AUTO_TEST_SUITE(areas_operations)

BOOST_AUTO_TEST_CASE(area_add)
{
    auto study = std::make_unique<Study>() ;
    const auto areaA = study->areaAdd("A");
    BOOST_CHECK(areaA != nullptr);
    BOOST_CHECK_EQUAL(areaA->name, "A");
    BOOST_CHECK_EQUAL(areaA->id, "a");
}

BOOST_FIXTURE_TEST_CASE(area_rename, OneAreaStudy)
{
    BOOST_CHECK(study.areaRename(areaA, "B"));
    BOOST_CHECK(areaA->name == "B");
    BOOST_CHECK(areaA->id == "b");
}

BOOST_FIXTURE_TEST_CASE(area_delete, OneAreaStudy)
{
    BOOST_CHECK_EQUAL(study.areas.size(), 1);
    BOOST_CHECK(study.areaDelete(areaA));
    BOOST_CHECK(study.areas.empty());
}

BOOST_AUTO_TEST_SUITE_END() //areas

// Check that disabled objects are indeed removed from computations
BOOST_AUTO_TEST_SUITE(remove_disabled)
BOOST_FIXTURE_TEST_CASE(thermal_cluster_delete, OneAreaStudy)
{
    auto disabledCluster = std::make_shared<ThermalCluster>(areaA);
    disabledCluster->setName("Cluster1");
    disabledCluster->enabled = false;

    areaA->thermal.list.add(disabledCluster);
    // Check that "Cluster1" is found
    BOOST_CHECK_EQUAL(areaA->thermal.list.find("cluster1"), disabledCluster.get());

    study.initializeRuntimeInfos(); // This should remove all disabled thermal clusters
    // Check that "Cluster1" isn't found
    BOOST_CHECK_EQUAL(areaA->thermal.list.find("cluster1"), nullptr);
}

BOOST_FIXTURE_TEST_CASE(renewable_cluster_delete, OneAreaStudy)
{
    auto disabledCluster = std::make_shared<RenewableCluster>(areaA);
    disabledCluster->setName("Cluster1");
    disabledCluster->enabled = false;

    areaA->renewable.list.add(disabledCluster);
    // Check that "Cluster1" is found
    BOOST_CHECK_EQUAL(areaA->renewable.list.find("cluster1"), disabledCluster.get());

    study.initializeRuntimeInfos(); // This should remove all disabled renewable clusters
    // Check that "Cluster1" isn't found
    BOOST_CHECK_EQUAL(areaA->renewable.list.find("cluster1"), nullptr);
}

BOOST_FIXTURE_TEST_CASE(short_term_storage_delete, OneAreaStudy)
{
    auto& sts = areaA->shortTermStorage.storagesByIndex;

    auto addSTS = [&sts](std::string&& name,
                         bool enabled)
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
                          {
                              return c.properties.name == name;
                          });
    };

    // Check that "Cluster1" and "Cluster2" are found
    BOOST_CHECK(findDisabledCluster("Cluster1") != sts.end());
    BOOST_CHECK(findDisabledCluster("Cluster2") != sts.end());

    study.initializeRuntimeInfos(); // This should remove all disabled short-term storages

    // Check that only "Cluster1" is found
    BOOST_CHECK(findDisabledCluster("Cluster1") != sts.end());
    BOOST_CHECK(findDisabledCluster("Cluster2") == sts.end());

    // operator<< doesn't exist for iterators, Boost can't generate output in case of failure, so we use BOOST_CHECK instead of BOOST_CHECK_EQUAL
}

BOOST_AUTO_TEST_SUITE_END() // remove_disabled


BOOST_AUTO_TEST_SUITE(thermal_clusters_operations)

BOOST_FIXTURE_TEST_CASE(thermal_cluster_add, OneAreaStudy)
{
    auto newCluster = std::make_shared<ThermalCluster>(areaA);
    newCluster->setName("Cluster");
    BOOST_CHECK_EQUAL(newCluster->name(), "Cluster");
    BOOST_CHECK_EQUAL(newCluster->id(), "cluster");

    areaA->thermal.list.add(newCluster);
    BOOST_CHECK_EQUAL(areaA->thermal.list.find("cluster"), newCluster.get());
    BOOST_CHECK_EQUAL(areaA->thermal.list.find("Cluster"), nullptr);
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
        areaA->thermal.list.add(newCluster);
        cluster = newCluster.get();
    }

    ThermalCluster* cluster;
};

BOOST_FIXTURE_TEST_CASE(thermal_cluster_rename, ThermalClusterStudy)
{
    BOOST_CHECK(study.clusterRename(cluster, "Renamed"));
    BOOST_CHECK(cluster->name() == "Renamed");
    BOOST_CHECK(cluster->id() == "renamed");
}

BOOST_FIXTURE_TEST_CASE(thermal_cluster_delete, ThermalClusterStudy)
{
    BOOST_CHECK(areaA->thermal.list.find("cluster") == cluster);
    areaA->thermal.list.remove("cluster");
    BOOST_CHECK(areaA->thermal.list.find("cluster") == nullptr);
    BOOST_CHECK(areaA->thermal.list.empty());
}

BOOST_AUTO_TEST_SUITE_END() // thermal clusters

BOOST_AUTO_TEST_SUITE(renewable_clusters_operations)

BOOST_FIXTURE_TEST_CASE(renewable_cluster_add, OneAreaStudy)
{
    auto newCluster = std::make_shared<RenewableCluster>(areaA);
    newCluster->setName("WindCluster");
    BOOST_CHECK(newCluster->name() == "WindCluster");
    BOOST_CHECK(newCluster->id() == "windcluster");

    areaA->renewable.list.add(newCluster);
    BOOST_CHECK(areaA->renewable.list.find("windcluster") == newCluster.get());
    BOOST_CHECK(areaA->renewable.list.find("WindCluster") == nullptr);
}


/*!
 * Study with:
 *  - one area named "A"
 *  - one renewable cluster named "WindCluster"
 */
struct RenewableClusterStudy : public OneAreaStudy
{
    RenewableClusterStudy()
    {
        areaA = study.areaAdd("A");
        auto newCluster = std::make_shared<RenewableCluster>(areaA);
        newCluster->setName("WindCluster");
        areaA->renewable.list.add(newCluster);
        cluster = newCluster.get();
    }

    RenewableCluster* cluster;
};

BOOST_FIXTURE_TEST_CASE(renewable_cluster_rename, RenewableClusterStudy)
{
    BOOST_CHECK(study.clusterRename(cluster, "Renamed"));
    BOOST_CHECK(cluster->name() == "Renamed");
    BOOST_CHECK(cluster->id() == "renamed");
}

BOOST_FIXTURE_TEST_CASE(renewable_cluster_delete, RenewableClusterStudy)
{
    BOOST_CHECK(areaA->renewable.list.find("windcluster") == cluster);
    BOOST_CHECK(areaA->renewable.list.remove("windcluster"));
    BOOST_CHECK(areaA->renewable.list.find("windcluster") == nullptr);
    BOOST_CHECK(areaA->renewable.list.empty());
}

BOOST_AUTO_TEST_SUITE_END() //renewable clusters

BOOST_AUTO_TEST_SUITE(studyVersion_class)

BOOST_AUTO_TEST_CASE(version_comparison)
{
    StudyVersion v1(7, 2), v2(8, 0), v3("8.0");
    BOOST_CHECK(v1 < v2);
    BOOST_CHECK(!(v1 > v2));
    BOOST_CHECK(v1 != v2);
    BOOST_CHECK(v2 == v3);
    BOOST_CHECK(StudyVersion(12, 3) > StudyVersion(1, 23));
}

BOOST_AUTO_TEST_CASE(version_parsing)
{
    BOOST_CHECK(StudyVersion(7,2) == StudyVersion("7.2"));
    BOOST_CHECK(StudyVersion("abc") == StudyVersion::unknown());

    BOOST_CHECK(StudyVersion::buildVersionLegacyOrCurrent("860") == StudyVersion(8, 6));
    BOOST_CHECK(StudyVersion::buildVersionLegacyOrCurrent("8.6") == StudyVersion(8, 6));
    BOOST_CHECK(StudyVersion::buildVersionLegacyOrCurrent("8..6") == StudyVersion::unknown());

    // 4.5 is not in the list of supported versions, thus failing
    BOOST_CHECK(StudyVersion::buildVersionLegacyOrCurrent("4.5") == StudyVersion::unknown());
}

BOOST_AUTO_TEST_SUITE_END() //version
