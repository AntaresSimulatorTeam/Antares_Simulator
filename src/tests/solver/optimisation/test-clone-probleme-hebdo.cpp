// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

/// \file test-clone-probleme-hebdo.cpp
/// \brief Unit tests for cloneProblemHebdoForWeek().
///
/// Validates that:
///  - The clone is deep-independent: mutating a vector in the clone
///    does not affect the source.
///  - previousSimulationFinalLevel is correctly overridden by the
///    precomputed initial levels.
///  - weekInTheYear is set to the requested weekIndex.

#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"
#include "antares/solver/simulation/simulation.h"

#define BOOST_TEST_MODULE clone_probleme_hebdo

#include <boost/test/unit_test.hpp>

#include "in-memory-study.h"

using namespace Antares::Solver::Simulation;

namespace
{
constexpr unsigned int gNumberTimeSteps = 168;
} // namespace

BOOST_AUTO_TEST_CASE(clone_is_independent_of_source_vectors)
{
    // Build a minimal study with 2 areas so we have meaningful data structures.
    StudyBuilder builder;
    builder.addAreaToStudy("FR");
    builder.addAreaToStudy("DE");

    PROBLEME_HEBDO src;
    SIM_AllocationProblemeHebdo(*builder.study, src, gNumberTimeSteps);

    // Set distinctive initial levels in the source.
    const double srcLevel0 = 123.0;
    const double srcLevel1 = 456.0;
    src.previousSimulationFinalLevel[0] = srcLevel0;
    src.previousSimulationFinalLevel[1] = srcLevel1;

    // Precomputed levels to inject for the clone.
    const double precompLevel0 = 999.0;
    const double precompLevel1 = 888.0;
    std::vector<double> precomputed = {precompLevel0, precompLevel1};

    const uint weekIndex = 5u;
    PROBLEME_HEBDO dst = cloneProblemHebdoForWeek(src, weekIndex, precomputed);

    // --- weekInTheYear should be overridden ---
    BOOST_CHECK_EQUAL(dst.weekInTheYear, weekIndex);

    // --- previousSimulationFinalLevel should be overridden ---
    BOOST_CHECK_EQUAL(dst.previousSimulationFinalLevel[0], precompLevel0);
    BOOST_CHECK_EQUAL(dst.previousSimulationFinalLevel[1], precompLevel1);

    // --- Source levels must be unchanged ---
    BOOST_CHECK_EQUAL(src.previousSimulationFinalLevel[0], srcLevel0);
    BOOST_CHECK_EQUAL(src.previousSimulationFinalLevel[1], srcLevel1);

    // --- Deep-independence: mutating a vector in the clone must not affect source ---
    BOOST_REQUIRE(!dst.CoutDeDefaillancePositive.empty());
    const double originalSrcValue = src.CoutDeDefaillancePositive[0];
    dst.CoutDeDefaillancePositive[0] = originalSrcValue + 1000.0;
    BOOST_CHECK_EQUAL(src.CoutDeDefaillancePositive[0], originalSrcValue);

    // --- Deep-independence: mutating hourly results in the clone ---
    BOOST_REQUIRE(!dst.ResultatsHoraires.empty());
    BOOST_REQUIRE(!dst.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive.empty());
    const double originalSrcHourly
      = src.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0];
    dst.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0]
      = originalSrcHourly + 777.0;
    BOOST_CHECK_EQUAL(src.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0],
                      originalSrcHourly);
}

BOOST_AUTO_TEST_CASE(clone_has_fresh_probleme_a_resoudre)
{
    StudyBuilder builder;
    builder.addAreaToStudy("FR");

    PROBLEME_HEBDO src;
    SIM_AllocationProblemeHebdo(*builder.study, src, gNumberTimeSteps);

    std::vector<double> precomputed(1, 0.0);
    PROBLEME_HEBDO dst = cloneProblemHebdoForWeek(src, 0u, precomputed);

    // ProblemeAResoudre must be a distinct object (different address)
    BOOST_CHECK(dst.ProblemeAResoudre.get() != src.ProblemeAResoudre.get());
    BOOST_CHECK(dst.ProblemeAResoudre != nullptr);
}

BOOST_AUTO_TEST_CASE(clone_source_week_index_unchanged)
{
    StudyBuilder builder;
    builder.addAreaToStudy("FR");

    PROBLEME_HEBDO src;
    SIM_AllocationProblemeHebdo(*builder.study, src, gNumberTimeSteps);
    src.weekInTheYear = 10u;

    std::vector<double> precomputed(1, 0.0);
    PROBLEME_HEBDO dst = cloneProblemHebdoForWeek(src, 3u, precomputed);

    // Clone's week index should be the requested one, source unchanged
    BOOST_CHECK_EQUAL(dst.weekInTheYear, 3u);
    BOOST_CHECK_EQUAL(src.weekInTheYear, 10u);
}
