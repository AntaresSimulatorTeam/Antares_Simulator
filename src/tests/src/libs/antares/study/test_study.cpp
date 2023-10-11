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

BOOST_AUTO_TEST_CASE(basic_areas_operations)
{
    Study study;
    Area* areaA = study.areaAdd("A");
    BOOST_CHECK(areaA != nullptr);
    BOOST_CHECK(areaA->name == "A");
    BOOST_CHECK(areaA->id == "a");
    BOOST_CHECK(study.areaRename(areaA, "B"));
    BOOST_CHECK(areaA->name == "B");
    BOOST_CHECK(areaA->id == "b");
    BOOST_CHECK(study.areaDelete(areaA));
    BOOST_CHECK(study.areas.empty());
}

BOOST_AUTO_TEST_CASE(basic_thermal_clusters_operations)
{
    Study study;
    Area& area = *study.areaAdd("A");
    auto newCluster = std::make_shared<ThermalCluster>(&area);
    newCluster->setName("Cluster");
    BOOST_CHECK(newCluster->name() == "Cluster");
    BOOST_CHECK(newCluster->id() == "cluster");

    area.thermal.list.add(newCluster);
    BOOST_CHECK(area.thermal.list.find("cluster") == newCluster.get());
    BOOST_CHECK(area.thermal.list.find("Cluster") == nullptr);

    //Renaming
    BOOST_CHECK(study.clusterRename(newCluster.get(), "Renamed"));
    BOOST_CHECK(newCluster->name() == "Renamed");
    BOOST_CHECK(newCluster->id() == "renamed");

    area.thermal.list.remove("renamed");
    BOOST_CHECK(area.thermal.list.find("renamed") == nullptr);
    BOOST_CHECK(area.thermal.list.empty());
}

BOOST_AUTO_TEST_CASE(basic_renewable_clusters_operations)
{
    Study study;
    Area& area = *study.areaAdd("A");
    auto newCluster = std::make_shared<RenewableCluster>(&area);
    newCluster->setName("WindCluster");
    BOOST_CHECK(newCluster->name() == "WindCluster");
    BOOST_CHECK(newCluster->id() == "windcluster");

    area.renewable.list.add(newCluster);
    BOOST_CHECK(area.renewable.list.find("windcluster") == newCluster.get());
    BOOST_CHECK(area.renewable.list.find("WindCluster") == nullptr);

    //Renaming
    BOOST_CHECK(study.clusterRename(newCluster.get(), "Renamed"));
    BOOST_CHECK(newCluster->name() == "Renamed");
    BOOST_CHECK(newCluster->id() == "renamed");

    BOOST_CHECK(area.renewable.list.remove("renamed"));
    BOOST_CHECK(area.renewable.list.find("renamed") == nullptr);
    BOOST_CHECK(area.renewable.list.empty());
}
