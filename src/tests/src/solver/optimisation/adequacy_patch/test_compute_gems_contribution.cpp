// test_compute_gems_contribution.cpp

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/io/inputs/yml-system/converter.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace std::string_literals;
using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::LinearProblem;
using namespace Api;
using namespace DataImpl;

// Model YAML: component with var_1, port with spillage_bound = 2*var_1+30
// and unsupplied_energy_bound = var_1/2-10
static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: area_conn_port_type
      description: port type for area connection
      fields:
        - id: to-area-bound
        - id: from-area-bound
      area-connection:
        injection-to-balance:
        spillage-bound: to-area-bound
        unsupplied-energy-bound: from-area-bound

  models:
    - id: model_with_vars
      variables:
        - id: var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: area_conn_port
          type: area_conn_port_type
      port-field-definitions:
        - port: area_conn_port
          field: to-area-bound
          definition: 2 * var_1 + 30
        - port: area_conn_port
          field: from-area-bound
          definition: var_1 / 2 - 10
)"s;

static const auto systemYaml = R"(
system:
  id: my_system
  model-libraries: my_lib
  components:
  - id: component_with_vars
    model: my_lib.model_with_vars
    scenario-group: sg

  area-connections:
    - component: component_with_vars
      port: area_conn_port
      area: area1
)"s;

struct GemsContributionFixture
{
    GemsContributionFixture():
        linearProblem(true, "scip")
    {
        modelerData = buildModelerSystem();

        problemeHebdo.modelerData = modelerData.get();
        problemeHebdo.NomsDesPays.push_back("area1");
        problemeHebdo.NomsDesPays.push_back("area2");
        problemeHebdo.NombreDePays = 2;
        problemeHebdo.HeureDansLAnnee = 0;
        problemeHebdo.year = 0;

        auto scenario = std::make_unique<Scenario>("SG");
        scenarioGroupRepository.addScenario("SG", std::move(scenario));
        modelerData->scenarioGroupRepository = std::move(scenarioGroupRepository);

        optimContainer = std::make_unique<OptimEntityContainer>(linearProblem);
        optimContainer->addFromSystemComponents(modelerData->system->Components());
        problemeHebdo.optimEntityContainer = optimContainer.get();

        addComponentsVariablesToLP();
    }

    std::unique_ptr<Solver::ModelerData> buildModelerSystem()
    {
        auto data = std::make_unique<Solver::ModelerData>();
        IO::Inputs::YmlModel::Parser parserModel;
        libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));

        IO::Inputs::YmlSystem::Parser parserSystem;
        auto ymlSystem = parserSystem.parse(systemYaml, "");
        auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);
        data->system = std::make_unique<System>(std::move(system));
        return data;
    }

    void addComponentsVariablesToLP()
    {
        FillContext ctx = {0, 0, 0, 0, 0};
        for (const auto& component: modelerData->system->Components())
        {
            for (const auto& variable: component.getModel()->Variables())
            {
                optimContainer->addStartColumn();
                for (unsigned t = 0; t <= ctx.getLocalLastTimeStep(); ++t)
                {
                    auto name = buildVariableName(component.Id(), variable.Id(), {}, t);
                    linearProblem.addVariable(-999, 999, false, name);
                }
            }
        }
    }

    PROBLEME_HEBDO problemeHebdo{};
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<Library> libraries;
    MpsolverImpl::OrtoolsLinearProblem linearProblem;
    std::unique_ptr<OptimEntityContainer> optimContainer;
    ScenarioGroupRepository scenarioGroupRepository;
};

BOOST_AUTO_TEST_SUITE(compute_gems_contribution_for_area)

BOOST_AUTO_TEST_CASE(no_modeler_data_returns_zero)
{
    PROBLEME_HEBDO problem{};
    problem.modelerData = nullptr;
    problem.NomsDesPays.push_back("area1");

    double result = computeGemsContributionForArea(&problem, 0, 0, "unsupplied_energy_bound");
    BOOST_CHECK_EQUAL(result, 0.0);
}

BOOST_AUTO_TEST_CASE(no_optimEntityContainer_returns_zero)
{
    GemsContributionFixture f;
    f.problemeHebdo.optimEntityContainer = nullptr;

    double result = computeGemsContributionForArea(&f.problemeHebdo,
                                                   0,
                                                   0,
                                                   "unsupplied_energy_bound");
    BOOST_CHECK_EQUAL(result, 0.0);
}

BOOST_AUTO_TEST_CASE(area_not_connected_returns_zero)
{
    GemsContributionFixture f;
    // area2 (index 1) has no GEMS component connected
    double result = computeGemsContributionForArea(&f.problemeHebdo,
                                                   1,
                                                   0,
                                                   "unsupplied_energy_bound");
    BOOST_CHECK_EQUAL(result, 0.0);
}

BOOST_FIXTURE_TEST_CASE(unsupplied_energy_bound_returns_evaluated_value, GemsContributionFixture)
{
    // definition: var_1 / 2 - 10, var_1 starts at 0 → expected = 0/2 - 10 = -10
    double result = computeGemsContributionForArea(&problemeHebdo, 0, 0, "unsupplied_energy_bound");
    BOOST_CHECK_EQUAL(result, -10.0);
}

BOOST_FIXTURE_TEST_CASE(spillage_bound_returns_evaluated_value, GemsContributionFixture)
{
    // definition: 2 * var_1 + 30, var_1 starts at 0 → expected = 2*0 + 30 = 30
    double result = computeGemsContributionForArea(&problemeHebdo, 0, 0, "spillage_bound");
    BOOST_CHECK_EQUAL(result, 30.0);
}

BOOST_FIXTURE_TEST_CASE(unknown_port_field_name_returns_zero, GemsContributionFixture)
{
    double result = computeGemsContributionForArea(&problemeHebdo, 0, 0, "nonexistent_field");
    BOOST_CHECK_EQUAL(result, 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
