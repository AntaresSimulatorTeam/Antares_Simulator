// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test NbOfDispatchedUnitsByPlant"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/economy/nbOfDispatchedUnitsByPlant.h"
#include "antares/solver/variable/state.h"

#include "in-memory-study.h"

using namespace Antares::Solver::Variable;
using namespace Antares::Solver::Variable::Economy;
using namespace Antares::Data;

namespace
{

struct StudyWithOneArea
{
    StudyWithOneArea()
    {
        study = std::make_unique<Study>();
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->maxNbYearsInParallel = 1;
        study->parameters.nbYears = 1;

        area = addAreaToListOfAreas(study->areas, "TestArea");
        if (area)
        {
            area->createMissingData();
            area->resetToDefaultValues();
        }
        study->areas.rebuildIndexes();
        area->index = 0;
        cluster = addClusterToArea(area, "TestCluster").get();

        study->initializeRuntimeInfos();
        study->parameters.resetPlaylist(study->parameters.nbYears);
    }

    std::unique_ptr<Study> study;
    Area* area = nullptr;
    ThermalCluster* cluster = nullptr;
};

} // anonymous namespace

BOOST_AUTO_TEST_SUITE(NbOfDispatchedUnitsByPlantTests)

// Regression test: verifies that yearEndBuildPrepareDataForEachThermalCluster is actually called
// by the base class for NbOfDispatchedUnitsByPlantTraits, i.e. that
// thermalClusterDispatchedUnitsCountForYear is populated from the hourly NODU values.
//
// Before the fix,
// DispatchablePlantByClusterBase::yearEndBuildPrepareDataForEachThermalClusterIfSupported only
// checked for a 5-parameter variant of the trait method (with auxiliaryData).
// NbOfDispatchedUnitsByPlantTraits provides a 4-parameter variant (without auxiliaryData), so the
// dispatch silently did nothing, leaving thermalClusterDispatchedUnitsCountForYear as zeros and
// causing wrong NODU and OP. COST output values.
BOOST_AUTO_TEST_CASE(yearEndBuildPrepare_populatesDispatchedUnitsCountForYear)
{
    StudyWithOneArea fixture;
    auto* area = fixture.area;
    auto* cluster = fixture.cluster;

    State state(*fixture.study);
    state.area = area;

    NbOfDispatchedUnitsByPlant<> variable;
    variable.initializeFromStudy(*fixture.study);
    variable.initializeFromArea(fixture.study.get(), area);

    const unsigned int numSpace = 0;
    const uint nodu_hour0 = 3;
    const uint nodu_hour1 = 5;

    variable.yearBegin(0, numSpace);

    state.hourInTheYear = 0;
    state.thermal[area->index].numberOfUnitsONbyCluster[cluster->enabledIndex] = nodu_hour0;
    variable.hourForEachArea(state, numSpace);

    state.hourInTheYear = 1;
    state.thermal[area->index].numberOfUnitsONbyCluster[cluster->enabledIndex] = nodu_hour1;
    variable.hourForEachArea(state, numSpace);

    state.yearEndResetThermal();
    state.thermalCluster = cluster;

    variable.yearEndBuildPrepareDataForEachThermalCluster(state, 0, numSpace);

    BOOST_CHECK_EQUAL(state.thermalClusterDispatchedUnitsCountForYear[0], nodu_hour0);
    BOOST_CHECK_EQUAL(state.thermalClusterDispatchedUnitsCountForYear[1], nodu_hour1);
}

// Verifies that yearEndBuildForEachThermalCluster correctly copies back from
// thermalClusterDispatchedUnitsCountForYear into the intermediate hourly values.
BOOST_AUTO_TEST_CASE(yearEndBuildForEach_copiesBackDispatchedUnitsCount)
{
    StudyWithOneArea fixture;
    auto* area = fixture.area;
    auto* cluster = fixture.cluster;

    const unsigned int rangeBegin = fixture.study->runtime.rangeLimits.hour[Data::rangeBegin];
    const unsigned int rangeEnd = fixture.study->runtime.rangeLimits.hour[Data::rangeEnd];

    State state(*fixture.study);
    state.area = area;
    state.thermalCluster = cluster;
    state.yearEndResetThermal();

    NbOfDispatchedUnitsByPlant<> variable;
    variable.initializeFromStudy(*fixture.study);
    variable.initializeFromArea(fixture.study.get(), area);
    variable.yearBegin(0, 0);

    const unsigned int numSpace = 0;
    const uint expectedNodu = 7;

    for (unsigned int h = rangeBegin; h <= rangeEnd; ++h)
    {
        state.thermalClusterDispatchedUnitsCountForYear[h] = expectedNodu;
    }

    variable.yearEndBuildForEachThermalCluster(state, 0, numSpace);

    // Verify the build step wrote back the values by reading hourly results
    const double* hourlyValues = variable.retrieveRawHourlyValuesForCurrentYear(0, numSpace);
    BOOST_REQUIRE(hourlyValues != nullptr);
    BOOST_CHECK_CLOSE(hourlyValues[rangeBegin], static_cast<double>(expectedNodu), 1e-6);
    BOOST_CHECK_CLOSE(hourlyValues[rangeEnd], static_cast<double>(expectedNodu), 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
