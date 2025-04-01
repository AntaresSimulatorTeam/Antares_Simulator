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

using Antares::logs;
using Antares::Check::checkForDuplicates;
using Antares::Data::Study;
using Antares::Data::ThermalCluster;

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
            errors.emplace_back(message);
        }
    }

    ~MessageHandler()
    {
        logs.callback.clear();
        destroyBoundEvents();
    }

    std::vector<std::string> errors;
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

    BOOST_CHECK(!checkForDuplicates(*study));

    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK_EQUAL(errors[0], "Duplicate thermal cluster `cluster` found in area `a`");
}
BOOST_AUTO_TEST_SUITE_END()
