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
#define BOOST_TEST_MODULE test solver simulation things

#define WIN32_LEAN_AND_MEAN

#include <algorithm> // std::adjacent_find

#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/timeseries-numbers.h>
#include <antares/study/duplicates.h>
#include <antares/utils/utils.h>
#include "antares/solver/ts-generator/generator.h"

using namespace Yuni;
using namespace Antares::Data;
using namespace Antares::Solver::TimeSeriesNumbers;


void initializeStudy(Study::Ptr study, unsigned int nbYears = 1)
{
    study->parameters.derated = false;

    study->runtime.rangeLimits.year[rangeBegin] = 0;
    study->runtime.rangeLimits.year[rangeEnd] = nbYears - 1;

    study->parameters.renewableGeneration.toAggregated(); // Default

    study->parameters.intraModal = 0;
    study->parameters.interModal = 0;
    study->parameters.timeSeriesToRefresh = 0;
}

// ========================
// Add an area to study
// ========================
Area* addAreaToStudy(Study::Ptr study, const std::string& areaName)
{
    Area* area = study->areaAdd(areaName);
    BOOST_CHECK(area);

    return area;
}

// ===========================
// Add a cluster to an area
// ===========================

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
    area->thermal.list.addToCompleteList(cluster);
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
    area->renewable.list.addToCompleteList(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ClusterType>(area);
    cluster->setName(clusterName);
    addClusterToAreaList(area, cluster);

    return cluster;
}

BOOST_AUTO_TEST_CASE(detection_of_duplicate_thermal_clusters)
{
    // Creating studies
    auto study_1 = std::make_shared<Study>();
    auto study_2 = std::make_shared<Study>();
    initializeStudy(study_1);
    initializeStudy(study_2);

    Area* area_1 = addAreaToStudy(study_1, "Area");
    Area* area_2 = addAreaToStudy(study_2, "Area");

    auto cluster1 = addClusterToArea<ThermalCluster>(area_1, "th-cluster");
    auto cluster2 = addClusterToArea<ThermalCluster>(area_1, "th-cluster");

    auto cluster3 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-1");
    auto cluster4 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-2");
    
    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_1), false);
    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_2), true);
}

BOOST_AUTO_TEST_CASE(detection_of_duplicate_renewable_clusters)
{
    // Creating studies
    auto study_1 = std::make_shared<Study>();
    auto study_2 = std::make_shared<Study>();
    initializeStudy(study_1);
    initializeStudy(study_2);

    Area* area_1 = addAreaToStudy(study_1, "Area");
    Area* area_2 = addAreaToStudy(study_2, "Area");

    auto cluster1 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster");
    auto cluster2 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster");

    auto cluster3 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-1");
    auto cluster4 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-2");

    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_1), false);
    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_2), true);
}

BOOST_AUTO_TEST_CASE(detection_of_duplicate_constraints)
{
    // Creating studies
    auto study_1 = std::make_shared<Study>();
    auto study_2 = std::make_shared<Study>();
    initializeStudy(study_1);
    initializeStudy(study_2);

    auto bc_1 = study_1->bindingConstraints.add("dummy");
    auto bc_2 = study_1->bindingConstraints.add("dummy");

    auto bc_3 = study_2->bindingConstraints.add("dummy_1");
    auto bc_4 = study_2->bindingConstraints.add("dummy_2");
    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_1), false);
    BOOST_CHECK_EQUAL(Antares::Check::checkForDuplicates(*study_2), true);
}

/* BOOST_AUTO_TEST_CASE(test_errors_ducplicate)
{
    //BOOST_CHECK_EQUAL(quoteArea("areaName"),"");
}*/
