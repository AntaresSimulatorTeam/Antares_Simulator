// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <optional>
#include <string>
#include <vector>

#include <antares/solver/optimisation/LegacyVariableInfo.h>
#include <antares/solver/optimisation/opt_rename_problem.h>

#define BOOST_TEST_MODULE "legacy_variable_info_namer"

#include <boost/test/unit_test.hpp>

using Antares::Optimization::LegacyVariableInfo;

// Verifies that VariableNamer records the correct LegacyVariableInfo fields for
// the variable types consumed by AddLegacyExtraOutputs.

namespace
{
struct VarFixture
{
    std::vector<std::string> names = std::vector<std::string>(8);
    std::vector<std::optional<LegacyVariableInfo>>
      info = std::vector<std::optional<LegacyVariableInfo>>(8);
    VariableNamer namer{names, info};
};

struct ConstrFixture
{
    std::vector<std::string> names = std::vector<std::string>(4);
    std::vector<std::optional<LegacyVariableInfo>>
      info = std::vector<std::optional<LegacyVariableInfo>>(4);
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(variable_namer, VarFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_records_name_component_and_area)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(42);
    namer.DispatchableProduction(0, "gas_cluster");

    BOOST_REQUIRE(info[0].has_value());
    BOOST_CHECK_EQUAL(info[0]->name, "DispatchableProduction");
    BOOST_CHECK_EQUAL(info[0]->component, "gas_cluster");
    BOOST_CHECK_EQUAL(info[0]->timeIndex, 42u);
    BOOST_CHECK_EQUAL(info[0]->area.value(), "fr");
}

BOOST_AUTO_TEST_CASE(nodu_records_cluster_name_and_area)
{
    namer.UpdateArea("de");
    namer.UpdateTimeStep(100);
    namer.NODU(1, "coal_cluster");

    BOOST_REQUIRE(info[1].has_value());
    BOOST_CHECK_EQUAL(info[1]->name, "NODU");
    BOOST_CHECK_EQUAL(info[1]->component, "coal_cluster");
    BOOST_CHECK_EQUAL(info[1]->timeIndex, 100u);
    BOOST_CHECK_EQUAL(info[1]->area.value(), "de");
}

BOOST_AUTO_TEST_CASE(direct_flow_records_origin_destination_as_component)
{
    namer.updateExtremities("fr", "de");
    namer.UpdateTimeStep(7);
    namer.DirectFlow(2);

    BOOST_REQUIRE(info[2].has_value());
    BOOST_CHECK_EQUAL(info[2]->name, "DirectFlow");
    BOOST_CHECK_EQUAL(info[2]->component, "fr$$de");
    BOOST_CHECK_EQUAL(info[2]->timeIndex, 7u);
    BOOST_CHECK(!info[2]->area.has_value());
}

BOOST_AUTO_TEST_CASE(direct_flow_area_is_not_left_over_from_a_prior_area_anchored_variable)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(1);
    namer.UnsuppliedEnergy(0);

    namer.updateExtremities("fr", "de");
    namer.UpdateTimeStep(2);
    namer.DirectFlow(2);

    BOOST_REQUIRE(info[2].has_value());
    BOOST_CHECK(!info[2]->area.has_value());
}

BOOST_AUTO_TEST_CASE(unsupplied_energy_records_area_as_component)
{
    namer.UpdateArea("es");
    namer.UpdateTimeStep(5);
    namer.UnsuppliedEnergy(3);

    BOOST_REQUIRE(info[3].has_value());
    BOOST_CHECK_EQUAL(info[3]->name, "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(info[3]->component, "es");
    BOOST_CHECK_EQUAL(info[3]->timeIndex, 5u);
    BOOST_CHECK_EQUAL(info[3]->area.value(), "es");
}

BOOST_AUTO_TEST_CASE(hydro_level_records_area_as_component)
{
    namer.UpdateArea("it");
    namer.UpdateTimeStep(168);
    namer.HydroLevel(4);

    BOOST_REQUIRE(info[4].has_value());
    BOOST_CHECK_EQUAL(info[4]->name, "HydroLevel");
    BOOST_CHECK_EQUAL(info[4]->component, "it");
    BOOST_CHECK_EQUAL(info[4]->timeIndex, 168u);
    BOOST_CHECK_EQUAL(info[4]->area.value(), "it");
}

BOOST_AUTO_TEST_CASE(sts_injection_records_injection_name_and_storage_component)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(10);
    namer.ShortTermStorageInjection(5, "battery");

    BOOST_REQUIRE(info[5].has_value());
    BOOST_CHECK_EQUAL(info[5]->name, "Injection");
    BOOST_CHECK_EQUAL(info[5]->component, "battery");
    BOOST_CHECK_EQUAL(info[5]->timeIndex, 10u);
    BOOST_CHECK_EQUAL(info[5]->area.value(), "fr");
}

BOOST_AUTO_TEST_CASE(sts_withdrawal_records_withdrawal_name_and_storage_component)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(10);
    namer.ShortTermStorageWithdrawal(6, "battery");

    BOOST_REQUIRE(info[6].has_value());
    BOOST_CHECK_EQUAL(info[6]->name, "Withdrawal");
    BOOST_CHECK_EQUAL(info[6]->component, "battery");
    BOOST_CHECK_EQUAL(info[6]->timeIndex, 10u);
    BOOST_CHECK_EQUAL(info[6]->area.value(), "fr");
}

BOOST_AUTO_TEST_CASE(cluster_names_are_unique_within_area_via_area_field)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(1);
    namer.DispatchableProduction(0, "cluster");

    namer.UpdateArea("de");
    namer.DispatchableProduction(1, "cluster");

    BOOST_CHECK_EQUAL(info[0]->area.value(), "fr");
    BOOST_CHECK_EQUAL(info[1]->area.value(), "de");
    BOOST_CHECK_EQUAL(info[0]->component, info[1]->component);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(constraint_namer)

BOOST_AUTO_TEST_CASE(recording_namer_stores_area_balance_entry)
{
    ConstrFixture f;
    ConstraintNamer namer(f.names, f.info);
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(8);
    namer.AreaBalance(0);

    BOOST_REQUIRE(f.info[0].has_value());
    BOOST_CHECK_EQUAL(f.info[0]->name, "AreaBalance");
    BOOST_CHECK_EQUAL(f.info[0]->component, "fr");
    BOOST_CHECK_EQUAL(f.info[0]->timeIndex, 8u);
    BOOST_CHECK_EQUAL(f.info[0]->area.value(), "fr");
}

BOOST_AUTO_TEST_CASE(non_recording_namer_leaves_entry_empty)
{
    ConstrFixture f;
    ConstraintNamer namer(f.names);
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(8);
    namer.AreaBalance(0);

    BOOST_CHECK(!f.info[0].has_value());
}

BOOST_AUTO_TEST_SUITE_END()
