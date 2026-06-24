// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimization::AddLegacyExtraOutputs;
using Antares::Optimization::LegacyExtraOutputsContext;
using Antares::Optimization::LegacyVariableInfo;

namespace
{
// Rows as produced by SimulationTable::storageIntoRows(), column order:
// block, component, output, absolute_time_index, block_time_index,
// scenario_index, value, basis_status.
struct Row
{
    explicit Row(const std::vector<std::string>& columns):
        block(columns[0]),
        component(columns[1]),
        output(columns[2]),
        absoluteTimeIndex(columns[3]),
        blockTimeIndex(columns[4]),
        scenarioIndex(columns[5]),
        value(std::stod(columns[6])),
        basisStatus(columns[7])
    {
    }

    std::string block;
    std::string component;
    std::string output;
    std::string absoluteTimeIndex;
    std::string blockTimeIndex;
    std::string scenarioIndex;
    double value;
    std::string basisStatus;
};

std::vector<Row> RowsForOutput(const SimulationTable& table, const std::string& output)
{
    std::vector<Row> rows;
    for (const auto& columns: table.storageIntoRows())
    {
        if (columns[2] == output)
        {
            rows.emplace_back(columns);
        }
    }
    return rows;
}

std::optional<Row> FindRow(const SimulationTable& table,
                           const std::string& output,
                           const std::string& component)
{
    for (const auto& row: RowsForOutput(table, output))
    {
        if (row.component == component)
        {
            return row;
        }
    }
    return std::nullopt;
}

struct Fixture
{
    // A small solved problem with, at timestep 168:
    //  - one thermal cluster (generation + NODU variables),
    //  - "area1" with both UnsuppliedEnergy and Spillage, "area2" with
    //    UnsuppliedEnergy only, "area3" below the loss-of-load threshold,
    //  - link "area1$$area2" with flow and hurdle-cost decomposition
    //    variables, link "area2$$area3" with a (negative) flow and a direct
    //    decomposition variable but no indirect counterpart,
    //  - one unnamed slot (index 4),
    //  - HydroLevel for "area1".
    Fixture()
    {
        info.resize(13);
        // "cluster1" lives in "area1"; the area qualifier disambiguates the
        // cluster name (not unique across areas) when keying study data.
        info[0] = LegacyVariableInfo{"DispatchableProduction", "cluster1", 168, "area1"};
        info[1] = LegacyVariableInfo{"UnsuppliedEnergy", "area1", 168};
        info[2] = LegacyVariableInfo{"Spillage", "area1", 168};
        info[3] = LegacyVariableInfo{"UnsuppliedEnergy", "area2", 168};
        info[5] = LegacyVariableInfo{"NODU", "cluster1", 168};
        info[6] = LegacyVariableInfo{"DirectFlow", "area1$$area2", 168};
        info[7] = LegacyVariableInfo{"PositiveDirectFlow", "area1$$area2", 168};
        info[8] = LegacyVariableInfo{"PositiveIndirectFlow", "area1$$area2", 168};
        info[9] = LegacyVariableInfo{"PositiveDirectFlow", "area2$$area3", 168};
        info[10] = LegacyVariableInfo{"UnsuppliedEnergy", "area3", 168};
        info[11] = LegacyVariableInfo{"DirectFlow", "area2$$area3", 168};
        info[12] = LegacyVariableInfo{"HydroLevel", "area1", 168};

        // The week's first hour matches the block anchor (168), so the link
        // congestion indicators read hour-in-week 0 of the capacity vectors.
        context.weekFirstTimeStep = 168;
    }

    std::vector<std::optional<LegacyVariableInfo>> info;
    std::vector<double>
      values = {3600., 52., 7., 13., -1., 2.3, 120., 0., 120., 10., 0.2, -30., 4000.};
    std::vector<double>
      costs = {35., 10000., 4., 20000., -1., 100., 0., 0.5, 0.7, 0.5, 9000., 0., 0.};

    // Constraints: balance constraints for "area1" (at loss of load: the
    // stored dual is minus the price), "area2" (cheap marginal unit) and
    // "area4" (no UnsuppliedEnergy variable recorded), the flow dissociation
    // constraint of link "area1$$area2", the final stock expression of
    // "area1", and one unnamed slot. "area3" has no balance constraint.
    Fixture& withConstraints()
    {
        constraintsInfo.resize(6);
        constraintsInfo[0] = LegacyVariableInfo{"AreaBalance", "area1", 168};
        constraintsInfo[1] = LegacyVariableInfo{"AreaBalance", "area2", 168};
        constraintsInfo[2] = LegacyVariableInfo{"AreaBalance", "area4", 168};
        constraintsInfo[3] = LegacyVariableInfo{"FlowDissociation", "area1$$area2", 168};
        constraintsInfo[5] = LegacyVariableInfo{"FinalStockExpression", "area1", 168};
        duals = {-10000., -50., -75., -3., -1., 42.};
        return *this;
    }

    // Reservoir capacity for "area1" only, so HydroLevel for any other area
    // is correctly skipped.
    Fixture& withReservoirs()
    {
        context.reservoirCapacityByArea["area1"] = 5000.;
        return *this;
    }

    // Per-pdt link capacities. The fixture uses a single recorded pdt (the
    // anchor is at globalFirst), so a 1-entry vector is enough.
    Fixture& withLinkCapacities(double directArea1Area2 = 200.,
                                double indirectArea1Area2 = 200.,
                                double directArea2Area3 = 200.,
                                double indirectArea2Area3 = 200.)
    {
        context.directCapacityByLink["area1$$area2"] = {directArea1Area2};
        context.indirectCapacityByLink["area1$$area2"] = {indirectArea1Area2};
        context.directCapacityByLink["area2$$area3"] = {directArea2Area3};
        context.indirectCapacityByLink["area2$$area3"] = {indirectArea2Area3};
        return *this;
    }

    // Per-area residual load, single recorded pdt like the link capacities.
    Fixture& withLoads(double area1 = 800., double area2 = 500., double area3 = 300.)
    {
        context.loadByArea["area1"] = {area1};
        context.loadByArea["area2"] = {area2};
        context.loadByArea["area3"] = {area3};
        return *this;
    }

    // Per-area inflows; only "area1" carries a series (the only HydroLevel
    // anchor in the fixture).
    Fixture& withInflows(double area1 = 123.4)
    {
        context.inflowsByArea["area1"] = {area1};
        return *this;
    }

    // Per-link loop flow, single recorded pdt.
    Fixture& withLoopFlows(double area1Area2 = 15., double area2Area3 = -8.)
    {
        context.loopFlowByLink["area1$$area2"] = {area1Area2};
        context.loopFlowByLink["area2$$area3"] = {area2Area3};
        return *this;
    }

    // Emission factors for "cluster1" (keyed "area1$$cluster1"). A few distinct
    // pollutants are set to non-zero so the ordinal-to-row mapping is checked.
    Fixture& withEmissionFactors(double co2 = 0.5, double nox = 0.01, double op5 = 2.)
    {
        std::array<double, Antares::Data::Pollutant::POLLUTANT_MAX> factors{};
        factors[Antares::Data::Pollutant::CO2] = co2;
        factors[Antares::Data::Pollutant::NOX] = nox;
        factors[Antares::Data::Pollutant::OP5] = op5;
        context.emissionFactorsByCluster["area1$$cluster1"] = factors;
        return *this;
    }

    // Margin data for "cluster1" (keyed "area1$$cluster1"), single recorded pdt.
    Fixture& withThermalMargin(double availability = 4000.,
                               double unitSize = 900.,
                               double minStablePower = 300.,
                               double minGenPower = 500.)
    {
        LegacyExtraOutputsContext::ThermalMarginData margin;
        margin.unitSize = unitSize;
        margin.minStablePower = minStablePower;
        margin.availability = {availability};
        margin.minGenPower = {minGenPower};
        context.thermalMarginByCluster["area1$$cluster1"] = margin;
        return *this;
    }

    std::vector<std::optional<LegacyVariableInfo>> constraintsInfo;
    std::vector<double> duals;
    LegacyExtraOutputsContext context;
    // Block covering timesteps [168, 335], year 2.
    FillContext fillContext{0, 167, 168, 335, 2};
    unsigned currentBlock = 1;
    SimulationTable table;

    void fill()
    {
        AddLegacyExtraOutputs(table,
                              info,
                              values,
                              costs,
                              constraintsInfo,
                              duals,
                              context,
                              fillContext,
                              currentBlock);
    }
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(test_legacy_extra_outputs, Fixture)

BOOST_AUTO_TEST_CASE(thermal_prop_cost_is_generation_cost_times_generation_power)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 35. * 3600., 1e-9);
}

BOOST_AUTO_TEST_CASE(extra_output_entries_carry_block_time_and_scenario)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->block, "2");               // currentBlock + 1
    BOOST_CHECK_EQUAL(row->absoluteTimeIndex, "169"); // timeIndex + 1
    BOOST_CHECK_EQUAL(row->blockTimeIndex, "1");      // first timestep of the block
    BOOST_CHECK_EQUAL(row->scenarioIndex, "2");
    BOOST_CHECK_EQUAL(row->basisStatus, "None");
}

BOOST_AUTO_TEST_CASE(imbalance_cost_combines_unsupplied_and_spilled_energy)
{
    fill();

    const auto rows = RowsForOutput(table, "imbalance_cost");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_CLOSE(rows[0].value, 10000. * 52. + 4. * 7., 1e-9);
}

BOOST_AUTO_TEST_CASE(imbalance_cost_is_skipped_when_spillage_is_missing)
{
    fill();

    // "area2" has UnsuppliedEnergy but no Spillage variable: no entry.
    BOOST_CHECK(!FindRow(table, "imbalance_cost", "area2").has_value());
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_one_above_threshold_and_zero_below)
{
    fill();

    const auto rows = RowsForOutput(table, "is_loss_of_load");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area1")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area2")->value, 1.);
    // 0.2 MW of unsupplied energy is below the 0.5 MW threshold.
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_zero_exactly_at_threshold)
{
    // area3's UnsuppliedEnergy is at index 10; the threshold check is strict
    // (> 0.5), so exactly 0.5 MW must not count as loss of load.
    values[10] = 0.5;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(actual_num_units_on_is_ceil_of_nodu)
{
    fill();

    const auto rows = RowsForOutput(table, "actual_num_units_on");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "cluster1");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // ceil(2.3)
}

BOOST_AUTO_TEST_CASE(abs_flow_is_absolute_value_of_signed_flow)
{
    fill();

    const auto rows = RowsForOutput(table, "abs_flow");
    BOOST_REQUIRE_EQUAL(rows.size(), 2);
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area1$$area2")->value, 120.);
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area2$$area3")->value, 30.); // |-30|
}

BOOST_AUTO_TEST_CASE(link_prop_cost_sums_direct_and_indirect_hurdle_costs)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "area1$$area2");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 0.5 * 0. + 0.7 * 120., 1e-9);
}

BOOST_AUTO_TEST_CASE(link_prop_cost_is_skipped_when_indirect_flow_is_missing)
{
    fill();

    // "area2$$area3" has PositiveDirectFlow but no PositiveIndirectFlow.
    BOOST_CHECK(!FindRow(table, "prop_cost", "area2$$area3").has_value());
}

BOOST_AUTO_TEST_CASE(no_other_rows_are_emitted)
{
    fill();

    // 2 prop_cost (cluster1, link) + 1 imbalance_cost + 3 is_loss_of_load
    // + 1 actual_num_units_on + 1 non_prop_cost + 2 abs_flow + 2 minus_flow.
    // The context is empty, so level_percentage, is_*_congested, actual_load,
    // actual_inflows and actual_loop_flow are skipped.
    BOOST_CHECK_EQUAL(table.rowCount(), 12);
}

BOOST_AUTO_TEST_CASE(level_percentage_is_hydro_level_over_reservoir_capacity)
{
    withReservoirs().fill();

    const auto rows = RowsForOutput(table, "level_percentage");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_CLOSE(rows[0].value, 4000. / 5000. * 100., 1e-9);
}

BOOST_AUTO_TEST_CASE(level_percentage_is_skipped_without_a_known_capacity)
{
    // Default context has no reservoir for "area1": the HydroLevel anchor
    // emits nothing.
    fill();
    BOOST_CHECK(RowsForOutput(table, "level_percentage").empty());
}

BOOST_AUTO_TEST_CASE(level_percentage_is_skipped_when_capacity_is_non_positive)
{
    context.reservoirCapacityByArea["area1"] = 0.;
    fill();
    BOOST_CHECK(RowsForOutput(table, "level_percentage").empty());
}

BOOST_AUTO_TEST_CASE(is_directly_congested_is_one_at_capacity_and_zero_below)
{
    // area1$$area2 carries flow 120 (direct direction), area2$$area3 carries -30
    // (indirect direction). With direct capacities 120 and 100 respectively,
    // only the first link is saturated in the direct direction.
    withLinkCapacities(/*directArea1Area2=*/120.,
                       /*indirectArea1Area2=*/200.,
                       /*directArea2Area3=*/100.,
                       /*indirectArea2Area3=*/200.)
      .fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area1$$area2")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area2$$area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_indirectly_congested_compares_minus_flow_to_indirect_capacity)
{
    // area2$$area3's flow is -30: it saturates the indirect direction when
    // the indirect capacity is also 30. area1$$area2's flow is +120: it never
    // saturates the indirect direction.
    withLinkCapacities(/*directArea1Area2=*/200.,
                       /*indirectArea1Area2=*/200.,
                       /*directArea2Area3=*/200.,
                       /*indirectArea2Area3=*/30.)
      .fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area1$$area2")->value, 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area2$$area3")->value, 1.);
}

BOOST_AUTO_TEST_CASE(congestion_indicators_are_skipped_without_a_known_capacity)
{
    // Default context has no link capacities: every DirectFlow anchor goes
    // unindexed and the congestion rows are skipped.
    fill();
    BOOST_CHECK(RowsForOutput(table, "is_directly_congested").empty());
    BOOST_CHECK(RowsForOutput(table, "is_indirectly_congested").empty());
}

BOOST_AUTO_TEST_CASE(price_is_minus_the_area_balance_dual)
{
    withConstraints().fill();

    const auto rows = RowsForOutput(table, "price");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area1")->value, 10000.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area2")->value, 50.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area4")->value, 75.);
}

BOOST_AUTO_TEST_CASE(is_near_loss_of_load_compares_price_to_unsupplied_cost)
{
    withConstraints().fill();

    // area1: price 10000 > 10000 - 5; area2: price 50 <= 20000 - 5.
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area1")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area2")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_near_loss_of_load_is_skipped_without_unsupplied_variable)
{
    withConstraints().fill();

    // "area4" has a balance constraint but no UnsuppliedEnergy variable, so
    // its unsupplied energy cost is unknown: price only, no nearness flag.
    BOOST_CHECK_EQUAL(RowsForOutput(table, "is_near_loss_of_load").size(), 2);
    BOOST_CHECK(!FindRow(table, "is_near_loss_of_load", "area4").has_value());
}

BOOST_AUTO_TEST_CASE(capacity_shadow_price_is_the_absolute_flow_dissociation_dual)
{
    withConstraints().fill();

    const auto rows = RowsForOutput(table, "capacity_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1$$area2");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // |-3|
}

BOOST_AUTO_TEST_CASE(hydro_shadow_price_is_the_final_stock_expression_dual)
{
    withConstraints().fill();

    const auto rows = RowsForOutput(table, "hydro_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_EQUAL(rows[0].value, 42.);
}

BOOST_AUTO_TEST_CASE(other_constraints_produce_no_extra_output)
{
    withConstraints().fill();

    // 12 variable-driven rows + 3 price + 2 is_near_loss_of_load
    // + 1 capacity_shadow_price + 1 hydro_shadow_price + 2 congestion fees
    // (abs/alg on area1$$area2, whose endpoints both have a price); the unnamed
    // slot adds nothing. Thermal profit and other context-driven outputs are
    // skipped here because no study data was injected.
    BOOST_CHECK_EQUAL(table.rowCount(), 21);
}

BOOST_AUTO_TEST_CASE(study_data_outputs_add_one_level_percentage_and_four_congestion_rows)
{
    withConstraints().withReservoirs().withLinkCapacities().fill();

    // The 21 rows of other_constraints_produce_no_extra_output (which already
    // include the 2 congestion fees), plus 1 level_percentage (area1) and 4
    // congestion-indicator rows (2 directions for each of the 2 links). load,
    // inflow and loop-flow series are still absent, so those outputs stay
    // skipped.
    BOOST_CHECK_EQUAL(table.rowCount(), 21 + 1 + 4);
}

BOOST_AUTO_TEST_CASE(minus_flow_is_the_negated_signed_flow)
{
    fill();

    const auto rows = RowsForOutput(table, "minus_flow");
    BOOST_REQUIRE_EQUAL(rows.size(), 2);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area1$$area2")->value, -120.);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area2$$area3")->value, 30.); // -(-30)
}

BOOST_AUTO_TEST_CASE(non_prop_cost_is_fixed_cost_times_units_without_a_previous_hour)
{
    // The fixture has no previous-hour NODU and no starting-units variable, so
    // the start-up term is zero: non_prop_cost = fixed_cost * ceil(NODU).
    fill();

    const auto row = FindRow(table, "non_prop_cost", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 100. * 3., 1e-9); // costs[5] * ceil(2.3)
}

BOOST_AUTO_TEST_CASE(non_prop_cost_adds_startup_cost_for_units_started_since_t_minus_one)
{
    // Append a previous-hour NODU (2 units on) and the cluster's starting-units
    // variable carrying the startup cost as its objective coefficient.
    info.push_back(LegacyVariableInfo{"NODU", "cluster1", 167});
    values.push_back(2.);
    costs.push_back(0.);
    info.push_back(LegacyVariableInfo{"NumberStartingDispatchableUnits", "cluster1", 168});
    values.push_back(1.);
    costs.push_back(5000.);

    fill();

    // 3 units on now, 2 last hour: 1 unit started.
    // non_prop_cost = 5000 * 1 + 100 * 3.
    const auto row = FindRow(table, "non_prop_cost", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 5000. * 1. + 100. * 3., 1e-9);
}

BOOST_AUTO_TEST_CASE(actual_load_reads_the_area_load_series)
{
    withLoads().fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area1")->value, 800.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area2")->value, 500.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area3")->value, 300.);
}

BOOST_AUTO_TEST_CASE(actual_load_is_skipped_without_a_load_series)
{
    fill();
    BOOST_CHECK(RowsForOutput(table, "actual_load").empty());
}

BOOST_AUTO_TEST_CASE(actual_inflows_is_the_rounded_inflow_series)
{
    withInflows(/*area1=*/123.4).fill();

    const auto rows = RowsForOutput(table, "actual_inflows");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_EQUAL(rows[0].value, 123.); // round(123.4)
}

BOOST_AUTO_TEST_CASE(actual_inflows_is_skipped_without_an_inflow_series)
{
    fill();
    BOOST_CHECK(RowsForOutput(table, "actual_inflows").empty());
}

BOOST_AUTO_TEST_CASE(actual_loop_flow_reads_the_link_loop_flow_series)
{
    withLoopFlows(/*area1Area2=*/15., /*area2Area3=*/-8.).fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area1$$area2")->value, 15.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area2$$area3")->value, -8.);
}

BOOST_AUTO_TEST_CASE(actual_loop_flow_is_skipped_without_a_loop_flow_series)
{
    fill();
    BOOST_CHECK(RowsForOutput(table, "actual_loop_flow").empty());
}

BOOST_AUTO_TEST_CASE(emissions_are_generation_power_times_each_factor)
{
    withEmissionFactors(/*co2=*/0.5, /*nox=*/0.01, /*op5=*/2.).fill();

    // generation_power = values[0] = 3600.
    BOOST_CHECK_CLOSE(FindRow(table, "co2_emissions", "cluster1")->value, 3600. * 0.5, 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "nox_emissions", "cluster1")->value, 3600. * 0.01, 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "op5_emissions", "cluster1")->value, 3600. * 2., 1e-9);
    // A pollutant with a zero factor still gets a row, valued 0.
    const auto so2 = FindRow(table, "so2_emissions", "cluster1");
    BOOST_REQUIRE(so2.has_value());
    BOOST_CHECK_EQUAL(so2->value, 0.);
}

BOOST_AUTO_TEST_CASE(emissions_emit_one_row_per_pollutant)
{
    withEmissionFactors().fill();

    // One row per pollutant in Pollutant::PollutantEnum, all on "cluster1".
    BOOST_CHECK_EQUAL(RowsForOutput(table, "co2_emissions").size(), 1);
    std::size_t emissionRows = 0;
    for (const auto& columns: table.storageIntoRows())
    {
        if (columns[2].size() > 10 && columns[2].substr(columns[2].size() - 10) == "_emissions")
        {
            ++emissionRows;
        }
    }
    BOOST_CHECK_EQUAL(emissionRows, Antares::Data::Pollutant::POLLUTANT_MAX);
}

BOOST_AUTO_TEST_CASE(emissions_are_skipped_for_clusters_absent_from_the_context)
{
    // Default context carries no emission factors: the DispatchableProduction
    // anchor emits prop_cost but no emission rows.
    fill();
    BOOST_CHECK(RowsForOutput(table, "co2_emissions").empty());
}

BOOST_AUTO_TEST_CASE(emissions_use_the_area_qualified_key)
{
    // Same cluster name but a different area: the key "area2$$cluster1" does not
    // match the anchor's "area1$$cluster1", so nothing is emitted.
    std::array<double, Antares::Data::Pollutant::POLLUTANT_MAX> factors{};
    factors[Antares::Data::Pollutant::CO2] = 1.;
    context.emissionFactorsByCluster["area2$$cluster1"] = factors;
    fill();
    BOOST_CHECK(RowsForOutput(table, "co2_emissions").empty());
}

BOOST_AUTO_TEST_CASE(thermal_margins_are_derived_from_availability_and_generation)
{
    // availability=4000, unitSize=900, minStablePower=300, minGenPower=500.
    // generation_power = values[0] = 3600.
    // cluster_availability = max(4000, 300*ceil(4000/900)=300*5=1500) = 4000.
    withThermalMargin(4000., 900., 300., 500.).fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 4000., 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "up_margin", "cluster1")->value, 4000. - 3600., 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "min_gen_power", "cluster1")->value, 500., 1e-9); // min(3600,500)
    BOOST_CHECK_CLOSE(FindRow(table, "down_margin", "cluster1")->value, 3600. - 500., 1e-9);
}

BOOST_AUTO_TEST_CASE(cluster_availability_takes_the_unit_floor_when_it_dominates)
{
    // availability=250, unitSize=100, minStablePower=200:
    // cluster_availability = max(250, 200*ceil(250/100)=200*3=600) = 600.
    withThermalMargin(/*availability=*/250., /*unitSize=*/100., /*minStablePower=*/200., 0.).fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 600., 1e-9);
}

BOOST_AUTO_TEST_CASE(cluster_availability_ignores_the_unit_floor_when_unit_size_is_zero)
{
    // A zero unit size would divide by zero in the floor term; it is treated as
    // no floor, so cluster_availability is just the availability.
    withThermalMargin(/*availability=*/250., /*unitSize=*/0., /*minStablePower=*/200., 0.).fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 250., 1e-9);
}

BOOST_AUTO_TEST_CASE(thermal_margins_are_skipped_for_clusters_absent_from_the_context)
{
    fill();
    BOOST_CHECK(RowsForOutput(table, "cluster_availability").empty());
    BOOST_CHECK(RowsForOutput(table, "up_margin").empty());
    BOOST_CHECK(RowsForOutput(table, "min_gen_power").empty());
    BOOST_CHECK(RowsForOutput(table, "down_margin").empty());
}

BOOST_AUTO_TEST_CASE(profit_is_margin_price_times_generation_above_the_min_gen_floor)
{
    // area1 price = -(-10000) = 10000; generation_cost = costs[0] = 35;
    // generation_power = values[0] = 3600; min_gen_power floor = 500.
    // profit = (10000 - 35) * max(3600 - 500, 0) = 9965 * 3100.
    withConstraints().withThermalMargin(4000., 900., 300., /*minGenPower=*/500.).fill();

    const auto row = FindRow(table, "profit", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 9965. * 3100., 1e-9);
}

BOOST_AUTO_TEST_CASE(profit_is_zero_when_generation_does_not_exceed_the_floor)
{
    // A floor above the generation clamps the dispatchable quantity to zero.
    withConstraints().withThermalMargin(4000., 900., 300., /*minGenPower=*/5000.).fill();

    const auto row = FindRow(table, "profit", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->value, 0.);
}

BOOST_AUTO_TEST_CASE(profit_is_skipped_without_an_area_price)
{
    // No constraints recorded: the area-price map is empty, so profit cannot be
    // computed even though the margin context is present.
    withThermalMargin().fill();
    BOOST_CHECK(RowsForOutput(table, "profit").empty());
}

BOOST_AUTO_TEST_CASE(profit_is_skipped_without_the_thermal_margin_floor)
{
    // The price is known but the cluster is absent from the margin context, so
    // the min_gen_power floor is unknown and profit is skipped.
    withConstraints().fill();
    BOOST_CHECK(RowsForOutput(table, "profit").empty());
}

BOOST_AUTO_TEST_CASE(congestion_fees_use_the_endpoint_area_prices)
{
    // Link area1$$area2, flow = values[6] = 120. price_in = area1 = 10000,
    // price_out = area2 = 50, delta = 50 - 10000 = -9950.
    withConstraints().fill();

    BOOST_CHECK_CLOSE(FindRow(table, "abs_congestion_fee", "area1$$area2")->value,
                      120. * 9950.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "alg_congestion_fee", "area1$$area2")->value,
                      120. * -9950.,
                      1e-9);
}

BOOST_AUTO_TEST_CASE(congestion_fees_are_skipped_when_an_endpoint_has_no_price)
{
    // Link area2$$area3: area3 has no balance constraint, so its price is
    // unknown and both congestion fees are skipped for that link.
    withConstraints().fill();

    BOOST_CHECK(!FindRow(table, "abs_congestion_fee", "area2$$area3").has_value());
    BOOST_CHECK(!FindRow(table, "alg_congestion_fee", "area2$$area3").has_value());
}

BOOST_AUTO_TEST_CASE(congestion_fees_are_skipped_without_any_prices)
{
    // No constraints: the area-price map is empty, so no link gets a fee.
    fill();
    BOOST_CHECK(RowsForOutput(table, "abs_congestion_fee").empty());
    BOOST_CHECK(RowsForOutput(table, "alg_congestion_fee").empty());
}

BOOST_AUTO_TEST_SUITE_END()
