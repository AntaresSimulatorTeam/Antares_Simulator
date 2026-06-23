// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimization::AddLegacyExtraOutputs;
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
    //    variables, link "area2$$area3" with a direct decomposition
    //    variable but no indirect counterpart,
    //  - one unnamed slot (index 4).
    Fixture()
    {
        info.resize(11);
        info[0] = LegacyVariableInfo{"DispatchableProduction", "cluster1", 168};
        info[1] = LegacyVariableInfo{"UnsuppliedEnergy", "area1", 168};
        info[2] = LegacyVariableInfo{"Spillage", "area1", 168};
        info[3] = LegacyVariableInfo{"UnsuppliedEnergy", "area2", 168};
        info[5] = LegacyVariableInfo{"NODU", "cluster1", 168};
        info[6] = LegacyVariableInfo{"DirectFlow", "area1$$area2", 168};
        info[7] = LegacyVariableInfo{"PositiveDirectFlow", "area1$$area2", 168};
        info[8] = LegacyVariableInfo{"PositiveIndirectFlow", "area1$$area2", 168};
        info[9] = LegacyVariableInfo{"PositiveDirectFlow", "area2$$area3", 168};
        info[10] = LegacyVariableInfo{"UnsuppliedEnergy", "area3", 168};
    }

    std::vector<std::optional<LegacyVariableInfo>> info;
    std::vector<double> values = {3600., 52., 7., 13., -1., 2.3, -120., 0., 120., 10., 0.2};
    std::vector<double> costs = {35., 10000., 4., 20000., -1., 100., 0., 0.5, 0.7, 0.5, 9000.};
    // Block covering timesteps [168, 335], year 2.
    FillContext fillContext{0, 167, 168, 335, 2};
    unsigned currentBlock = 1;
    SimulationTable table;

    void fill()
    {
        AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);
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
    BOOST_CHECK_EQUAL(row->block, "1");               // currentBlock
    BOOST_CHECK_EQUAL(row->absoluteTimeIndex, "168"); // timeIndex
    BOOST_CHECK_EQUAL(row->blockTimeIndex, "0");      // first timestep of the block
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
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1$$area2");
    BOOST_CHECK_EQUAL(rows[0].value, 120.); // |-120|
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
    // + 1 actual_num_units_on + 1 abs_flow
    BOOST_CHECK_EQUAL(table.rowCount(),
                      8); // update this count when adding new outputs to the fixture
}

BOOST_AUTO_TEST_SUITE_END()
