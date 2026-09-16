// test_compute_gems_contribution.cpp

#include <algorithm>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/io/inputs/yml-system/converter.h"
#include "antares/optimisation/linear-problem-api/StructuredLinearProblem.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optimisation/adequacy_patch_csr/gems-part.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace std::string_literals;
using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::LinearProblem;
using namespace Api;
using namespace DataImpl;

// Model YAML: GEMS component with two time-dependent parameters:
//  - residual_load: the residual load seen by the legacy system
//  - load: the GEMS load
// Its port is connected to an area and exposes:
//  - spillage bound        = 2 * residual_load + 30
//  - unsupplied energy bound = load / 2 - 10
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
    - id: gems_model
      parameters:
        - id: residual_load
          time-dependent: true
          scenario-dependent: true
        - id: load
          time-dependent: true
          scenario-dependent: true
      ports:
        - id: area_conn_port
          type: area_conn_port_type
      port-field-definitions:
        - port: area_conn_port
          field: to-area-bound
          definition: 2 * residual_load + 30
        - port: area_conn_port
          field: from-area-bound
          definition: load / 2 - 10
)"s;

static const auto systemYaml = R"(
system:
  id: my_system
  model-libraries: my_lib
  components:
  - id: gems_component
    model: my_lib.gems_model
    scenario-group: sg
    parameters:
      - id: residual_load
        time-dependent: true
        scenario-dependent: true
        value: residual_load
      - id: load
        time-dependent: true
        scenario-dependent: true
        value: load

  area-connections:
    - component: gems_component
      port: area_conn_port
      area: area1
)"s;

namespace
{
// Indices of the unsupplied energy variables in problemAResoudre (Xmax/Xmin).
// area1 -> variable 0 at hour 0, variable 2 at hour 1; area2 -> variable 1.
const int ensVarArea1Hour0 = 0;
const int ensVarArea2 = 1;
const int ensVarArea1Hour1 = 2;

// Indices of the constraints in problemAResoudre (SecondMembre).
const int fictitiousLoadArea1 = 10;
const int fictitiousLoadArea2 = 11;
const int maxEnsLoadArea1 = 12;
const int maxEnsLoadArea2 = 13;
} // namespace

struct GemsContributionFixture
{
    GemsContributionFixture()
    {
        modelerData = buildModelerSystem();

        problemeHebdo.modelerData = modelerData.get();
        problemeHebdo.NomsDesPays.push_back("area1");
        problemeHebdo.NomsDesPays.push_back("area2");
        problemeHebdo.NombreDePays = 2;
        problemeHebdo.HeureDansLAnnee = 0;
        problemeHebdo.year = 0;

        problemeHebdo.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>();
        problemeHebdo.adequacyPatchRuntimeData->areaMode = {
          Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch,
          Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch};

        // Initialize CorrespondanceVarNativesVarOptim for VariableManager.
        problemeHebdo.CorrespondanceVarNativesVarOptim.resize(2); // hours 0 and 1
        problemeHebdo.CorrespondanceVarNativesVarOptim[0].NumeroDeVariableDefaillancePositive = {
          ensVarArea1Hour0,
          ensVarArea2};
        problemeHebdo.CorrespondanceVarNativesVarOptim[1].NumeroDeVariableDefaillancePositive = {
          ensVarArea1Hour1,
          ensVarArea2};

        problemAResoudre.Xmax.resize(20, 0.0);
        problemAResoudre.Xmin.resize(20, 0.0);
        problemAResoudre.SecondMembre.resize(20, 0.0);

        ScenarioGroupRepository scenarioGroupRepository;
        auto scenario = std::make_unique<Scenario>("SG");
        scenario->setTimeSerieNumber(0, 1);
        scenarioGroupRepository.addScenario("SG", std::move(scenario));
        modelerData->scenarioGroupRepository = std::move(scenarioGroupRepository);

        modelerData->dataSeries = std::make_unique<LinearProblemData>();

        auto linearProblem = std::make_shared<StructuredLinearProblem>();
        problemeHebdo.ortoolsProblem_ = linearProblem;
        problemeHebdo.optimEntityContainer = std::make_shared<OptimEntityContainer>(*linearProblem);

        constraintFictitious = {{0, fictitiousLoadArea1}, {1, fictitiousLoadArea2}};
        constraintMaxEns = {{0, maxEnsLoadArea1}, {1, maxEnsLoadArea2}};

        gemsPart = makeGemsPart(&problemeHebdo,
                                problemAResoudre,
                                variableManager_,
                                constraintFictitious,
                                constraintMaxEns);
    }

    // Resets the bounds to the values set by the legacy adequacy patch, right before the GEMS
    // contribution is applied.
    void setLegacyBounds()
    {
        std::fill(problemAResoudre.Xmax.begin(), problemAResoudre.Xmax.end(), 0.0);
        std::fill(problemAResoudre.SecondMembre.begin(), problemAResoudre.SecondMembre.end(), 0.0);

        problemAResoudre.Xmax[ensVarArea1Hour0] = 100.0;
        problemAResoudre.Xmax[ensVarArea2] = 100.0;
        problemAResoudre.Xmax[ensVarArea1Hour1] = 100.0;
        problemAResoudre.SecondMembre[fictitiousLoadArea1] = 500.0;
        problemAResoudre.SecondMembre[fictitiousLoadArea2] = 500.0;
        problemAResoudre.SecondMembre[maxEnsLoadArea1] = 300.0;
        problemAResoudre.SecondMembre[maxEnsLoadArea2] = 300.0;
    }

    // Sets the values of the GEMS parameters (one time series per parameter, one value per hour).
    void setGemsParameters(const std::vector<double>& residualLoad, const std::vector<double>& load)
    {
        auto data = std::make_unique<LinearProblemData>();
        addTimeSeries(*data, "residual_load", residualLoad);
        addTimeSeries(*data, "load", load);
        modelerData->dataSeries = std::move(data);
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

    static void addTimeSeries(LinearProblemData& data,
                              const std::string& name,
                              const std::vector<double>& values)
    {
        auto tsSet = std::make_unique<TimeSeriesSet>(name, values.size());
        tsSet->add(values);
        data.addDataSeries(std::move(tsSet));
    }

    PROBLEME_HEBDO problemeHebdo{};
    std::unique_ptr<Solver::ModelerData> modelerData;
    // Keeps the model definitions alive: the system components hold raw pointers into them.
    std::vector<Library> libraries;
    VariableManagement::VariableManager variableManager_{&problemeHebdo};
    std::unique_ptr<IGemsPart> gemsPart;
    PROBLEME_ANTARES_A_RESOUDRE problemAResoudre{};
    std::map<int, int> constraintFictitious;
    std::map<int, int> constraintMaxEns;
};

BOOST_AUTO_TEST_SUITE(gems_contribution_tests)

// Residual load > 0: the spillage bound increases the RHS of the fictitious load constraint.
BOOST_FIXTURE_TEST_CASE(spillage_bound_with_positive_residual_load, GemsContributionFixture)
{
    // residual_load = 10 -> 2 * 10 + 30 = 50
    // load = 20          -> 20 / 2 - 10 = 0
    setGemsParameters({10.0, 10.0}, {20.0, 20.0});
    setLegacyBounds();

    gemsPart->setHour(0);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    // Fictitious load constraint (spillage bound): only area1 is connected to the GEMS component
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0 + 50.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea2], 500.0);

    // Unsupplied energy bound: contribution is zero
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea2], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea2], 300.0);

    // Bounds not touched by the GEMS contribution remain at their legacy value
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[5], 0.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[5], 0.0);
}

// Residual load < 0: the spillage bound decreases the RHS of the fictitious load constraint.
BOOST_FIXTURE_TEST_CASE(spillage_bound_with_negative_residual_load, GemsContributionFixture)
{
    // residual_load = -20 -> 2 * (-20) + 30 = -10
    // load = 20           -> 20 / 2 - 10 = 0
    setGemsParameters({-20.0, -20.0}, {20.0, 20.0});
    setLegacyBounds();

    gemsPart->setHour(0);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    // Fictitious load constraint (spillage bound): only area1 is connected to the GEMS component
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0 - 10.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea2], 500.0);

    // Unsupplied energy bound: contribution is zero
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea2], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea2], 300.0);
}

// GEMS load > 0: the unsupplied energy bound increases the ENS variable bound and the RHS of the
// max ENS load constraint.
BOOST_FIXTURE_TEST_CASE(unsupplied_energy_bound_with_positive_gems_load, GemsContributionFixture)
{
    // load = 50           -> 50 / 2 - 10 = 15
    // residual_load = -15 -> 2 * (-15) + 30 = 0
    setGemsParameters({-15.0, -15.0}, {50.0, 50.0});
    setLegacyBounds();

    gemsPart->setHour(0);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    // Unsupplied energy bound: only area1 is connected to the GEMS component
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0 + 15.0);
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea2], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0 + 15.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea2], 300.0);

    // Fictitious load constraint (spillage bound): contribution is zero
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea2], 500.0);
}

// GEMS load < 0: the unsupplied energy bound decreases the ENS variable bound and the RHS of the
// max ENS load constraint.
BOOST_FIXTURE_TEST_CASE(unsupplied_energy_bound_with_negative_gems_load, GemsContributionFixture)
{
    // load = -10          -> -10 / 2 - 10 = -15
    // residual_load = -15 -> 2 * (-15) + 30 = 0
    setGemsParameters({-15.0, -15.0}, {-10.0, -10.0});
    setLegacyBounds();

    gemsPart->setHour(0);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    // Unsupplied energy bound: only area1 is connected to the GEMS component
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0 - 15.0);
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea2], 100.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0 - 15.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea2], 300.0);

    // Fictitious load constraint (spillage bound): contribution is zero
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea2], 500.0);
}

// The GEMS load is time-dependent: its contribution is evaluated at the triggered hour.
BOOST_FIXTURE_TEST_CASE(gems_load_is_evaluated_at_the_triggered_hour, GemsContributionFixture)
{
    // load[0] = 1  -> 1 / 2 - 10 = -9.5
    // load[1] = 42 -> 42 / 2 - 10 = 11
    // residual_load = -15 -> 2 * (-15) + 30 = 0
    setGemsParameters({-15.0, -15.0}, {1.0, 42.0});
    setLegacyBounds();

    // Hour 0
    gemsPart->setHour(0);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0 - 9.5);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0 - 9.5);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0);

    // Hour 1: the GEMS part only adds its contribution, so no reset happens between hours.
    gemsPart->setHour(1);
    gemsPart->setBoundsOnENS();
    gemsPart->setRHSfictitiousLoadValue();
    gemsPart->setRHSMaxEnsLoadValue();

    // Hour 1 uses its own ENS variable
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour1], 100.0 + 11.0);
    // The max-ENS constraint accumulates the contribution of both hours
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[maxEnsLoadArea1], 300.0 - 9.5 + 11.0);
    // The ENS variable of hour 0 is not modified at hour 1
    BOOST_CHECK_EQUAL(problemAResoudre.Xmax[ensVarArea1Hour0], 100.0 - 9.5);
    BOOST_CHECK_EQUAL(problemAResoudre.SecondMembre[fictitiousLoadArea1], 500.0);
}

BOOST_AUTO_TEST_CASE(factory_returns_null_gems_part_when_no_modeler_data)
{
    PROBLEME_HEBDO problem{};
    problem.modelerData = nullptr;
    PROBLEME_ANTARES_A_RESOUDRE pa{};
    VariableManagement::VariableManager vm(&problem);
    std::map<int, int> cf, cm;
    auto gp = makeGemsPart(&problem, pa, vm, cf, cm);
    BOOST_CHECK(dynamic_cast<NullGemsPart*>(gp.get()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(factory_returns_active_gems_part, GemsContributionFixture)
{
    BOOST_CHECK(dynamic_cast<ActiveGemsPart*>(gemsPart.get()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(throws_when_no_optimEntityContainer, GemsContributionFixture)
{
    problemeHebdo.optimEntityContainer.reset();
    BOOST_CHECK_THROW((ActiveGemsPart(&problemeHebdo,
                                      problemAResoudre,
                                      variableManager_,
                                      constraintFictitious,
                                      constraintMaxEns)),
                      std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
