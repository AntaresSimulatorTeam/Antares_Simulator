// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test congestion probability"
#define WIN32_LEAN_AND_MEAN

#include <limits>

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/economy/links/congestionProbability.h"

using namespace Antares::Solver::Variable;

namespace
{
struct Fixture
{
    Fixture():
        study(std::make_unique<Antares::Data::Study>()),
        state(*study)
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->parameters.nbYears = 2;
        study->maxNbYearsInParallel = 1;
        study->parameters.resetPlaylist(2);
        study->initializeRuntimeInfos();
        variable.initializeFromStudy(*study);
        variable.simulationBegin();

        link.index = 0;
        state.link = &link;
        state.year = 0;
        state.hourInTheYear = 192;
        state.hourInTheWeek = 24;
        state.ntc.ValeurDuFlux = {0.};
        state.ntc.ValeurDeNTCOrigineVersExtremite = {0.};
        state.ntc.ValeurDeNTCExtremiteVersOrigine = {0.};
    }

    void check(double flow,
               double directCapacity,
               double indirectCapacity,
               double expectedDirect,
               double expectedIndirect)
    {
        variable.yearBegin(0, 0);
        state.ntc.ValeurDuFlux[0] = flow;
        state.ntc.CapaciteEffectiveOrigineVersExtremite = {directCapacity};
        state.ntc.CapaciteEffectiveExtremiteVersOrigine = {indirectCapacity};
        variable.hourForEachLink(state, 0);
        BOOST_CHECK_EQUAL(variable.retrieveRawHourlyValuesForCurrentYear(0, 0)[192],
                          expectedDirect);
        BOOST_CHECK_EQUAL(variable.retrieveRawHourlyValuesForCurrentYear(1, 0)[192],
                          expectedIndirect);
    }

    std::unique_ptr<Antares::Data::Study> study;
    State state;
    Antares::Data::AreaLink link;
    Economy::CongestionProbability variable;
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(congestion_probability, Fixture)

BOOST_AUTO_TEST_CASE(increased_capacities_do_not_report_unsaturated_flows_as_congested)
{
    check(50., 100., 100., 0., 0.);
    check(-50., 100., 100., 0., 0.);
    check(100., 100., 100., 50., 0.);
    check(-100., 100., 100., 0., 50.);
}

BOOST_AUTO_TEST_CASE(reduced_capacities_report_congestion_in_both_directions)
{
    state.ntc.ValeurDeNTCOrigineVersExtremite[0] = 200.;
    state.ntc.ValeurDeNTCExtremiteVersOrigine[0] = 200.;
    check(50., 50., 80., 50., 0.);
    check(-80., 50., 80., 0., 50.);
}

BOOST_AUTO_TEST_CASE(congestion_tolerance_is_preserved)
{
    check(100. - 5e-6, 100., 100., 50., 0.);
    check(100. - 2e-5, 100., 100., 0., 0.);
    check(-100. + 5e-6, 100., 100., 0., 50.);
    check(-100. + 2e-5, 100., 100., 0., 0.);
}

BOOST_AUTO_TEST_CASE(missing_effective_capacities_fall_back_to_legacy_ntc)
{
    state.ntc.ValeurDeNTCOrigineVersExtremite[0] = 60.;
    state.ntc.ValeurDeNTCExtremiteVersOrigine[0] = 90.;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    check(50., nan, nan, 0., 0.);
    check(60., nan, nan, 50., 0.);
    check(-90., nan, nan, 0., 50.);

    variable.yearBegin(0, 0);
    state.ntc.CapaciteEffectiveOrigineVersExtremite.clear();
    state.ntc.CapaciteEffectiveExtremiteVersOrigine.clear();
    variable.hourForEachLink(state, 0);
    BOOST_CHECK_EQUAL(variable.retrieveRawHourlyValuesForCurrentYear(1, 0)[192], 50.);
}

BOOST_AUTO_TEST_SUITE_END()
