// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test dynamic aggregation"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

#include "in-memory-study.h"

using namespace Antares::Solver::Variable;
using namespace Antares::Data;

std::unique_ptr<Study> createMockStudy()
{
    auto study = std::make_unique<Study>();

    // Create Area1 with thermal groups A and B
    Area* area1 = addAreaToListOfAreas(study->areas, "Area1");
    area1->index = 0;

    auto thermalA1 = addClusterToArea(area1, "Thermal_A1");
    auto thermalB1 = addClusterToArea(area1, "Thermal_B1");
    thermalA1->setGroup("A");
    thermalB1->setGroup("B");
    thermalA1->index = 0;
    thermalB1->index = 1;

    // Create Area2 with thermal groups B and C
    Area* area2 = addAreaToListOfAreas(study->areas, "Area2");
    area2->index = 1;

    auto thermalB2 = addClusterToArea(area2, "Thermal_B2");
    auto thermalC2 = addClusterToArea(area2, "Thermal_C2");
    thermalB2->setGroup("B");
    thermalC2->setGroup("C");
    thermalB2->index = 0;
    thermalC2->index = 1;

    return study;
}

PROBLEME_HEBDO createMockProblemHebdo()
{
    PROBLEME_HEBDO pb;
    pb.HeureDansLAnnee = 0;
    pb.year = 0;
    pb.NombreDePays = 2;

    // Initialize results for 2 areas
    pb.ResultatsHoraires.resize(2);

    // Area1 results
    auto& area1Results = pb.ResultatsHoraires[0];
    area1Results.ProductionThermique.resize(Constants::nbHoursInAWeek);

    // Set up thermal production results for Area1
    for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
    {
        PRODUCTION_THERMIQUE_OPTIMALE thermalProd;
        thermalProd.ProductionThermiqueDuPalier.resize(2);      // 2 thermal clusters
        thermalProd.ProductionThermiqueDuPalier[0] = 200.0 + h; // Thermal A1
        thermalProd.ProductionThermiqueDuPalier[1] = 150.0 + h; // Thermal B1
        area1Results.ProductionThermique[h] = thermalProd;
    }

    // Area2 results
    auto& area2Results = pb.ResultatsHoraires[1];
    area2Results.ProductionThermique.resize(Constants::nbHoursInAWeek);

    // Set up thermal production results for Area2
    for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
    {
        PRODUCTION_THERMIQUE_OPTIMALE thermalProd;
        thermalProd.ProductionThermiqueDuPalier.resize(2);      // 2 thermal clusters
        thermalProd.ProductionThermiqueDuPalier[0] = 180.0 + h; // Thermal B2
        thermalProd.ProductionThermiqueDuPalier[1] = 120.0 + h; // Thermal C2
        area2Results.ProductionThermique[h] = thermalProd;
    }

    return pb;
}

BOOST_AUTO_TEST_SUITE(DynamicAggregationWithMocksTests)

BOOST_AUTO_TEST_CASE(SetDataSingleYearAddResultsToSet_WithMockData)
{
    auto study = createMockStudy();

    std::set<Area*, CompareAreaName> areaSet;
    for (auto& [_, area]: study->areas)
    {
        areaSet.insert(area.get());
    }

    SetDataSingleYear setData(areaSet);
    PROBLEME_HEBDO pb = createMockProblemHebdo();
    BOOST_CHECK_NO_THROW(setData.addResultsToSet(pb));

    auto thermalResults = setData.getThermalResults();
    BOOST_CHECK_EQUAL(thermalResults.size(), 3);  // Groups A, B, C
    BOOST_CHECK_EQUAL(thermalResults[0][0], 200); // A = 200
    BOOST_CHECK_EQUAL(thermalResults[1][0], 330); // B = 150 + 180
    BOOST_CHECK_EQUAL(thermalResults[2][0], 120); // C = 120

    BOOST_CHECK_EQUAL(thermalResults[1][160], 650); // B = (150+160) + (180+160)
}

BOOST_AUTO_TEST_SUITE_END()
