// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/opt_rename_problem.h"

using Antares::Optimization::LegacyVariableInfo;

namespace
{
struct NamerFixture
{
    NamerFixture():
        varNamer(names, legacyInfo)
    {
    }

    void setupArea(const std::string& area, unsigned timeStep = 0)
    {
        varNamer.UpdateArea(area);
        varNamer.UpdateTimeStep(timeStep);
    }

    void setupLink(const std::string& origin, const std::string& destination, unsigned timeStep = 0)
    {
        varNamer.updateExtremities(origin, destination);
        varNamer.UpdateTimeStep(timeStep);
    }

    std::vector<std::string> names = std::vector<std::string>(20);
    std::vector<std::optional<LegacyVariableInfo>>
      legacyInfo = std::vector<std::optional<LegacyVariableInfo>>(20);
    VariableNamer varNamer;
};

struct ConstraintNamerFixture
{
    ConstraintNamerFixture():
        constrNamer(names, &legacyInfo)
    {
    }

    void setupArea(const std::string& area, unsigned timeStep = 0)
    {
        constrNamer.UpdateArea(area);
        constrNamer.UpdateTimeStep(timeStep);
    }

    void setupLink(const std::string& origin, const std::string& destination, unsigned timeStep = 0)
    {
        constrNamer.updateExtremities(origin, destination);
        constrNamer.UpdateTimeStep(timeStep);
    }

    std::vector<std::string> names = std::vector<std::string>(20);
    std::vector<std::optional<LegacyVariableInfo>>
      legacyInfo = std::vector<std::optional<LegacyVariableInfo>>(20);
    ConstraintNamer constrNamer;
};
} // namespace

BOOST_AUTO_TEST_SUITE(LegacyNamingUniqueness)

BOOST_FIXTURE_TEST_CASE(area_variable_component_is_area_node, NamerFixture)
{
    setupArea("west");
    varNamer.UnsuppliedEnergy(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_node");
}

BOOST_FIXTURE_TEST_CASE(spillage_component_matches_unsupplied_energy, NamerFixture)
{
    setupArea("west");
    varNamer.UnsuppliedEnergy(0);
    varNamer.Spillage(1);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(different_areas_produce_different_node_components, NamerFixture)
{
    setupArea("west");
    varNamer.UnsuppliedEnergy(0);
    setupArea("east");
    varNamer.UnsuppliedEnergy(1);
    BOOST_CHECK_NE(legacyInfo[0]->component, legacyInfo[1]->component);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_node");
    BOOST_CHECK_EQUAL(legacyInfo[1]->component, "east_node");
}

BOOST_FIXTURE_TEST_CASE(thermal_component_includes_area_and_cluster, NamerFixture)
{
    setupArea("west");
    varNamer.DispatchableProduction(0, "gas");
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_thermal_gas");
}

BOOST_FIXTURE_TEST_CASE(same_cluster_name_different_areas_are_unique, NamerFixture)
{
    setupArea("west");
    varNamer.DispatchableProduction(0, "gas");
    setupArea("east");
    varNamer.DispatchableProduction(1, "gas");
    BOOST_CHECK_NE(legacyInfo[0]->component, legacyInfo[1]->component);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_thermal_gas");
    BOOST_CHECK_EQUAL(legacyInfo[1]->component, "east_thermal_gas");
}

BOOST_FIXTURE_TEST_CASE(nodu_has_same_component_as_dispatachable_production, NamerFixture)
{
    setupArea("west");
    varNamer.DispatchableProduction(0, "gas");
    varNamer.NODU(1, "gas");
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(sts_component_includes_area_and_cluster, NamerFixture)
{
    setupArea("west");
    varNamer.ShortTermStorageInjection(0, "battery");
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_short_term_storage_battery");
}

BOOST_FIXTURE_TEST_CASE(same_sts_name_different_areas_are_unique, NamerFixture)
{
    setupArea("west");
    varNamer.ShortTermStorageInjection(0, "battery");
    setupArea("east");
    varNamer.ShortTermStorageInjection(1, "battery");
    BOOST_CHECK_NE(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(sts_injection_and_withdrawal_share_component, NamerFixture)
{
    setupArea("west");
    varNamer.ShortTermStorageInjection(0, "battery");
    varNamer.ShortTermStorageWithdrawal(1, "battery");
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(link_component_is_alphabetically_ordered, NamerFixture)
{
    setupLink("west", "east");
    varNamer.DirectFlow(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "east_west_link");
}

BOOST_FIXTURE_TEST_CASE(link_component_is_same_regardless_of_direction, NamerFixture)
{
    setupLink("west", "east");
    varNamer.DirectFlow(0);
    setupLink("east", "west");
    varNamer.DirectFlow(1);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(link_flow_variables_share_component, NamerFixture)
{
    setupLink("area1", "area2");
    varNamer.DirectFlow(0);
    varNamer.PositiveDirectFlow(1);
    varNamer.PositiveIndirectFlow(2);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, legacyInfo[1]->component);
    BOOST_CHECK_EQUAL(legacyInfo[1]->component, legacyInfo[2]->component);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "area1_area2_link");
}

BOOST_FIXTURE_TEST_CASE(hydro_prod_component_is_area_hydro_storage, NamerFixture)
{
    setupArea("west");
    varNamer.HydProd(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_hydro_storage");
}

BOOST_FIXTURE_TEST_CASE(all_hydro_variables_share_component, NamerFixture)
{
    setupArea("west");
    varNamer.HydProd(0);
    varNamer.HydProdDown(1);
    varNamer.HydProdUp(2);
    varNamer.Pumping(3);
    varNamer.HydroLevel(4);
    varNamer.Overflow(5);
    varNamer.FinalStorageLevel(6);
    for (unsigned i = 1; i <= 6; ++i)
    {
        BOOST_CHECK_EQUAL(legacyInfo[i]->component, "west_hydro_storage");
    }
}

BOOST_FIXTURE_TEST_CASE(hydro_component_differs_from_node_component, NamerFixture)
{
    setupArea("west");
    varNamer.UnsuppliedEnergy(0);
    varNamer.HydProd(1);
    BOOST_CHECK_NE(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_FIXTURE_TEST_CASE(area_balance_constraint_component_is_area_node, ConstraintNamerFixture)
{
    setupArea("west");
    constrNamer.AreaBalance(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_node");
}

BOOST_FIXTURE_TEST_CASE(flow_dissociation_constraint_component_is_link, ConstraintNamerFixture)
{
    setupLink("west", "east");
    constrNamer.FlowDissociation(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "east_west_link");
}

BOOST_FIXTURE_TEST_CASE(final_stock_expression_constraint_is_hydro, ConstraintNamerFixture)
{
    setupArea("west");
    constrNamer.FinalStockExpression(0);
    BOOST_CHECK_EQUAL(legacyInfo[0]->component, "west_hydro_storage");
}

BOOST_FIXTURE_TEST_CASE(thermal_and_node_components_never_collide, NamerFixture)
{
    setupArea("west");
    varNamer.UnsuppliedEnergy(0);
    varNamer.DispatchableProduction(1, "node");
    BOOST_CHECK_NE(legacyInfo[0]->component, legacyInfo[1]->component);
}

BOOST_AUTO_TEST_SUITE_END()
