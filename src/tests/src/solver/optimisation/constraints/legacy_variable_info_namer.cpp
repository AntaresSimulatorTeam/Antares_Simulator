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
// the raw rows of the legacy simulation table.

namespace
{
struct VarFixture
{
    std::vector<std::string> names = std::vector<std::string>(8);
    std::vector<std::optional<LegacyVariableInfo>>
      info = std::vector<std::optional<LegacyVariableInfo>>(8);
    VariableNamer namer{names, info};
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(variable_namer, VarFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_records_name_and_cluster_component)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(42);
    namer.DispatchableProduction(0, "gas_cluster");

    BOOST_REQUIRE(info[0].has_value());
    BOOST_CHECK_EQUAL(info[0]->name, "DispatchableProduction");
    BOOST_CHECK_EQUAL(info[0]->component, "fr_thermal_gas_cluster");
    BOOST_CHECK_EQUAL(info[0]->timeIndex, 42u);
}

BOOST_AUTO_TEST_CASE(nodu_records_cluster_name)
{
    namer.UpdateArea("de");
    namer.UpdateTimeStep(100);
    namer.NODU(1, "coal_cluster");

    BOOST_REQUIRE(info[1].has_value());
    BOOST_CHECK_EQUAL(info[1]->name, "NODU");
    BOOST_CHECK_EQUAL(info[1]->component, "de_thermal_coal_cluster");
    BOOST_CHECK_EQUAL(info[1]->timeIndex, 100u);
}

BOOST_AUTO_TEST_CASE(direct_flow_records_origin_destination_as_component)
{
    namer.updateExtremities("fr", "de");
    namer.UpdateTimeStep(7);
    namer.DirectFlow(2);

    BOOST_REQUIRE(info[2].has_value());
    BOOST_CHECK_EQUAL(info[2]->name, "DirectFlow");
    BOOST_CHECK_EQUAL(info[2]->component, "de_fr_link");
    BOOST_CHECK_EQUAL(info[2]->timeIndex, 7u);
}

BOOST_AUTO_TEST_CASE(unsupplied_energy_records_area_as_component)
{
    namer.UpdateArea("es");
    namer.UpdateTimeStep(5);
    namer.UnsuppliedEnergy(3);

    BOOST_REQUIRE(info[3].has_value());
    BOOST_CHECK_EQUAL(info[3]->name, "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(info[3]->component, "es_node");
    BOOST_CHECK_EQUAL(info[3]->timeIndex, 5u);
}

BOOST_AUTO_TEST_CASE(hydro_level_records_area_as_component)
{
    namer.UpdateArea("it");
    namer.UpdateTimeStep(168);
    namer.HydroLevel(4);

    BOOST_REQUIRE(info[4].has_value());
    BOOST_CHECK_EQUAL(info[4]->name, "HydroLevel");
    BOOST_CHECK_EQUAL(info[4]->component, "it_hydro_storage");
    BOOST_CHECK_EQUAL(info[4]->timeIndex, 168u);
}

BOOST_AUTO_TEST_CASE(layer_storage_records_final_storage_layer_name_with_index)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(42);
    namer.LayerStorage(7, 0);

    BOOST_REQUIRE(info[7].has_value());
    BOOST_CHECK_EQUAL(info[7]->name, "final_storage_layer_0");
    BOOST_CHECK_EQUAL(info[7]->component, "fr_hydro_storage");
    BOOST_CHECK_EQUAL(info[7]->timeIndex, 42u);
    BOOST_CHECK_EQUAL(names[7], "LayerStorage::area<fr>::Layer<0>::hour<42>");
}

BOOST_AUTO_TEST_CASE(sts_injection_records_injection_name_and_storage_component)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(10);
    namer.ShortTermStorageInjection(5, "battery");

    BOOST_REQUIRE(info[5].has_value());
    BOOST_CHECK_EQUAL(info[5]->name, "Injection");
    BOOST_CHECK_EQUAL(info[5]->component, "fr_short_term_storage_battery");
    BOOST_CHECK_EQUAL(info[5]->timeIndex, 10u);
}

BOOST_AUTO_TEST_CASE(sts_withdrawal_records_withdrawal_name_and_storage_component)
{
    namer.UpdateArea("fr");
    namer.UpdateTimeStep(10);
    namer.ShortTermStorageWithdrawal(6, "battery");

    BOOST_REQUIRE(info[6].has_value());
    BOOST_CHECK_EQUAL(info[6]->name, "Withdrawal");
    BOOST_CHECK_EQUAL(info[6]->component, "fr_short_term_storage_battery");
    BOOST_CHECK_EQUAL(info[6]->timeIndex, 10u);
}

BOOST_AUTO_TEST_CASE(constraint_namer_records_nothing)
{
    std::vector<std::string> names(4);
    ConstraintNamer constraintNamer(names);
    constraintNamer.UpdateArea("fr");
    constraintNamer.UpdateTimeStep(8);
    constraintNamer.AreaBalance(0);

    BOOST_CHECK(!names[0].empty());
}

BOOST_AUTO_TEST_SUITE_END()
