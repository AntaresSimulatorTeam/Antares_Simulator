// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/io/inputs/yml-system/converter.h"
#include "antares/io/inputs/yml-system/parser.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/gemsThermalHeuristics.h"

using namespace std::string_literals;
using namespace Antares;
using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::DataImpl;
using namespace Antares::ModelerStudy::SystemModel;
using Antares::Optimization::GemsHeuristicsState;

namespace
{
const auto libraryYaml = R"(
library:
  id: lib
  models:
    - id: thermal
      parameters:
        - id: p_min_unit
          time-dependent: false
          scenario-dependent: false
        - id: p_max_unit
          time-dependent: false
          scenario-dependent: false
        - id: d_min_down
          time-dependent: false
          scenario-dependent: false
        - id: p_max_cluster
        - id: d_min_down_scenario
          time-dependent: false
      variables:
        - id: generation
          lower-bound: 0
          upper-bound: p_max_cluster
        - id: nb_units_on
          lower-bound: 0
          upper-bound: p_max_cluster / p_max_unit
          variable-type: integer
        - id: invested
          lower-bound: 0
          upper-bound: 2
          time-dependent: false
          scenario-dependent: false
      objective-contributions:
        - id: production
          expression: sum(-generation - nb_units_on)
        - id: investment
          expression: -invested
)"s;

std::string component(const std::string& id, const std::string& heuristicId)
{
    return R"(
    - id: )"
           + id + R"(
      model: lib.thermal
      scenario-group: SG
      integer-strategy:
        id: heuristic
        heuristic-id: )"
           + heuristicId + R"(
      parameters:
        - id: p_min_unit
          time-dependent: false
          scenario-dependent: false
          value: 50
        - id: p_max_unit
          time-dependent: false
          scenario-dependent: false
          value: 100
        - id: d_min_down
          time-dependent: false
          scenario-dependent: false
          value: 1
        - id: p_max_cluster
          time-dependent: true
          scenario-dependent: true
          value: pmax_ts
        - id: d_min_down_scenario
          time-dependent: false
          scenario-dependent: true
          value: dmin_ts)";
}

// G: heuristic fast, H: heuristic accurate. The inputs use every kind of source (variable
// solution, lower and upper bounds of time-dependent and constant variables, constant,
// time-dependent and scenario-dependent parameters).
HeuristicDefinition fastHeuristic()
{
    using enum HeuristicElement;
    using enum HeuristicInputSource;
    return {.type = HeuristicType::Fast,
            .inputs = {{GenerationPower, "generation", VariableSolution},
                       {ClusterMaxGeneration, "p_max_cluster", Parameter},
                       {MinPowerPerUnit, "p_min_unit", Parameter},
                       {MaxPowerPerUnit, "p_max_unit", Parameter},
                       {MinUpDuration, "invested", VariableUpperBound},
                       {MinDownDuration, "d_min_down", Parameter}},
            .outputs = {
              {MinimumGenerationPower, "generation", HeuristicOutputTarget::VariableLowerBound}}};
}

HeuristicDefinition accurateHeuristic()
{
    using enum HeuristicElement;
    using enum HeuristicInputSource;
    return {.type = HeuristicType::Accurate,
            .inputs = {{NumUnitsOnOpt, "nb_units_on", VariableSolution},
                       {NumUnitsMax, "nb_units_on", VariableLowerBound},
                       {MinUpDuration, "invested", VariableSolution},
                       {MinDownDuration, "d_min_down_scenario", Parameter}},
            .outputs = {
              {MinimumNumUnitsOn, "nb_units_on", HeuristicOutputTarget::VariableUpperBound}}};
}

/// Hybrid-like setup: a 4 time step problem holding the GEMS components G and H, solved as a
/// "first optimisation" (the components are relaxed: integer strategy heuristic)
struct GemsHeuristicsFixture
{
    GemsHeuristicsFixture():
        linearProblem(true, "scip"),
        optimEntityContainer(linearProblem)
    {
        IO::Inputs::YmlModel::Parser modelParser;
        modelerData.libraries.push_back(
          IO::Inputs::ModelConverter::convert(modelParser.parse(libraryYaml)));
        IO::Inputs::YmlSystem::Parser systemParser;
        const auto systemYaml = "system:\n  id: s\n  components:" + component("G", "fast")
                                + component("H", "accurate") + "\n";
        modelerData.system = std::make_unique<System>(
          IO::Inputs::SystemConverter::convert(systemParser.parse(systemYaml, ""),
                                               modelerData.libraries));
        modelerData.heuristicsByModel["lib.thermal"] = {fastHeuristic(), accurateHeuristic()};

        DataSeriesRepository dataSeries;
        auto pmax = std::make_unique<TimeSeriesSet>("pmax_ts", 4);
        pmax->add(std::vector{150., 150., 0., 0.});
        dataSeries.addDataSeries(std::move(pmax));
        auto dmin = std::make_unique<TimeSeriesSet>("dmin_ts", 1);
        dmin->add(std::vector{1.});
        dataSeries.addDataSeries(std::move(dmin));
        modelerData.dataSeries = std::make_unique<LinearProblemData>(std::move(dataSeries));

        auto scenario = std::make_unique<Scenario>("SG");
        scenario->setTimeSerieNumber(0, 1);
        modelerData.scenarioGroupRepository.addScenario("SG", std::move(scenario));

        Optimization::resetGemsHeuristicsState(state, 0, 4);
    }

    void solveFirstOptimisation()
    {
        const auto& components = modelerData.system->Components();
        optimEntityContainer.addFromSystemComponents(components);
        std::vector<std::unique_ptr<ComponentFiller>> fillers;
        for (const auto& c: components)
        {
            fillers.push_back(
              std::make_unique<ComponentFiller>(c,
                                                modelerData.dataSeries.get(),
                                                optimEntityContainer,
                                                modelerData.scenarioGroupRepository,
                                                Solver::Config::Location::SUBPROBLEMS));
        }
        for (const auto& filler: fillers)
        {
            filler->addVariables(ctx);
        }
        for (const auto& filler: fillers)
        {
            filler->addObjectives(ctx);
        }
        linearProblem.setMinimization();
        linearProblem.solve(false);
    }

    std::vector<double> overriddenLowerBounds(const std::string& componentId,
                                              const std::string& variableId)
    {
        std::vector<double> lb(4, 0.);
        std::vector<double> ub(4, 1000.);
        state.boundOverrides.apply(componentId, variableId, ctx, lb, ub);
        return lb;
    }

    std::vector<double> overriddenUpperBounds(const std::string& componentId,
                                              const std::string& variableId)
    {
        std::vector<double> lb(4, 0.);
        std::vector<double> ub(4, 1000.);
        state.boundOverrides.apply(componentId, variableId, ctx, lb, ub);
        return ub;
    }

    Solver::ModelerData modelerData;
    MpsolverImpl::OrtoolsLinearProblem linearProblem;
    OptimEntityContainer optimEntityContainer;
    const Api::FillContext ctx{0, 3, 0, 3, 0};
    GemsHeuristicsState state;
};

void checkEqual(const std::vector<double>& actual, const std::vector<double>& expected)
{
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}
} // namespace

BOOST_FIXTURE_TEST_SUITE(gems_thermal_heuristics, GemsHeuristicsFixture)

BOOST_AUTO_TEST_CASE(inputs_are_captured_after_the_first_optimisation)
{
    solveFirstOptimisation();
    Optimization::captureGemsHeuristicInputs(state, modelerData, optimEntityContainer, ctx);

    // generation at its upper bound p_max_cluster, nb_units_on at p_max_cluster / p_max_unit
    checkEqual(state.solutions.at({"G", "generation"}), {150., 150., 0., 0.});
    checkEqual(state.solutions.at({"H", "nb_units_on"}), {1.5, 1.5, 0., 0.});
    // a constant variable is repeated at each time step
    checkEqual(state.solutions.at({"H", "invested"}), {2., 2., 2., 2.});
    checkEqual(state.upperBounds.at({"G", "invested"}), {2., 2., 2., 2.});
    checkEqual(state.lowerBounds.at({"H", "nb_units_on"}), {0., 0., 0., 0.});
    // inputs read from parameters are not captured
    BOOST_CHECK(!state.solutions.contains({"G", "p_max_cluster"}));
}

BOOST_AUTO_TEST_CASE(heuristics_compute_the_bounds_of_the_second_optimisation)
{
    solveFirstOptimisation();
    Optimization::captureGemsHeuristicInputs(state, modelerData, optimEntityContainer, ctx);
    Optimization::runGemsThermalHeuristics(state, modelerData, 0);

    // fast: 2 units of 100 MW for 150 MW, kept on by blocks of max(2, 1) hours,
    // minimum generation = 2 * p_min_unit
    BOOST_CHECK(state.boundOverrides.contains("G", "generation"));
    checkEqual(overriddenLowerBounds("G", "generation"), {100., 100., 0., 0.});

    // accurate: ceil(1.5) = 2 units, applied to the upper bound (output target of the test)
    BOOST_CHECK(state.boundOverrides.contains("H", "nb_units_on"));
    checkEqual(overriddenUpperBounds("H", "nb_units_on"), {2., 2., 0., 0.});
}

BOOST_AUTO_TEST_CASE(reset_clears_the_previous_week)
{
    solveFirstOptimisation();
    Optimization::captureGemsHeuristicInputs(state, modelerData, optimEntityContainer, ctx);
    Optimization::runGemsThermalHeuristics(state, modelerData, 0);

    Optimization::resetGemsHeuristicsState(state, 168, 4);
    BOOST_CHECK(state.solutions.empty());
    BOOST_CHECK(state.lowerBounds.empty());
    BOOST_CHECK(state.upperBounds.empty());
    BOOST_CHECK(state.boundOverrides.empty());
    BOOST_CHECK_EQUAL(state.firstGlobalTimeStep, 168);
}

BOOST_AUTO_TEST_CASE(missing_first_optimisation_values)
{
    // runGemsThermalHeuristics without captureGemsHeuristicInputs
    BOOST_CHECK_THROW(Optimization::runGemsThermalHeuristics(state, modelerData, 0),
                      Optimization::GemsHeuristicError);
}

BOOST_AUTO_TEST_CASE(fast_does_nothing_without_minimum_power)
{
    auto heuristic = fastHeuristic();
    heuristic.inputs[2] = {HeuristicElement::MinPowerPerUnit,
                           "invested",
                           HeuristicInputSource::VariableLowerBound}; // = 0
    modelerData.heuristicsByModel["lib.thermal"] = {heuristic, accurateHeuristic()};

    solveFirstOptimisation();
    Optimization::captureGemsHeuristicInputs(state, modelerData, optimEntityContainer, ctx);
    Optimization::runGemsThermalHeuristics(state, modelerData, 0);

    BOOST_CHECK(!state.boundOverrides.contains("G", "generation"));
    BOOST_CHECK(state.boundOverrides.contains("H", "nb_units_on"));
}

BOOST_AUTO_TEST_CASE(unknown_input_variable)
{
    auto heuristic = fastHeuristic();
    heuristic.inputs[0].id = "unknown";
    modelerData.heuristicsByModel["lib.thermal"] = {heuristic, accurateHeuristic()};

    solveFirstOptimisation();
    BOOST_CHECK_THROW(
      Optimization::captureGemsHeuristicInputs(state, modelerData, optimEntityContainer, ctx),
      Optimization::GemsHeuristicError);
}

BOOST_AUTO_TEST_SUITE_END()
