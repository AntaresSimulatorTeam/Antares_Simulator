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

#include "antares/study/builder.h"

BOOST_AUTO_TEST_CASE(two_areas)
{
    auto study = std::make_unique<Antares::Data::Study>(true);
    StudyBuilder builder(*study);
    builder.addArea()
      .setName("hello")
      .setUnsuppliedEnergyCost(221)
      .setSpilledEnergyCost(20)
      .setNodalOptimization(2)
      .add()
      .addArea()
      .setName("world")
      .setUnsuppliedEnergyCost(221)
      .setSpilledEnergyCost(20)
      .setNodalOptimization(2)
      .add()
      .build();
    BOOST_CHECK_EQUAL(study->areas.size(), 2);
}
