// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/study/system-model/system.h>

using namespace Antares::Solver;
using namespace Antares::Solver::LoadFiles;
using namespace Antares::ModelerStudy::SystemModel;

namespace
{
// A thermal model with constant and time-dependent parameters and variables, to bind the
// elements of both heuristics
const std::string thermalLibrary = R"(library:
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
        - id: d_min_up
          time-dependent: false
          scenario-dependent: false
        - id: d_min_down # may be time-dependent: checked on the component values
        - id: p_max_cluster
        - id: nb_units_max
      variables:
        - id: generation
          lower-bound: 0
        - id: nb_units_on
          lower-bound: 0
          variable-type: integer
        - id: invested
          time-dependent: false
          scenario-dependent: false
)";

const std::string fastHeuristic = R"(
      - id: fast
        inputs:
          - heuristic-element: generation_power
            id: generation
            type: variable-solution
          - heuristic-element: cluster_max_generation
            id: p_max_cluster
          - heuristic-element: min_power_per_unit
            id: p_min_unit
          - heuristic-element: max_power_per_unit
            id: p_max_unit
          - heuristic-element: min_up_duration
            id: d_min_up
          - heuristic-element: min_down_duration
            id: d_min_down
        outputs:
          - heuristic-element: minimum_generation_power
            id: generation
            type: variable-lower-bound)";

const std::string accurateHeuristic = R"(
      - id: accurate
        inputs:
          - heuristic-element: num_units_on_opt
            id: nb_units_on
            type: variable-solution
          - heuristic-element: num_units_max
            id: nb_units_on
            type: variable-upper-bound
          - heuristic-element: min_up_duration
            id: d_min_up
          - heuristic-element: min_down_duration
            id: d_min_down
        outputs:
          - heuristic-element: minimum_num_units_on
            id: nb_units_on
            type: variable-upper-bound)";

std::string optimConfig(const std::string& heuristics)
{
    return "models:\n  - id: lib.thermal\n    heuristics:" + heuristics + "\n";
}

struct HeuristicsFixture
{
    HeuristicsFixture()
    {
        studyFolder = std::filesystem::temp_directory_path() / "antares-test-load-heuristics";
        std::filesystem::remove_all(studyFolder);
        std::filesystem::create_directories(studyFolder / "input" / "model-libraries");
        write(studyFolder / "input" / "model-libraries" / "lib.yml", thermalLibrary);
    }

    ~HeuristicsFixture()
    {
        std::filesystem::remove_all(studyFolder);
    }

    LoadedLibraries load(const std::string& optimConfigContent)
    {
        write(studyFolder / "input" / "optim-config.yml", optimConfigContent);
        return loadLibraries(studyFolder);
    }

    static void write(const std::filesystem::path& path, const std::string& content)
    {
        std::ofstream out(path, std::ofstream::trunc);
        out << content;
    }

    std::filesystem::path studyFolder;
};

/// Replaces the first occurrence of `from` in `text` by `to`
std::string replace(std::string text, const std::string& from, const std::string& to)
{
    const auto position = text.find(from);
    BOOST_REQUIRE(position != std::string::npos);
    return text.replace(position, from.size(), to);
}

void checkLoadingFails(HeuristicsFixture& fixture,
                       const std::string& heuristics,
                       const std::string& expectedMessage)
{
    BOOST_CHECK_EXCEPTION(fixture.load(optimConfig(heuristics)),
                          std::exception,
                          containsMessage(expectedMessage));
}
} // namespace

BOOST_AUTO_TEST_SUITE(convert_heuristics)

BOOST_FIXTURE_TEST_CASE(both_heuristics_are_converted, HeuristicsFixture)
{
    const auto loaded = load(optimConfig(fastHeuristic + accurateHeuristic));

    const auto& heuristics = loaded.heuristicsByModel.at("lib.thermal");
    BOOST_REQUIRE_EQUAL(heuristics.size(), 2);

    const auto& fast = heuristics[0];
    BOOST_CHECK(fast.type == HeuristicType::Fast);
    BOOST_CHECK_EQUAL(fast.inputs.size(), 6);
    BOOST_CHECK(fast.input(HeuristicElement::GenerationPower)->source
                == HeuristicInputSource::VariableSolution);
    BOOST_CHECK_EQUAL(fast.input(HeuristicElement::MinUpDuration)->id, "d_min_up");
    BOOST_CHECK(fast.input(HeuristicElement::MinUpDuration)->source
                == HeuristicInputSource::Parameter);
    BOOST_CHECK(fast.output(HeuristicElement::MinimumGenerationPower)->target
                == HeuristicOutputTarget::VariableLowerBound);
    BOOST_CHECK(fast.input(HeuristicElement::NumUnitsOnOpt) == nullptr);

    const auto& accurate = heuristics[1];
    BOOST_CHECK(accurate.type == HeuristicType::Accurate);
    BOOST_CHECK(accurate.input(HeuristicElement::NumUnitsMax)->source
                == HeuristicInputSource::VariableUpperBound);
    BOOST_CHECK(accurate.output(HeuristicElement::MinimumNumUnitsOn)->target
                == HeuristicOutputTarget::VariableUpperBound);
}

BOOST_FIXTURE_TEST_CASE(variable_bounds_can_be_inputs, HeuristicsFixture)
{
    auto heuristic = replace(accurateHeuristic,
                             "type: variable-upper-bound",
                             "type: variable-lower-bound");
    const auto loaded = load(optimConfig(heuristic));
    const auto& accurate = loaded.heuristicsByModel.at("lib.thermal")[0];
    BOOST_CHECK(accurate.input(HeuristicElement::NumUnitsMax)->source
                == HeuristicInputSource::VariableLowerBound);
}

BOOST_FIXTURE_TEST_CASE(models_without_heuristics_are_ignored, HeuristicsFixture)
{
    const auto loaded = load(R"(models:
  - id: lib.thermal
    model-decomposition:
      variables:
        - id: generation
          location: subproblems
)");
    BOOST_CHECK(loaded.heuristicsByModel.empty());
}

BOOST_FIXTURE_TEST_CASE(unknown_model, HeuristicsFixture)
{
    BOOST_CHECK_EXCEPTION(load("models:\n  - id: lib.unknown\n    heuristics:" + fastHeuristic),
                          std::exception,
                          containsMessage("No model 'lib.unknown' found"));
}

BOOST_FIXTURE_TEST_CASE(heuristic_defined_twice, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      fastHeuristic + fastHeuristic,
                      "heuristic 'fast' is defined several times");
}

BOOST_FIXTURE_TEST_CASE(input_of_the_other_heuristic, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic, "cluster_max_generation", "num_units_max"),
                      "'num_units_max' is not an input of this heuristic");
}

BOOST_FIXTURE_TEST_CASE(input_defined_twice, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic, "cluster_max_generation", "min_power_per_unit"),
                      "input 'min_power_per_unit' is defined several times");
}

BOOST_FIXTURE_TEST_CASE(missing_input, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              R"(          - heuristic-element: cluster_max_generation
            id: p_max_cluster
)",
                              ""),
                      "missing input 'cluster_max_generation'");
}

BOOST_FIXTURE_TEST_CASE(output_of_the_other_heuristic, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic, "minimum_generation_power", "minimum_num_units_on"),
                      "'minimum_num_units_on' is not an output of this heuristic");
}

BOOST_FIXTURE_TEST_CASE(output_defined_twice, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      fastHeuristic + R"(
          - heuristic-element: minimum_generation_power
            id: generation
            type: variable-upper-bound)",
                      "output 'minimum_generation_power' is defined several times");
}

BOOST_FIXTURE_TEST_CASE(missing_output, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              R"(
        outputs:
          - heuristic-element: minimum_generation_power
            id: generation
            type: variable-lower-bound)",
                              ""),
                      "missing output 'minimum_generation_power'");
}

BOOST_FIXTURE_TEST_CASE(unknown_parameter, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic, "id: p_max_cluster", "id: unknown_param"),
                      "no parameter 'unknown_param' in the model");
}

BOOST_FIXTURE_TEST_CASE(unknown_variable, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              "id: generation\n            type: variable-solution",
                              "id: unknown_var\n            type: variable-solution"),
                      "no variable 'unknown_var' in the model");
}

BOOST_FIXTURE_TEST_CASE(constant_element_bound_to_a_time_dependent_variable, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              "id: d_min_up",
                              "id: generation\n            type: variable-solution"),
                      "'generation' must not be time-dependent");
}

BOOST_FIXTURE_TEST_CASE(time_dependent_element_bound_to_a_constant_variable, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              "id: generation\n            type: variable-solution",
                              "id: invested\n            type: variable-solution"),
                      "'invested' must be time-dependent");
}

BOOST_FIXTURE_TEST_CASE(time_dependent_element_bound_to_a_constant_parameter, HeuristicsFixture)
{
    checkLoadingFails(*this,
                      replace(fastHeuristic,
                              "id: generation\n            type: variable-solution",
                              "id: p_min_unit\n            type: parameter"),
                      "'p_min_unit' must be time-dependent");
}

BOOST_FIXTURE_TEST_CASE(free_element_accepts_a_constant_variable, HeuristicsFixture)
{
    const auto heuristic = replace(fastHeuristic,
                                   "id: p_max_cluster",
                                   "id: invested\n            type: variable-solution");
    const auto loaded = load(optimConfig(heuristic));
    BOOST_CHECK_EQUAL(loaded.heuristicsByModel.at("lib.thermal")[0]
                        .input(HeuristicElement::ClusterMaxGeneration)
                        ->id,
                      "invested");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_integer_strategies)

namespace
{
struct IntegerStrategiesFixture: HeuristicsFixture
{
    /// Builds a modeler data with one thermal component
    ModelerData modelerData(const std::string& strategy,
                            const std::string& heuristicId,
                            const std::string& optimConfigContent,
                            Antares::LinearProblem::VariabilityType minDownType = Antares::
                              LinearProblem::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        auto loaded = load(optimConfigContent);
        ModelerData data;
        data.libraries = std::move(loaded.libraries);
        data.heuristicsByModel = std::move(loaded.heuristicsByModel);

        const auto* model = &data.libraries[0].Models().at("thermal");
        std::map<std::string, ParameterTypeAndValue> parameters;
        for (const auto& [id, parameter]: model->Parameters())
        {
            parameters[id] = {
              .id = id,
              .type = Antares::LinearProblem::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
              .value = "1"};
        }
        parameters["d_min_down"].type = minDownType;
        std::vector<Component> components;
        components.push_back(ComponentBuilder()
                               .withId("G")
                               .withModel(model)
                               .withParameterValues(parameters)
                               .withIntegerStrategy(strategy, heuristicId)
                               .build());
        data.system = std::make_unique<System>(
          SystemBuilder().withId("system").withComponents(std::move(components)).build());
        return data;
    }
};
} // namespace

BOOST_FIXTURE_TEST_CASE(exact_and_relaxed_components_need_no_heuristic, IntegerStrategiesFixture)
{
    const auto config = optimConfig(fastHeuristic);
    BOOST_CHECK_NO_THROW(checkIntegerStrategies(modelerData("exact", "", config)));
    BOOST_CHECK_NO_THROW(checkIntegerStrategies(modelerData("relaxed", "", config)));
}

BOOST_FIXTURE_TEST_CASE(declared_heuristic_is_accepted, IntegerStrategiesFixture)
{
    const auto data = modelerData("heuristic", "fast", optimConfig(fastHeuristic));
    BOOST_CHECK_NO_THROW(checkIntegerStrategies(data));
    BOOST_CHECK(data.hasHeuristicComponents());
    BOOST_REQUIRE(data.heuristicFor(data.system->Components()[0]) != nullptr);
    BOOST_CHECK(data.heuristicFor(data.system->Components()[0])->type == HeuristicType::Fast);
}

BOOST_FIXTURE_TEST_CASE(heuristic_not_declared_for_the_model, IntegerStrategiesFixture)
{
    BOOST_CHECK_EXCEPTION(checkIntegerStrategies(
                            modelerData("heuristic", "accurate", optimConfig(fastHeuristic))),
                          std::exception,
                          containsMessage("Component 'G': heuristic 'accurate' is not declared in "
                                          "optim-config.yml for model 'lib.thermal'"));
}

BOOST_FIXTURE_TEST_CASE(model_without_any_heuristic, IntegerStrategiesFixture)
{
    const auto data = modelerData("heuristic",
                                  "fast",
                                  R"(models:
  - id: lib.thermal
    model-decomposition:
      variables: []
)");
    BOOST_CHECK(data.heuristicFor(data.system->Components()[0]) == nullptr);
    BOOST_CHECK_EXCEPTION(checkIntegerStrategies(data),
                          std::exception,
                          containsMessage("heuristic 'fast' is not declared"));
}

BOOST_FIXTURE_TEST_CASE(heuristics_are_incompatible_with_benders, IntegerStrategiesFixture)
{
    auto data = modelerData("heuristic", "fast", optimConfig(fastHeuristic));
    data.resolutionMode = ResolutionMode::BENDERS_DECOMPOSITION;
    BOOST_CHECK_EXCEPTION(checkIntegerStrategies(data),
                          std::exception,
                          containsMessage("integer strategy 'heuristic' is not compatible with "
                                          "the benders-decomposition resolution mode"));
}

BOOST_FIXTURE_TEST_CASE(constant_input_given_a_time_series, IntegerStrategiesFixture)
{
    const auto data = modelerData("heuristic",
                                  "fast",
                                  optimConfig(fastHeuristic),
                                  Antares::LinearProblem::VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EXCEPTION(checkIntegerStrategies(data),
                          std::exception,
                          containsMessage("parameter 'd_min_down' is used as heuristic input "
                                          "'min_down_duration' and must not be time-dependent"));
}

BOOST_AUTO_TEST_SUITE_END()
