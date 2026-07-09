// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimization::AddLegacyExtraOutputs;

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

std::optional<Row> FindRowAt(const SimulationTable& table,
                             const std::string& output,
                             const std::string& component,
                             const std::string& absoluteTimeIndex)
{
    for (const auto& row: RowsForOutput(table, output))
    {
        if (row.component == component && row.absoluteTimeIndex == absoluteTimeIndex)
        {
            return row;
        }
    }
    return std::nullopt;
}

// Per-hour layout of the solved problem's variables. The correspondence
// tables below and the X / CoutLineaire vectors are built from the same
// offsets, so each test reads its operands the way AddLegacyExtraOutputs
// does: by index.
enum VarOffset : int
{
    dispatchableProduction = 0, // cluster1 (area1): X 3600, cost 35
    unsuppliedArea1 = 1,        // X 52,  cost 10000
    spillageArea1 = 2,          // X 7,   cost 4
    unsuppliedArea2 = 3,        // X 13,  cost 20000
    spillageArea2 = 4,          // X 0,   cost 1
    nodu = 5,                   // cluster1: X 2.3, cost 100 (fixed cost)
    directFlowLink0 = 6,        // area1$$area2: X 120
    positiveDirectLink0 = 7,    // X 120, cost 0.5 (direct hurdle cost)
    positiveIndirectLink0 = 8,  // X 0,   cost 0.7 (indirect hurdle cost)
    unsuppliedArea3 = 9,        // X 0.2, cost 9000
    spillageArea3 = 10,         // X 0,   cost 1
    directFlowLink1 = 11,       // area2$$area3: X -30
    hydroLevelArea1 = 12,       // X 4000
    numberStarting = 13,        // cluster1: X 1, cost 5000 (startup cost)
    stsInjection = 14,          // battery1 (area2): X 40
    stsWithdrawal = 15,         // battery1 (area2): X 100
    variablesPerHour = 16
};

// Per-hour layout of the constraints, followed by one weekly
// FinalStockExpression constraint for area1 (dual 42) at index
// nbPdt * constraintsPerHour.
enum ConstraintOffset : int
{
    balanceArea1 = 0,          // dual -10000
    balanceArea2 = 1,          // dual -50
    balanceArea3 = 2,          // dual -75
    flowDissociationLink0 = 3, // dual -3
    constraintsPerHour = 4
};

struct Fixture
{
    // A minimal solved weekly problem: three areas, one thermal cluster
    // ("cluster1" in "area1", with reservoir hydro and accurate water value),
    // link 0 "area1$$area2" managed with hurdle costs, link 1 "area2$$area3"
    // without. One hour by default; the week starts at absolute hour 168
    // (weekInTheYear = 1), matching the block window of fillContext.
    explicit Fixture(int nbPdt = 1)
    {
        problem.weekInTheYear = 1;
        problem.NombreDePasDeTemps = nbPdt;
        problem.NombreDePasDeTempsPourUneOptimisation = nbPdt;
        problem.OptimisationNotFastMode = true;

        problem.NombreDePays = 3;
        problem.NomsDesPays = {"area1", "area2", "area3"};

        problem.NombreDInterconnexions = 2;
        problem.PaysOrigineDeLInterconnexion = {0, 1};
        problem.PaysExtremiteDeLInterconnexion = {1, 2};
        problem.CoutDeTransport.resize(2);
        problem.CoutDeTransport[0].IntercoGereeAvecDesCouts = true;
        problem.CoutDeTransport[1].IntercoGereeAvecDesCouts = false;

        problem.ConsommationsAbattues.resize(nbPdt);
        problem.AllMustRunGeneration.resize(nbPdt);
        problem.ValeursDeNTC.resize(nbPdt);

        problem.CaracteristiquesHydrauliques.resize(3);
        auto& hydroArea1 = problem.CaracteristiquesHydrauliques[0];
        hydroArea1.TailleReservoir = 5000.;
        hydroArea1.ApportNaturelHoraire.assign(nbPdt, 123.4);
        hydroArea1.AccurateWaterValue = true;
        problem.CaracteristiquesHydrauliques[1].AccurateWaterValue = false;
        problem.CaracteristiquesHydrauliques[2].AccurateWaterValue = false;

        problem.PaliersThermiquesDuPays.resize(3);
        auto& paliers = problem.PaliersThermiquesDuPays[0];
        paliers.NombreDePaliersThermiques = 1;
        paliers.NomsDesPaliersThermiques = {"cluster1"};
        paliers.NumeroDuPalierDansLEnsembleDesPaliersThermiques = {0};
        paliers.TailleUnitaireDUnGroupeDuPalierThermique = {900.};
        paliers.PminDuPalierThermiquePendantUneHeure = {300.};
        paliers.emissionFactors.resize(1); // value-initialized: all factors 0
        paliers.emissionFactors[0][Antares::Data::Pollutant::CO2] = 0.5;
        paliers.emissionFactors[0][Antares::Data::Pollutant::NOX] = 0.01;
        paliers.emissionFactors[0][Antares::Data::Pollutant::OP5] = 2.;
        paliers.PuissanceDisponibleEtCout.resize(1);
        paliers.PuissanceDisponibleEtCout[0].PuissanceDisponibleDuPalierThermique.assign(nbPdt,
                                                                                         4000.);
        paliers.PuissanceDisponibleEtCout[0].PuissanceMinDuPalierThermique.assign(nbPdt, 500.);
        problem.PaliersThermiquesDuPays[1].NombreDePaliersThermiques = 0;
        problem.PaliersThermiquesDuPays[2].NombreDePaliersThermiques = 0;

        // One short-term storage, "battery1" in area2.
        problem.ShortTermStorage.resize(3);
        auto& storage = problem.ShortTermStorage[1].emplace_back();
        storage.name = "battery1";
        storage.clusterGlobalIndex = 0;

        // Input-only generation series for area1: aggregated wind and one
        // misc-gen entry, as filled by SIM_RenseignementProblemeHebdo.
        problem.InputGenerationOfArea.resize(3);
        problem.InputGenerationOfArea[0].push_back(
          {.componentName = "area1_wind", .availablePower = std::vector<double>(nbPdt, 320.)});
        problem.InputGenerationOfArea[0].push_back(
          {.componentName = "area1_combined_heat_power",
           .availablePower = std::vector<double>(nbPdt, 12.5)});

        problem.CorrespondanceVarNativesVarOptim.resize(nbPdt);
        problem.CorrespondanceCntNativesCntOptim.resize(nbPdt);
        auto& solved = *problem.ProblemeAResoudre;
        for (int pdt = 0; pdt < nbPdt; ++pdt)
        {
            problem.ConsommationsAbattues[pdt].ConsommationAbattueDuPays = {790., 500., 280.};
            problem.AllMustRunGeneration[pdt].AllMustRunGenerationOfArea = {10., 0., 20.};
            auto& ntc = problem.ValeursDeNTC[pdt];
            ntc.ValeurDeNTCOrigineVersExtremite = {200., 200.};
            ntc.ValeurDeNTCExtremiteVersOrigine = {200., 200.};
            ntc.ValeurDeLoopFlowOrigineVersExtremite = {15., -8.};

            const int base = pdt * variablesPerHour;
            auto& vars = problem.CorrespondanceVarNativesVarOptim[pdt];
            vars.NumeroDeVariableDuPalierThermique = {base + dispatchableProduction};
            vars.NumeroDeVariableDefaillancePositive = {base + unsuppliedArea1,
                                                        base + unsuppliedArea2,
                                                        base + unsuppliedArea3};
            vars.NumeroDeVariableDefaillanceNegative = {base + spillageArea1,
                                                        base + spillageArea2,
                                                        base + spillageArea3};
            vars.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique = {base + nodu};
            vars.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique = {
              base + numberStarting};
            vars.NumeroDeVariableDuFluxDirect = {base + directFlowLink0, base + directFlowLink1};
            vars.NumeroDeVariableDuFluxDirectPositif = {base + positiveDirectLink0, -1};
            vars.NumeroDeVariableDuFluxIndirectPositif = {base + positiveIndirectLink0, -1};
            // Only area1 manages a reservoir; -1 is the "no variable" sentinel
            // written by the construction site.
            vars.NumeroDeVariablesDeNiveau = {base + hydroLevelArea1, -1, -1};
            vars.SIM_ShortTermStorage.InjectionVariable = {base + stsInjection};
            vars.SIM_ShortTermStorage.WithdrawalVariable = {base + stsWithdrawal};

            const int cntBase = pdt * constraintsPerHour;
            auto& constraints = problem.CorrespondanceCntNativesCntOptim[pdt];
            constraints.NumeroDeContrainteDesBilansPays = {cntBase + balanceArea1,
                                                           cntBase + balanceArea2,
                                                           cntBase + balanceArea3};
            constraints.NumeroDeContrainteDeDissociationDeFlux = {cntBase + flowDissociationLink0,
                                                                  -1};

            solved.X.insert(
              solved.X.end(),
              {3600., 52., 7., 13., 0., 2.3, 120., 120., 0., 0.2, 0., -30., 4000., 1., 40., 100.});
            solved.CoutLineaire.insert(
              solved.CoutLineaire.end(),
              {35., 10000., 4., 20000., 1., 100., 0., 0.5, 0.7, 9000., 1., 0., 0., 5000., 0., 0.});
            solved.CoutsMarginauxDesContraintes.insert(solved.CoutsMarginauxDesContraintes.end(),
                                                       {-10000., -50., -75., -3.});
        }
        problem.NumeroDeContrainteExpressionStockFinal = {nbPdt * constraintsPerHour, 0, 0};
        solved.CoutsMarginauxDesContraintes.push_back(42.);

        // Two weekly LayerStorage variables for area1 (accurate water value),
        // appended after the hourly variables: X 500 / 250, cost -20 / -10
        // (the construction site writes -WaterLayerValues as the coefficient).
        const int firstLayerVariable = nbPdt * variablesPerHour;
        problem.NumeroDeVariableDeTrancheDeStock = {
          {firstLayerVariable, firstLayerVariable + 1},
          {},
          {}};
        solved.X.insert(solved.X.end(), {500., 250.});
        solved.CoutLineaire.insert(solved.CoutLineaire.end(), {-20., -10.});

        solved.NombreDeVariables = nbPdt * variablesPerHour + 2;
        solved.NombreDeContraintes = nbPdt * constraintsPerHour + 1;
    }

    void fill()
    {
        AddLegacyExtraOutputs(table, problem, fillContext, currentBlock);
    }

    PROBLEME_HEBDO problem;
    // Block covering timesteps [168, 335], year 2.
    FillContext fillContext{0, 167, 168, 335, 2};
    unsigned currentBlock = 1;
    SimulationTable table;
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
    BOOST_CHECK_EQUAL(row->block, "1");               // currentBlock (0-indexed, same as raw rows)
    BOOST_CHECK_EQUAL(row->absoluteTimeIndex, "168"); // weekInTheYear * 168 + pdt
    BOOST_CHECK_EQUAL(row->blockTimeIndex, "0");      // first timestep of the block
    BOOST_CHECK_EQUAL(row->scenarioIndex, "2");
    BOOST_CHECK_EQUAL(row->basisStatus, "None");
}

BOOST_AUTO_TEST_CASE(imbalance_cost_combines_unsupplied_and_spilled_energy)
{
    fill();

    const auto row = FindRow(table, "imbalance_cost", "area1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 4. * 7. + 10000. * 52., 1e-9);
}

BOOST_AUTO_TEST_CASE(is_significant_loss_of_load_is_one_above_threshold_and_zero_below)
{
    fill();

    const auto rows = RowsForOutput(table, "is_significant_loss_of_load");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area1")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area2")->value, 1.);
    // 0.2 MW of unsupplied energy is below the 0.5 MW threshold.
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_significant_loss_of_load_is_zero_exactly_at_threshold)
{
    // The threshold check is strict (> 0.5), so exactly 0.5 MW must not count
    // as significant loss of load.
    problem.ProblemeAResoudre->X[unsuppliedArea3] = 0.5;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_one_for_any_positive_unsupplied_energy)
{
    fill();

    const auto rows = RowsForOutput(table, "is_loss_of_load");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area1")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area2")->value, 1.);
    // Strict > 0: even 0.2 MW below the significance threshold counts.
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3")->value, 1.);
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_zero_without_unsupplied_energy)
{
    problem.ProblemeAResoudre->X[unsuppliedArea3] = 0.;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(actual_load_is_the_residual_load_plus_must_run_generation)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area1")->value, 790. + 10.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area2")->value, 500. + 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area3")->value, 280. + 20.);
}

BOOST_AUTO_TEST_CASE(price_is_minus_the_area_balance_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "price");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area1")->value, 10000.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area2")->value, 50.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area3")->value, 75.);
}

BOOST_AUTO_TEST_CASE(is_near_loss_of_load_compares_price_to_unsupplied_cost)
{
    fill();

    // area1: price 10000 > 10000 - 5; area2: 50 <= 20000 - 5; area3: 75 <= 9000 - 5.
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area1")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area2")->value, 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(level_percentage_is_hydro_level_over_reservoir_capacity)
{
    fill();

    const auto rows = RowsForOutput(table, "level_percentage");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_CLOSE(rows[0].value, 4000. / 5000. * 100., 1e-9);
}

BOOST_AUTO_TEST_CASE(level_percentage_is_skipped_when_capacity_is_non_positive)
{
    problem.CaracteristiquesHydrauliques[0].TailleReservoir = 0.;
    fill();

    BOOST_CHECK(RowsForOutput(table, "level_percentage").empty());
    // The inflows do not depend on the reservoir capacity.
    BOOST_CHECK(!RowsForOutput(table, "actual_inflows").empty());
}

BOOST_AUTO_TEST_CASE(hydro_outputs_are_skipped_without_a_hydro_level_variable)
{
    // -1 is the sentinel written by the construction site for areas whose
    // reservoir is not managed.
    problem.CorrespondanceVarNativesVarOptim[0].NumeroDeVariablesDeNiveau[0] = -1;
    fill();

    BOOST_CHECK(RowsForOutput(table, "level_percentage").empty());
    BOOST_CHECK(RowsForOutput(table, "actual_inflows").empty());
}

BOOST_AUTO_TEST_CASE(actual_inflows_is_the_rounded_inflow_series)
{
    fill();

    const auto rows = RowsForOutput(table, "actual_inflows");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_EQUAL(rows[0].value, 123.); // round(123.4)
}

BOOST_AUTO_TEST_CASE(actual_inflows_is_skipped_without_an_inflow_series)
{
    problem.CaracteristiquesHydrauliques[0].ApportNaturelHoraire.clear();
    fill();

    BOOST_CHECK(RowsForOutput(table, "actual_inflows").empty());
}

BOOST_AUTO_TEST_CASE(abs_flow_is_absolute_value_of_signed_flow)
{
    fill();

    const auto rows = RowsForOutput(table, "abs_flow");
    BOOST_REQUIRE_EQUAL(rows.size(), 2);
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area1$$area2")->value, 120.);
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area2$$area3")->value, 30.); // |-30|
}

BOOST_AUTO_TEST_CASE(minus_flow_is_the_negated_signed_flow)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area1$$area2")->value, -120.);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area2$$area3")->value, 30.); // -(-30)
}

BOOST_AUTO_TEST_CASE(actual_loop_flow_reads_the_link_loop_flow_series)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area1$$area2")->value, 15.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area2$$area3")->value, -8.);
}

BOOST_AUTO_TEST_CASE(is_directly_congested_is_one_at_capacity_and_zero_below)
{
    // Link 0 carries flow 120 (direct direction), link 1 carries -30 (indirect
    // direction). With direct capacities 120 and 100 respectively, only link 0
    // is saturated in the direct direction.
    problem.ValeursDeNTC[0].ValeurDeNTCOrigineVersExtremite = {120., 100.};
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area1$$area2")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area2$$area3")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_indirectly_congested_compares_minus_flow_to_indirect_capacity)
{
    // Link 1's flow is -30: it saturates the indirect direction when the
    // indirect capacity is also 30. Link 0's flow is +120: it never saturates
    // the indirect direction.
    problem.ValeursDeNTC[0].ValeurDeNTCExtremiteVersOrigine = {200., 30.};
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area1$$area2")->value, 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area2$$area3")->value, 1.);
}

BOOST_AUTO_TEST_CASE(congestion_fees_use_the_endpoint_area_prices)
{
    // Link 0: flow 120, price_in = area1 = 10000, price_out = area2 = 50,
    // delta = -9950. Link 1: flow -30, delta = 75 - 50 = 25.
    fill();

    BOOST_CHECK_CLOSE(FindRow(table, "abs_congestion_fee", "area1$$area2")->value,
                      120. * 9950.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "alg_congestion_fee", "area1$$area2")->value,
                      120. * -9950.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "abs_congestion_fee", "area2$$area3")->value, 30. * 25., 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "alg_congestion_fee", "area2$$area3")->value,
                      -30. * 25.,
                      1e-9);
}

BOOST_AUTO_TEST_CASE(link_prop_cost_sums_direct_and_indirect_hurdle_costs)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "area1$$area2");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 0.5 * 120. + 0.7 * 0., 1e-9);
}

BOOST_AUTO_TEST_CASE(capacity_shadow_price_is_the_absolute_flow_dissociation_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "capacity_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1$$area2");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // |-3|
}

BOOST_AUTO_TEST_CASE(hurdle_cost_outputs_are_skipped_for_links_without_hurdle_costs)
{
    fill();

    // Link 1 is not managed with hurdle costs: no flow decomposition
    // variables, no FlowDissociation constraint.
    BOOST_CHECK(!FindRow(table, "prop_cost", "area2$$area3").has_value());
    BOOST_CHECK(!FindRow(table, "capacity_shadow_price", "area2$$area3").has_value());
}

BOOST_AUTO_TEST_CASE(hydro_shadow_price_is_the_final_stock_expression_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "hydro_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_EQUAL(rows[0].value, 42.);
    // Anchored on the last hour of the interval.
    BOOST_CHECK_EQUAL(rows[0].absoluteTimeIndex, "168");
}

BOOST_AUTO_TEST_CASE(hydro_shadow_price_is_skipped_without_accurate_water_value)
{
    problem.CaracteristiquesHydrauliques[0].AccurateWaterValue = false;
    fill();

    BOOST_CHECK(RowsForOutput(table, "hydro_shadow_price").empty());
}

BOOST_AUTO_TEST_CASE(bellman_value_sums_the_layer_costs_times_layer_storages)
{
    fill();

    const auto rows = RowsForOutput(table, "bellman_value");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    // -20 * 500 + -10 * 250, the objective coefficients being -WaterLayerValues.
    BOOST_CHECK_EQUAL(rows[0].value, -12500.);
    // Anchored on the last hour of the interval, like hydro_shadow_price.
    BOOST_CHECK_EQUAL(rows[0].absoluteTimeIndex, "168");
}

BOOST_AUTO_TEST_CASE(bellman_value_is_skipped_without_accurate_water_value)
{
    problem.CaracteristiquesHydrauliques[0].AccurateWaterValue = false;
    fill();

    BOOST_CHECK(RowsForOutput(table, "bellman_value").empty());
}

BOOST_AUTO_TEST_CASE(input_generation_emits_power_and_its_negation_per_component)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "generation_power", "area1_wind")->value, 320.);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_generation", "area1_wind")->value, -320.);
    BOOST_CHECK_EQUAL(FindRow(table, "generation_power", "area1_combined_heat_power")->value,
                      12.5);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_generation", "area1_combined_heat_power")->value,
                      -12.5);
    // Areas without input series produce no rows.
    BOOST_CHECK_EQUAL(RowsForOutput(table, "generation_power").size(), 2);
}

BOOST_AUTO_TEST_CASE(input_generation_is_skipped_when_the_series_are_absent)
{
    problem.InputGenerationOfArea.clear();
    fill();

    BOOST_CHECK(RowsForOutput(table, "generation_power").empty());
    BOOST_CHECK(RowsForOutput(table, "minus_generation").empty());
}

BOOST_AUTO_TEST_CASE(storage_profit_is_net_withdrawal_times_area_price)
{
    fill();

    // battery1 is in area2 (price 50): floor((100 - 40) * 50 + 0.5) = 3000.
    const auto row = FindRow(table, "profit", "battery1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->value, 3000.);
}

BOOST_AUTO_TEST_CASE(storage_profit_is_rounded_to_the_nearest_integer)
{
    // (100.01 - 40) * 50 = 3000.5, rounded up by the floor(x + 0.5) formula.
    problem.ProblemeAResoudre->X[stsWithdrawal] = 100.01;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "profit", "battery1")->value, 3001.);
}

BOOST_AUTO_TEST_CASE(storage_profit_is_negative_when_injecting_at_positive_price)
{
    problem.ProblemeAResoudre->X[stsWithdrawal] = 10.;
    fill();

    // floor((10 - 40) * 50 + 0.5) = -1500.
    BOOST_CHECK_EQUAL(FindRow(table, "profit", "battery1")->value, -1500.);
}

BOOST_AUTO_TEST_CASE(emissions_are_generation_power_times_each_factor)
{
    fill();

    // generation_power = 3600.
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
    fill();

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
    BOOST_CHECK_EQUAL(emissionRows,
                      static_cast<std::size_t>(Antares::Data::Pollutant::POLLUTANT_MAX));
}

BOOST_AUTO_TEST_CASE(thermal_margins_are_derived_from_availability_and_generation)
{
    // availability=4000, unitSize=900, minStablePower=300, minGenPower=500,
    // generation_power = 3600.
    // cluster_availability = max(4000, 300*ceil(4000/900)=300*5=1500) = 4000.
    fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 4000., 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "up_margin", "cluster1")->value, 4000. - 3600., 1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "min_gen_power", "cluster1")->value,
                      500.,
                      1e-9); // min(3600, 500)
    BOOST_CHECK_CLOSE(FindRow(table, "down_margin", "cluster1")->value, 3600. - 500., 1e-9);
}

BOOST_AUTO_TEST_CASE(cluster_availability_takes_the_unit_floor_when_it_dominates)
{
    // availability=250, unitSize=100, minStablePower=200:
    // cluster_availability = max(250, 200*ceil(250/100)=200*3=600) = 600.
    auto& paliers = problem.PaliersThermiquesDuPays[0];
    paliers.PuissanceDisponibleEtCout[0].PuissanceDisponibleDuPalierThermique = {250.};
    paliers.TailleUnitaireDUnGroupeDuPalierThermique = {100.};
    paliers.PminDuPalierThermiquePendantUneHeure = {200.};
    fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 600., 1e-9);
}

BOOST_AUTO_TEST_CASE(cluster_availability_ignores_the_unit_floor_when_unit_size_is_zero)
{
    // A zero unit size would divide by zero in the floor term; it is treated as
    // no floor, so cluster_availability is just the availability.
    auto& paliers = problem.PaliersThermiquesDuPays[0];
    paliers.PuissanceDisponibleEtCout[0].PuissanceDisponibleDuPalierThermique = {250.};
    paliers.TailleUnitaireDUnGroupeDuPalierThermique = {0.};
    paliers.PminDuPalierThermiquePendantUneHeure = {200.};
    fill();

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "cluster1")->value, 250., 1e-9);
}

BOOST_AUTO_TEST_CASE(profit_is_margin_price_times_generation_above_the_min_gen_floor)
{
    // area1 price = -(-10000) = 10000; generation_cost = 35;
    // generation_power = 3600; min_gen_power floor = 500.
    // profit = (10000 - 35) * max(3600 - 500, 0) = 9965 * 3100.
    fill();

    const auto row = FindRow(table, "profit", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 9965. * 3100., 1e-9);
}

BOOST_AUTO_TEST_CASE(profit_is_zero_when_generation_does_not_exceed_the_floor)
{
    // A floor above the generation clamps the dispatchable quantity to zero.
    problem.PaliersThermiquesDuPays[0].PuissanceDisponibleEtCout[0].PuissanceMinDuPalierThermique
      = {5000.};
    fill();

    const auto row = FindRow(table, "profit", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->value, 0.);
}

BOOST_AUTO_TEST_CASE(actual_num_units_on_is_ceil_of_nodu)
{
    fill();

    const auto rows = RowsForOutput(table, "actual_num_units_on");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "cluster1");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // ceil(2.3)
}

BOOST_AUTO_TEST_CASE(non_prop_cost_has_no_startup_term_at_the_first_hour)
{
    // At the first hour of the interval the previous NODU is not part of this
    // solution, so the start-up term is dropped:
    // non_prop_cost = fixed_cost * ceil(NODU) = 100 * 3.
    fill();

    const auto row = FindRow(table, "non_prop_cost", "cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 100. * 3., 1e-9);
}

BOOST_AUTO_TEST_CASE(unit_commitment_outputs_are_skipped_in_fast_mode)
{
    // The NODU variables only exist in "not fast" mode.
    problem.OptimisationNotFastMode = false;
    fill();

    BOOST_CHECK(RowsForOutput(table, "actual_num_units_on").empty());
    BOOST_CHECK(RowsForOutput(table, "non_prop_cost").empty());
}

BOOST_AUTO_TEST_CASE(no_other_rows_are_emitted)
{
    fill();

    // Areas: 3 x (imbalance_cost, is_significant_loss_of_load,
    //             is_loss_of_load, actual_load, price,
    //             is_near_loss_of_load) + area1's level_percentage and
    //             actual_inflows                                  = 20
    // Thermal: prop_cost + 13 emissions + 4 margins + profit
    //          + actual_num_units_on + non_prop_cost              = 21
    // Links: link 0 (abs_flow, minus_flow, actual_loop_flow, 2 congestion
    //        indicators, 2 congestion fees, prop_cost,
    //        capacity_shadow_price) = 9; link 1 without the hurdle-cost
    //        outputs = 7                                          = 16
    // Short-term storage: battery1's profit                       = 1
    // Input generation: 2 area1 components x (generation_power,
    //                    minus_generation)                         = 4
    // Weekly: area1's hydro_shadow_price and bellman_value        = 2
    BOOST_CHECK_EQUAL(table.rowCount(), 20 + 21 + 16 + 1 + 4 + 2);
}

BOOST_AUTO_TEST_SUITE_END()

// Two-hour problem: the second hour exercises the cross-hour startup term of
// non_prop_cost.
BOOST_AUTO_TEST_CASE(non_prop_cost_adds_startup_cost_for_units_started_since_t_minus_one)
{
    Fixture fixture(/*nbPdt=*/2);
    // Hour 0: ceil(2.3) = 3 units on. Hour 1: ceil(4.2) = 5 units on, so 2
    // units started, each at startup cost 5000; fixed cost 100 per unit on.
    fixture.problem.ProblemeAResoudre->X[variablesPerHour + nodu] = 4.2;
    fixture.fill();

    const auto row = FindRowAt(fixture.table, "non_prop_cost", "cluster1", "169");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 5000. * 2. + 100. * 5., 1e-9);
}
