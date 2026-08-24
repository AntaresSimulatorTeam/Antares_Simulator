// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/InactiveComponentsAnalyzer.h"
#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::LinearProblem::Api::FillContext;
using Antares::Optimization::AddLegacyExtraOutputs;
using Antares::Optimization::DumpSimulationTableAfterPostProcess;
using Antares::Optimization::LegacyVariableInfo;
using Antares::Optimization::LegacyWeeklyBlock;

namespace
{
// Column order: block, component, output, absolute_time_index,
// block_time_index, scenario_index, value, basis_status.
struct Row
{
    explicit Row(const SimulationTable& table, size_t rowIndex):
        block(table.columns()[0]->toString(rowIndex)),
        component(table.columns()[1]->toString(rowIndex)),
        output(table.columns()[2]->toString(rowIndex)),
        absoluteTimeIndex(table.columns()[3]->toString(rowIndex)),
        blockTimeIndex(table.columns()[4]->toString(rowIndex)),
        scenarioIndex(table.columns()[5]->toString(rowIndex)),
        value(std::stod(table.columns()[6]->toString(rowIndex))),
        basisStatus(table.columns()[7]->toString(rowIndex))
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
    const auto& cols = table.columns();
    for (size_t i = 0; i < table.rowCount(); ++i)
    {
        if (cols[2]->toString(i) == output)
        {
            rows.emplace_back(table, i);
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
    dispatchableProduction = 0, // cluster1 (area1): X 3600, cost 35.0005
                                // (user market bid 35 + noise)
    unsuppliedArea1 = 1,        // X 52,  cost 10000.0005 (user cost 10000 + noise)
    spillageArea1 = 2,          // X 7,   cost 4.0005 (user cost 4 + noise)
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
    hydProdArea1 = 16,          // X 700
    pumpingArea1 = 17,          // X 100
    variablesPerHour = 18
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

        // User-provided imbalance costs; the CoutLineaire entries for the
        // unsupplied/spillage variables below are deliberately different
        // (they carry the optimisation noise).
        problem.CoutDeDefaillancePositiveSansBruit = {10000., 10000., 10000.};
        problem.CoutDeDefaillanceNegativeSansBruit = {4., 4., 4.};

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
        // User-provided market bid cost; the CoutLineaire entry for the
        // production variable below is deliberately different (it carries the
        // thermal noise).
        paliers.PuissanceDisponibleEtCout[0].PuissanceDisponibleDuPalierThermique.assign(nbPdt,
                                                                                         4000.);
        paliers.PuissanceDisponibleEtCout[0].PuissanceMinDuPalierThermique.assign(nbPdt, 500.);
        paliers.PuissanceDisponibleEtCout[0]
          .CoutHoraireDeProductionDuPalierThermiqueSansBruit.assign(nbPdt, 30.);
        paliers.PuissanceDisponibleEtCout[0].CoutMarginalDeProductionDuPalierThermique.assign(nbPdt,
                                                                                              40.);
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

        problem.CoutDeDefaillancePositiveSansBruit = {9500., 19000., 8500.};
        problem.CoutDeDefaillanceNegativeSansBruit = {3., 0.5, 0.5};

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
            vars.NumeroDeVariablesDeLaProdHyd = {base + hydProdArea1, -1, -1};
            vars.NumeroDeVariablesDePompage = {base + pumpingArea1, -1, -1};
            vars.SIM_ShortTermStorage.InjectionVariable = {base + stsInjection};
            vars.SIM_ShortTermStorage.WithdrawalVariable = {base + stsWithdrawal};

            const int cntBase = pdt * constraintsPerHour;
            auto& constraints = problem.CorrespondanceCntNativesCntOptim[pdt];
            constraints.NumeroDeContrainteDesBilansPays = {cntBase + balanceArea1,
                                                           cntBase + balanceArea2,
                                                           cntBase + balanceArea3};
            constraints.NumeroDeContrainteDeDissociationDeFlux = {cntBase + flowDissociationLink0,
                                                                  -1};

            solved.X.insert(solved.X.end(),
                            {3600.,
                             52.,
                             7.,
                             13.,
                             0.,
                             2.3,
                             120.,
                             120.,
                             0.,
                             0.2,
                             0.,
                             -30.,
                             4000.,
                             1.,
                             40.,
                             100.,
                             700.,
                             100.});
            solved.CoutLineaire.insert(solved.CoutLineaire.end(),
                                       {35.0005,
                                        10000.0005,
                                        4.0005,
                                        20000.,
                                        1.,
                                        100.,
                                        0.,
                                        0.5,
                                        0.7,
                                        9000.,
                                        1.,
                                        0.,
                                        0.,
                                        5000.,
                                        0.,
                                        0.,
                                        0.,
                                        0.});
            solved.CoutsMarginauxDesContraintes.insert(solved.CoutsMarginauxDesContraintes.end(),
                                                       {-10000., -50., -75., -3.});
        }
        problem.NumeroDeContrainteExpressionStockFinal = {nbPdt * constraintsPerHour, 0, 0};
        solved.CoutsMarginauxDesContraintes.push_back(42.);

        // Two weekly LayerStorage variables for area1 (accurate water value),
        // appended after the hourly variables: X 500 / 250, cost -20 / -10
        // (the construction site writes -WaterLayerValues as the coefficient).
        const int firstLayerVariable = nbPdt * variablesPerHour;
        problem.NumeroDeVariableDeTrancheDeStock = {{firstLayerVariable, firstLayerVariable + 1},
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

BOOST_AUTO_TEST_CASE(thermal_prop_cost_is_market_bid_cost_times_generation_power)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "area1_thermal_cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 40. * 3600., 1e-9);
}

BOOST_AUTO_TEST_CASE(extra_output_entries_carry_block_time_and_scenario)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "area1_thermal_cluster1");
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

    const auto rows = RowsForOutput(table, "imbalance_cost");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(rows[0].component, "area1_node");
    BOOST_CHECK_CLOSE(rows[0].value, 9500. * 52. + 3. * 7., 1e-9);
}

BOOST_AUTO_TEST_CASE(is_significant_loss_of_load_is_one_above_threshold_and_zero_below)
{
    fill();

    const auto rows = RowsForOutput(table, "is_significant_loss_of_load");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area1_node")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area2_node")->value, 1.);
    // 0.2 MW of unsupplied energy is below the 0.5 MW threshold.
    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area3_node")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_significant_loss_of_load_is_zero_exactly_at_threshold)
{
    // The threshold check is strict (> 0.5), so exactly 0.5 MW must not count
    // as significant loss of load.
    problem.ProblemeAResoudre->X[unsuppliedArea3] = 0.5;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_significant_loss_of_load", "area3_node")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_one_for_any_positive_unsupplied_energy)
{
    fill();

    const auto rows = RowsForOutput(table, "is_loss_of_load");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area1_node")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area2_node")->value, 1.);
    // Strict > 0: even 0.2 MW below the significance threshold counts.
    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3_node")->value, 1.);
}

BOOST_AUTO_TEST_CASE(is_loss_of_load_is_zero_without_unsupplied_energy)
{
    problem.ProblemeAResoudre->X[unsuppliedArea3] = 0.;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_loss_of_load", "area3_node")->value, 0.);
}

BOOST_AUTO_TEST_CASE(actual_load_is_the_residual_load_plus_must_run_generation)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area1_load")->value, 790. + 10.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area2_load")->value, 500. + 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_load", "area3_load")->value, 280. + 20.);
}

BOOST_AUTO_TEST_CASE(price_is_minus_the_area_balance_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "price");
    BOOST_REQUIRE_EQUAL(rows.size(), 3);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area1_node")->value, 10000.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area2_node")->value, 50.);
    BOOST_CHECK_EQUAL(FindRow(table, "price", "area3_node")->value, 75.);
}

BOOST_AUTO_TEST_CASE(is_near_loss_of_load_compares_price_to_unsupplied_cost)
{
    fill();

    // area1: price 10000 > 9500 - 5; area2: 50 <= 19000 - 5; area3: 75 <= 8500 - 5.
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area1_node")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area2_node")->value, 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_near_loss_of_load", "area3_node")->value, 0.);
}

BOOST_AUTO_TEST_CASE(level_percentage_is_hydro_level_over_reservoir_capacity)
{
    fill();

    const auto rows = RowsForOutput(table, "level_percentage");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1_hydro_storage");
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
    BOOST_CHECK_EQUAL(rows[0].component, "area1_hydro_storage");
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
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area1_area2_link")->value, 120.);
    BOOST_CHECK_EQUAL(FindRow(table, "abs_flow", "area2_area3_link")->value, 30.); // |-30|
}

BOOST_AUTO_TEST_CASE(minus_flow_is_the_negated_signed_flow)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area1_area2_link")->value, -120.);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_flow", "area2_area3_link")->value, 30.); // -(-30)
}

BOOST_AUTO_TEST_CASE(actual_loop_flow_reads_the_link_loop_flow_series)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area1_area2_link")->value, 15.);
    BOOST_CHECK_EQUAL(FindRow(table, "actual_loop_flow", "area2_area3_link")->value, -8.);
}

BOOST_AUTO_TEST_CASE(is_directly_congested_is_one_at_capacity_and_zero_below)
{
    // Link 0 carries flow 120 (direct direction), link 1 carries -30 (indirect
    // direction). With direct capacities 120 and 100 respectively, only link 0
    // is saturated in the direct direction.
    problem.ValeursDeNTC[0].ValeurDeNTCOrigineVersExtremite = {120., 100.};
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area1_area2_link")->value, 1.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_directly_congested", "area2_area3_link")->value, 0.);
}

BOOST_AUTO_TEST_CASE(is_indirectly_congested_compares_minus_flow_to_indirect_capacity)
{
    // Link 1's flow is -30: it saturates the indirect direction when the
    // indirect capacity is also 30. Link 0's flow is +120: it never saturates
    // the indirect direction.
    problem.ValeursDeNTC[0].ValeurDeNTCExtremiteVersOrigine = {200., 30.};
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area1_area2_link")->value, 0.);
    BOOST_CHECK_EQUAL(FindRow(table, "is_indirectly_congested", "area2_area3_link")->value, 1.);
}

BOOST_AUTO_TEST_CASE(congestion_fees_use_the_endpoint_area_prices)
{
    // Link 0: flow 120, price_in = area1 = 10000, price_out = area2 = 50,
    // delta = -9950. Link 1: flow -30, delta = 75 - 50 = 25.
    fill();

    BOOST_CHECK_CLOSE(FindRow(table, "abs_congestion_fee", "area1_area2_link")->value,
                      120. * 9950.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "alg_congestion_fee", "area1_area2_link")->value,
                      120. * -9950.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "abs_congestion_fee", "area2_area3_link")->value,
                      30. * 25.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "alg_congestion_fee", "area2_area3_link")->value,
                      -30. * 25.,
                      1e-9);
}

BOOST_AUTO_TEST_CASE(link_prop_cost_sums_direct_and_indirect_hurdle_costs)
{
    fill();

    const auto row = FindRow(table, "prop_cost", "area1_area2_link");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 0.5 * 120. + 0.7 * 0., 1e-9);
}

BOOST_AUTO_TEST_CASE(is_near_loss_of_load_is_skipped_without_unsupplied_variable)
{
    fill();

    // "area4" has a balance constraint but no UnsuppliedEnergy variable, so
    // its unsupplied energy cost is unknown: price only, no nearness flag.
    BOOST_CHECK_EQUAL(RowsForOutput(table, "is_near_loss_of_load").size(), 3);
    BOOST_CHECK(!FindRow(table, "is_near_loss_of_load", "area4_node").has_value());
}

BOOST_AUTO_TEST_CASE(capacity_shadow_price_is_the_absolute_flow_dissociation_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "capacity_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1_area2_link");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // |-3|
}

BOOST_AUTO_TEST_CASE(hurdle_cost_outputs_are_skipped_for_links_without_hurdle_costs)
{
    fill();

    // Link 1 is not managed with hurdle costs: no flow decomposition
    // variables, no FlowDissociation constraint.
    BOOST_CHECK(!FindRow(table, "prop_cost", "area2_area3_link").has_value());
    BOOST_CHECK(!FindRow(table, "capacity_shadow_price", "area2_area3_link").has_value());
}

// --- Suppression of the entire row set for a link flagged all-zero (both
// NTC directions zero across the whole study, point 3) by the precomputed
// analyzer.

BOOST_AUTO_TEST_CASE(link_outputs_are_entirely_suppressed_when_flagged_all_zero)
{
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setLinkAllZero(0, true); // area1_area2_link
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "abs_flow", "area1_area2_link").has_value());
    BOOST_CHECK(!FindRow(table, "minus_flow", "area1_area2_link").has_value());
    BOOST_CHECK(!FindRow(table, "out_port.flow", "area1_area2_link").has_value());
    BOOST_CHECK(!FindRow(table, "in_port.flow", "area1_area2_link").has_value());
    BOOST_CHECK(!FindRow(table, "prop_cost", "area1_area2_link").has_value());
    BOOST_CHECK(!FindRow(table, "capacity_shadow_price", "area1_area2_link").has_value());
    // The other link (index 1) is unaffected.
    BOOST_CHECK(FindRow(table, "abs_flow", "area2_area3_link").has_value());
}

BOOST_AUTO_TEST_CASE(link_outputs_are_kept_when_the_all_zero_flag_is_false)
{
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setLinkAllZero(0, false);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(FindRow(table, "abs_flow", "area1_area2_link").has_value());
}

BOOST_AUTO_TEST_CASE(hydro_shadow_price_is_the_final_stock_expression_dual)
{
    fill();

    const auto rows = RowsForOutput(table, "hydro_shadow_price");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1_hydro_storage");
    BOOST_CHECK_EQUAL(rows[0].value, -42.);
    // Anchored on the last hour of the interval.
    BOOST_CHECK_EQUAL(rows[0].absoluteTimeIndex, "168");
}

BOOST_AUTO_TEST_CASE(hydro_shadow_price_is_skipped_without_accurate_water_value)
{
    problem.CaracteristiquesHydrauliques[0].AccurateWaterValue = false;
    fill();

    BOOST_CHECK(RowsForOutput(table, "hydro_shadow_price").empty());
}

BOOST_AUTO_TEST_CASE(bellman_value_is_the_negated_layer_costs_times_layer_storages)
{
    fill();

    const auto rows = RowsForOutput(table, "bellman_value");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    // -(-20 * 500 + -10 * 250) = 12500, the objective coefficients being
    // -WaterLayerValues.
    BOOST_CHECK_EQUAL(rows[0].value, 12500.);
    // Anchored on the last hour of the interval, like hydro_shadow_price.
    BOOST_CHECK_EQUAL(rows[0].absoluteTimeIndex, "168");
}

BOOST_AUTO_TEST_CASE(bellman_value_is_skipped_without_accurate_water_value)
{
    problem.CaracteristiquesHydrauliques[0].AccurateWaterValue = false;
    fill();

    BOOST_CHECK(RowsForOutput(table, "bellman_value").empty());
}

BOOST_AUTO_TEST_CASE(port_field_balance_price_mirrors_the_area_price)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.price", "area1_node")->value, 10000.);
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.price", "area2_node")->value, 50.);
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.price", "area3_node")->value, 75.);
}

BOOST_AUTO_TEST_CASE(port_field_load_flow_is_minus_the_raw_load)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_load")->value, -(790. + 10.));
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area2_load")->value, -(500. + 0.));
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area3_load")->value, -(280. + 20.));
}

BOOST_AUTO_TEST_CASE(port_field_hydro_flow_is_generation_minus_pumping)
{
    fill();

    // Only area1 has hydro production variables: 700 generated, 100 pumped.
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_hydro_storage")->value, 600.);
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area2_hydro_storage").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area3_hydro_storage").has_value());
}

BOOST_AUTO_TEST_CASE(port_field_hydro_flow_ignores_pumping_when_absent)
{
    for (auto& vars: problem.CorrespondanceVarNativesVarOptim)
    {
        vars.NumeroDeVariablesDePompage[0] = -1;
    }
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_hydro_storage")->value, 700.);
}

// --- Suppression of the hydro balance_port.flow row when the reservoir is
// unmanaged AND its inflow series is entirely zero (point 2). Unlike
// level_percentage/actual_inflows (guarded by the HydroLevel variable, i.e.
// reservoirManagement alone), this row is anchored on HydProd, which exists
// independently of reservoir management: suppressing it needs both
// conditions, since a non-managed reservoir can still have legitimate
// turbine generation.

BOOST_AUTO_TEST_CASE(
  hydro_balance_port_flow_is_suppressed_when_reservoir_unmanaged_and_inflow_all_zero)
{
    problem.CaracteristiquesHydrauliques[0].SuiviNiveauHoraire = false;
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setHydroInflowAllZero(0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_hydro_storage").has_value());
}

BOOST_AUTO_TEST_CASE(hydro_balance_port_flow_is_kept_when_reservoir_unmanaged_but_inflow_nonzero)
{
    problem.CaracteristiquesHydrauliques[0].SuiviNiveauHoraire = false;
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setHydroInflowAllZero(0, false);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(FindRow(table, "balance_port.flow", "area1_hydro_storage").has_value());
}

BOOST_AUTO_TEST_CASE(hydro_balance_port_flow_is_kept_when_reservoir_managed_even_if_inflow_all_zero)
{
    problem.CaracteristiquesHydrauliques[0].SuiviNiveauHoraire = true;
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setHydroInflowAllZero(0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(FindRow(table, "balance_port.flow", "area1_hydro_storage").has_value());
}

BOOST_AUTO_TEST_CASE(port_field_link_flows_are_the_signed_flow_and_its_negation)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "out_port.flow", "area1_area2_link")->value, 120.);
    BOOST_CHECK_EQUAL(FindRow(table, "in_port.flow", "area1_area2_link")->value, -120.);
    BOOST_CHECK_EQUAL(FindRow(table, "out_port.flow", "area2_area3_link")->value, -30.);
    BOOST_CHECK_EQUAL(FindRow(table, "in_port.flow", "area2_area3_link")->value, 30.);
}

BOOST_AUTO_TEST_CASE(port_field_flows_cover_thermal_storage_and_input_generation)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_thermal_cluster1")->value, 3600.);
    BOOST_CHECK_EQUAL(
      FindRow(table, "balance_port.flow", "area2_short_term_storage_battery1")->value,
      100. - 40.);
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_wind")->value, 320.);
    BOOST_CHECK_EQUAL(FindRow(table, "balance_port.flow", "area1_combined_heat_power")->value,
                      12.5);
}

BOOST_AUTO_TEST_CASE(input_generation_emits_power_and_its_negation_per_component)
{
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "generation_power", "area1_wind")->value, 320.);
    BOOST_CHECK_EQUAL(FindRow(table, "minus_generation", "area1_wind")->value, -320.);
    BOOST_CHECK_EQUAL(FindRow(table, "generation_power", "area1_combined_heat_power")->value, 12.5);
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

// --- Suppression of rows for structurally inactive load / ROR / solar /
// wind / misc-gen components, driven by a study-wide, once-only precomputed
// InactiveComponentsAnalyzer (see docs/architecture/legacy-extra-outputs.md).
// The analyzer is null by default in this fixture: every test above keeps
// emitting every row unaffected, which is the regression this suite locks
// in for callers/fixtures that never set `problem.inactiveComponents`.

BOOST_AUTO_TEST_CASE(actual_load_and_load_balance_port_are_skipped_when_load_series_is_all_zero)
{
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setLoadAllZero(0, true); // area1
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "actual_load", "area1_load").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_load").has_value());
    // Only area1 is flagged: area2/area3 are unaffected.
    BOOST_CHECK(FindRow(table, "actual_load", "area2_load").has_value());
    BOOST_CHECK(FindRow(table, "actual_load", "area3_load").has_value());
}

BOOST_AUTO_TEST_CASE(actual_load_is_kept_when_the_all_zero_flag_is_false)
{
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setLoadAllZero(0, false); // explicit, same as the default
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(FindRow(table, "actual_load", "area1_load").has_value());
    BOOST_CHECK(FindRow(table, "balance_port.flow", "area1_load").has_value());
}

BOOST_AUTO_TEST_CASE(ror_generation_rows_are_skipped_when_ror_series_is_all_zero)
{
    problem.InputGenerationOfArea[0].push_back(
      {.componentName = "area1_run_of_river", .availablePower = {0.}});
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setRorAllZero(0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "generation_power", "area1_run_of_river").has_value());
    BOOST_CHECK(!FindRow(table, "minus_generation", "area1_run_of_river").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_run_of_river").has_value());
    // Unrelated components in the same area are unaffected.
    BOOST_CHECK(FindRow(table, "balance_port.flow", "area1_wind").has_value());
}

BOOST_AUTO_TEST_CASE(solar_generation_rows_are_skipped_when_solar_series_is_all_zero)
{
    problem.InputGenerationOfArea[0].push_back(
      {.componentName = "area1_solar", .availablePower = {0.}});
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setSolarAllZero(0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "generation_power", "area1_solar").has_value());
    BOOST_CHECK(!FindRow(table, "minus_generation", "area1_solar").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_solar").has_value());
}

BOOST_AUTO_TEST_CASE(wind_generation_rows_are_skipped_when_wind_series_is_all_zero)
{
    // The fixture already carries an "area1_wind" input-generation entry.
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setWindAllZero(0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "generation_power", "area1_wind").has_value());
    BOOST_CHECK(!FindRow(table, "minus_generation", "area1_wind").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_wind").has_value());
    // The sibling misc-gen entry ("area1_combined_heat_power") is unaffected.
    BOOST_CHECK(FindRow(table, "generation_power", "area1_combined_heat_power").has_value());
}

BOOST_AUTO_TEST_CASE(misc_gen_component_is_skipped_when_its_own_column_series_is_all_zero)
{
    // The fixture already carries one misc-gen entry, "area1_combined_heat_power",
    // conventionally column 0.
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setMiscGenColumnAllZero(0, 0, true);
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "generation_power", "area1_combined_heat_power").has_value());
    BOOST_CHECK(!FindRow(table, "minus_generation", "area1_combined_heat_power").has_value());
    BOOST_CHECK(!FindRow(table, "balance_port.flow", "area1_combined_heat_power").has_value());
}

BOOST_AUTO_TEST_CASE(misc_gen_columns_are_suppressed_independently_of_each_other)
{
    // A second misc-gen entry, conventionally column 1 ("area1_biomass"),
    // stays active while column 0 is flagged all-zero: each of the 8
    // misc-gen sub-components is suppressed independently.
    problem.InputGenerationOfArea[0].push_back(
      {.componentName = "area1_biomass", .availablePower = {42.}});
    auto analyzer = std::make_shared<Antares::Optimization::InactiveComponentsAnalyzer>();
    analyzer->setMiscGenColumnAllZero(0, 0, true); // combined_heat_power only
    problem.inactiveComponents = analyzer;
    fill();

    BOOST_CHECK(!FindRow(table, "generation_power", "area1_combined_heat_power").has_value());
    BOOST_CHECK(FindRow(table, "generation_power", "area1_biomass").has_value());
}

BOOST_AUTO_TEST_CASE(storage_profit_is_net_withdrawal_times_area_price)
{
    fill();

    // battery1 is in area2 (price 50): floor((100 - 40) * 50 + 0.5) = 3000.
    const auto row = FindRow(table, "profit", "area2_short_term_storage_battery1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->value, 3000.);
}

BOOST_AUTO_TEST_CASE(storage_profit_is_rounded_to_the_nearest_integer)
{
    // (100.01 - 40) * 50 = 3000.5, rounded up by the floor(x + 0.5) formula.
    problem.ProblemeAResoudre->X[stsWithdrawal] = 100.01;
    fill();

    BOOST_CHECK_EQUAL(FindRow(table, "profit", "area2_short_term_storage_battery1")->value, 3001.);
}

BOOST_AUTO_TEST_CASE(storage_profit_is_negative_when_injecting_at_positive_price)
{
    problem.ProblemeAResoudre->X[stsWithdrawal] = 10.;
    fill();

    // floor((10 - 40) * 50 + 0.5) = -1500.
    BOOST_CHECK_EQUAL(FindRow(table, "profit", "area2_short_term_storage_battery1")->value, -1500.);
}

BOOST_AUTO_TEST_CASE(emissions_are_generation_power_times_each_factor)
{
    fill();

    // generation_power = 3600.
    BOOST_CHECK_CLOSE(FindRow(table, "co2_emissions", "area1_thermal_cluster1")->value,
                      3600. * 0.5,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "nox_emissions", "area1_thermal_cluster1")->value,
                      3600. * 0.01,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "op5_emissions", "area1_thermal_cluster1")->value,
                      3600. * 2.,
                      1e-9);
    // A pollutant with a zero factor still gets a row, valued 0.
    const auto so2 = FindRow(table, "so2_emissions", "area1_thermal_cluster1");
    BOOST_REQUIRE(so2.has_value());
    BOOST_CHECK_EQUAL(so2->value, 0.);
}

BOOST_AUTO_TEST_CASE(emissions_emit_one_row_per_pollutant)
{
    fill();

    // One row per pollutant in Pollutant::PollutantEnum, all on "cluster1".
    BOOST_CHECK_EQUAL(RowsForOutput(table, "co2_emissions").size(), 1);
    std::size_t emissionRows = 0;
    const auto& cols = table.columns();
    for (size_t i = 0; i < table.rowCount(); ++i)
    {
        const auto out = cols[2]->toString(i);
        if (out.size() > 10 && out.substr(out.size() - 10) == "_emissions")
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

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "area1_thermal_cluster1")->value,
                      4000.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "up_margin", "area1_thermal_cluster1")->value,
                      4000. - 3600.,
                      1e-9);
    BOOST_CHECK_CLOSE(FindRow(table, "min_gen_power", "area1_thermal_cluster1")->value,
                      500.,
                      1e-9); // min(3600, 500)
    BOOST_CHECK_CLOSE(FindRow(table, "down_margin", "area1_thermal_cluster1")->value,
                      3600. - 500.,
                      1e-9);
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

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "area1_thermal_cluster1")->value,
                      600.,
                      1e-9);
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

    BOOST_CHECK_CLOSE(FindRow(table, "cluster_availability", "area1_thermal_cluster1")->value,
                      250.,
                      1e-9);
}

BOOST_AUTO_TEST_CASE(profit_is_margin_price_times_generation_above_the_min_gen_floor)
{
    // area1 price = -(-10000) = 10000; unperturbed market bid cost = 30;
    // generation_power = 3600; min_gen_power floor = 500.
    // profit = (10000 - 30) * max(3600 - 500, 0) = 9970 * 3100.
    fill();

    const auto row = FindRow(table, "profit", "area1_thermal_cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 9970. * 3100., 1e-9);
}

BOOST_AUTO_TEST_CASE(profit_is_zero_when_generation_does_not_exceed_the_floor)
{
    // A floor above the generation clamps the dispatchable quantity to zero.
    problem.PaliersThermiquesDuPays[0].PuissanceDisponibleEtCout[0].PuissanceMinDuPalierThermique
      = {5000.};
    fill();

    const auto row = FindRow(table, "profit", "area1_thermal_cluster1");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_EQUAL(row->value, 0.);
}

BOOST_AUTO_TEST_CASE(actual_num_units_on_is_ceil_of_nodu)
{
    fill();

    const auto rows = RowsForOutput(table, "actual_num_units_on");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1_thermal_cluster1");
    BOOST_CHECK_EQUAL(rows[0].value, 3.); // ceil(2.3)
}

BOOST_AUTO_TEST_CASE(non_prop_cost_has_no_startup_term_at_the_first_hour)
{
    // At the first hour of the interval the previous NODU is not part of this
    // solution, so the start-up term is dropped:
    // non_prop_cost = fixed_cost * ceil(NODU) = 100 * 3.
    fill();

    const auto row = FindRow(table, "non_prop_cost", "area1_thermal_cluster1");
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
    //                    minus_generation, balance_port.flow)      = 6
    // Port fields: 3 balance_port.price + 3 {area}_load flows
    //              + area1_hydro_storage flow + 2 links x
    //              (out_port.flow, in_port.flow) + cluster1 and
    //              battery1 flows                                  = 13
    // Weekly: area1's hydro_shadow_price and bellman_value        = 2
    BOOST_CHECK_EQUAL(table.rowCount(), 20 + 21 + 16 + 1 + 6 + 13 + 2);
}

// Wires the result addresses the way OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire
// does for the quantities post-processing moves, so the fixture can be mutated
// the way remix hydro / the adequacy patch mutate a solved week.
void wireResultAddresses(PROBLEME_HEBDO& problem)
{
    problem.OptimisationAuPasHebdomadaire = true;
    problem.HeureDansLAnnee = 168;

    auto& solved = *problem.ProblemeAResoudre;
    solved.LegacyVariablesInfo.assign(solved.NombreDeVariables, std::nullopt);
    solved.AdresseOuPlacerLaValeurDesVariablesOptimisees.assign(solved.NombreDeVariables, nullptr);
    solved.AdresseOuPlacerLaValeurDesCoutsMarginaux.assign(solved.NombreDeContraintes, nullptr);

    problem.ResultatsHoraires.resize(problem.NombreDePays);
    for (auto& hourlyResults: problem.ResultatsHoraires)
    {
        hourlyResults.ValeursHorairesDeDefaillancePositive.assign(1, 0.);
        hourlyResults.CoutsMarginauxHoraires.assign(1, 0.);
    }
    problem.ValeursDeNTC[0].ValeurDuFlux.assign(problem.NombreDInterconnexions, 0.);

    solved.AdresseOuPlacerLaValeurDesVariablesOptimisees[unsuppliedArea1]
      = &problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0];
    solved.AdresseOuPlacerLaValeurDesVariablesOptimisees[directFlowLink0] = &problem.ValeursDeNTC[0]
                                                                               .ValeurDuFlux[0];
    solved.AdresseOuPlacerLaValeurDesCoutsMarginaux[balanceArea1] = &problem.ResultatsHoraires[0]
                                                                       .CoutsMarginauxHoraires[0];

    // Publish the solved values, as OPT_AppelDuSimplexe does right after the solve.
    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0] = solved
                                                                             .X[unsuppliedArea1];
    problem.ValeursDeNTC[0].ValeurDuFlux[0] = solved.X[directFlowLink0];
    problem.ResultatsHoraires[0].CoutsMarginauxHoraires[0] = solved.CoutsMarginauxDesContraintes
                                                               [balanceArea1];

    solved.LegacyVariablesInfo[unsuppliedArea1] = LegacyVariableInfo{.name = "UnsuppliedEnergy",
                                                                     .component = "area1_node",
                                                                     .timeIndex = 168};
}

BOOST_AUTO_TEST_CASE(post_process_dump_reads_results_mutated_after_the_solve)
{
    wireResultAddresses(problem);

    // Remix hydro shaves the peak: 32 MW of the shortfall are served by storage.
    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0] = 20.;
    // The adequacy patch reprices the hour at the area's unsupplied energy cost.
    problem.ResultatsHoraires[0].CoutsMarginauxHoraires[0] = -9500.;

    DumpSimulationTableAfterPostProcess(table, problem, fillContext, currentBlock);

    // Raw row: the recorded variable now carries the post-processed value, not X.
    const auto raw = FindRow(table, "unsupplied_energy", "area1_node");
    BOOST_REQUIRE(raw.has_value());
    BOOST_CHECK_CLOSE(raw->value, 20., 1e-9);

    // Derived rows are recomputed from the refreshed solution: imbalance_cost is
    // spillCost * spilled + unsuppliedCost * unsupplied.
    const auto imbalance = FindRow(table, "imbalance_cost", "area1_node");
    BOOST_REQUIRE(imbalance.has_value());
    BOOST_CHECK_CLOSE(imbalance->value, 3. * 7. + 9500. * 20., 1e-9);

    // ... and price follows the repriced dual, negated.
    const auto price = FindRow(table, "price", "area1_node");
    BOOST_REQUIRE(price.has_value());
    BOOST_CHECK_CLOSE(price->value, 9500., 1e-9);
}

BOOST_AUTO_TEST_CASE(post_process_dump_leaves_the_solver_state_untouched)
{
    wireResultAddresses(problem);
    const auto& solved = *problem.ProblemeAResoudre;
    const std::vector<double> xBefore = solved.X;
    const std::vector<double> dualsBefore = solved.CoutsMarginauxDesContraintes;

    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0] = 20.;
    problem.ResultatsHoraires[0].CoutsMarginauxHoraires[0] = -9500.;

    DumpSimulationTableAfterPostProcess(table, problem, fillContext, currentBlock);

    BOOST_CHECK(solved.X == xBefore);
    BOOST_CHECK(solved.CoutsMarginauxDesContraintes == dualsBefore);
    // Specifically, the refreshed entries are back to what the optimizer left.
    BOOST_CHECK_CLOSE(solved.X[unsuppliedArea1], 52., 1e-9);
    BOOST_CHECK_CLOSE(solved.CoutsMarginauxDesContraintes[balanceArea1], -10000., 1e-9);
}

BOOST_AUTO_TEST_CASE(post_process_dump_is_skipped_when_the_optimization_range_is_daily)
{
    wireResultAddresses(problem);
    problem.OptimisationAuPasHebdomadaire = false;
    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[0] = 20.;

    DumpSimulationTableAfterPostProcess(table, problem, fillContext, currentBlock);

    BOOST_CHECK_EQUAL(table.rowCount(), 0u);
}

BOOST_AUTO_TEST_CASE(weekly_block_is_derived_from_the_hour_in_the_year)
{
    problem.HeureDansLAnnee = 168;
    BOOST_CHECK_EQUAL(LegacyWeeklyBlock(problem), 1u);

    problem.HeureDansLAnnee = 0;
    BOOST_CHECK_EQUAL(LegacyWeeklyBlock(problem), 0u);

    problem.HeureDansLAnnee = 3 * 168 + 5;
    BOOST_CHECK_EQUAL(LegacyWeeklyBlock(problem), 3u);
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

    const auto row = FindRowAt(fixture.table, "non_prop_cost", "area1_thermal_cluster1", "169");
    BOOST_REQUIRE(row.has_value());
    BOOST_CHECK_CLOSE(row->value, 5000. * 2. + 100. * 5., 1e-9);
}
