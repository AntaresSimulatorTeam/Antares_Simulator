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

#include <antares/logs/logs.h>
#include <antares/study/duplicates.h>
#include <antares/study/study.h>
#include "antares/study/parts/short-term-storage/container.h"

using Antares::logs;
using Antares::Check::checkForDuplicates;
using Antares::Data::RenewableCluster;
using Antares::Data::Study;
using Antares::Data::ThermalCluster;
using namespace Antares::Data::ShortTermStorage;

BOOST_AUTO_TEST_SUITE(study_duplicates)

struct MessageHandler: public Yuni::IEventObserver<MessageHandler, Yuni::Policy::SingleThreaded>
{
    MessageHandler()
    {
        logs.callback.connect(this, &MessageHandler::onLogMessage);
    }

    void onLogMessage(int level, const std::string& message)
    {
        if (level == Yuni::Logs::Verbosity::Error::level)
        {
            errors.insert(message);
        }
    }

    ~MessageHandler()
    {
        logs.callback.clear();
        destroyBoundEvents();
    }

    std::set<std::string> errors;
};

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(area);
        c->setName(name);
        area->thermal.list.addToCompleteList(c);
    };
    addCluster("cluster");
    addCluster("cluster");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(area);
        c->setName(name);
        area->thermal.list.addToCompleteList(c);
    };
    addCluster("cluster");
    addCluster("cluster");

    addCluster("cluster_2");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 2); // Stops after first error
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster` found in area `a`"));
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(area);
        c->setName(name);
        area->thermal.list.addToCompleteList(c);
    };

    addCluster("cluster_1");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(two_areas_two_duplicate_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");
    const auto areaB = study->areaAdd("B");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(area);
        c->setName(name);
        areaB->thermal.list.addToCompleteList(c);
    };

    addCluster("cluster");
    addCluster("cluster");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster` found in area `b`"));
}

BOOST_FIXTURE_TEST_CASE(two_areas_two_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");
    const auto areaB = study->areaAdd("B");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(area);
        c->setName(name);
        areaB->thermal.list.addToCompleteList(c);
    };

    checkForDuplicates(*study);
    addCluster("cluster_1");
    addCluster("cluster_2");

    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(two_areas_four_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    const auto areaB = study->areaAdd("B");

    auto addClusterA = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(areaA);
        c->setName(name);
        areaA->thermal.list.addToCompleteList(c);
    };

    auto addClusterB = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(areaB);
        c->setName(name);
        areaB->thermal.list.addToCompleteList(c);
    };

    addClusterA("cluster_1");
    addClusterA("cluster_2");
    addClusterB("cluster_1");
    addClusterB("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(two_areas_eight_duplicates_thermal_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    const auto areaB = study->areaAdd("B");

    auto addClusterA = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(areaA);
        c->setName(name);
        areaA->thermal.list.addToCompleteList(c);
    };

    auto addClusterB = [&](const std::string& name)
    {
        auto c = std::make_shared<ThermalCluster>(areaB);
        c->setName(name);
        areaB->thermal.list.addToCompleteList(c);
    };

    addClusterA("cluster_1");
    addClusterA("cluster_2");
    addClusterB("cluster_1");
    addClusterB("cluster_2");
    addClusterA("cluster_1");
    addClusterA("cluster_2");
    addClusterB("cluster_1");
    addClusterB("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 4);
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster_1` found in area `a`"));
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster_2` found in area `a`"));
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster_1` found in area `b`"));
    BOOST_CHECK(errors.contains("Duplicate thermal cluster `cluster_2` found in area `b`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_renewable_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<RenewableCluster>(area);
        c->setName(name);
        area->renewable.list.addToCompleteList(c);
    };
    addCluster("cluster");
    addCluster("cluster");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK(errors.contains("Duplicate renewable cluster `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_renewable_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<RenewableCluster>(area);
        c->setName(name);
        area->renewable.list.addToCompleteList(c);
    };
    addCluster("cluster");
    addCluster("cluster");

    addCluster("cluster_2");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 2);
    BOOST_CHECK(errors.contains("Duplicate renewable cluster `cluster` found in area `a`"));
    BOOST_CHECK(errors.contains("Duplicate renewable cluster `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_renewable_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        auto c = std::make_shared<RenewableCluster>(area);
        c->setName(name);
        area->renewable.list.addToCompleteList(c);
    };
    addCluster("cluster_1");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(single_area_two_duplicate_STS_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        STStorageCluster cluster;
        cluster.properties.name = name;
        area->shortTermStorage.storagesByIndex.push_back(cluster);
    };
    addCluster("cluster");
    addCluster("cluster");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK(errors.contains("Duplicate short term storage `cluster` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_four_duplicate_STS_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        STStorageCluster cluster;
        cluster.properties.name = name;
        area->shortTermStorage.storagesByIndex.push_back(cluster);
    };
    addCluster("cluster");
    addCluster("cluster");

    addCluster("cluster_2");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 2);
    BOOST_CHECK(errors.contains("Duplicate short term storage `cluster` found in area `a`"));
    BOOST_CHECK(errors.contains("Duplicate short term storage `cluster_2` found in area `a`"));
}

BOOST_FIXTURE_TEST_CASE(single_area_two_STS_clusters, MessageHandler)
{
    auto study = std::make_unique<Study>();
    const auto area = study->areaAdd("A");

    auto addCluster = [&](const std::string& name)
    {
        STStorageCluster cluster;
        cluster.properties.name = name;
        area->shortTermStorage.storagesByIndex.push_back(cluster);
    };
    addCluster("cluster_1");
    addCluster("cluster_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(detection_of_duplicate_constraints, MessageHandler)
{
    // Creating studies
    auto study = std::make_unique<Study>();

    auto bc_1 = study->bindingConstraints.add("dummy");
    auto bc_2 = study->bindingConstraints.add("dummy");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK(errors.contains("Duplicate binding constraint `dummy` found in study"));
}

BOOST_FIXTURE_TEST_CASE(detection_of_more_duplicate_constraints, MessageHandler)
{
    // Creating studies
    auto study = std::make_unique<Study>();

    auto bc_1 = study->bindingConstraints.add("dummy");
    auto bc_2 = study->bindingConstraints.add("dummy");

    auto bc_3 = study->bindingConstraints.add("dummy_2");
    auto bc_4 = study->bindingConstraints.add("dummy_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 2);
    BOOST_CHECK(errors.contains("Duplicate binding constraint `dummy` found in study"));
    BOOST_CHECK(errors.contains("Duplicate binding constraint `dummy_2` found in study"));
}

BOOST_FIXTURE_TEST_CASE(detection_of_non_duplicate_constraints, MessageHandler)
{
    // Creating studies
    auto study = std::make_unique<Study>();

    auto bc_1 = study->bindingConstraints.add("dummy_1");
    auto bc_2 = study->bindingConstraints.add("dummy_2");

    checkForDuplicates(*study);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
}
BOOST_AUTO_TEST_SUITE_END()
