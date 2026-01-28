// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/converter.h>
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"
#include "antares/study/system-model/library.h"

#include "unit_test_utils.h"

using namespace std::string_literals;

using namespace Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Optimisation;
using namespace LinearProblemApi;
using namespace LinearProblemDataImpl;

static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: area_conn_port_type
      description: some port type for area connection
      fields:
        - id: to-area-bound
      area-connection:
        - to-area-bound-field: to-area-bound

  models:
    - id: model_with_vars
      variables:
        - id: var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: var_2
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: area_conn_port
          type: area_conn_port_type
      port-field-definitions:
        - port: area_conn_port
          field: to-area-bound
          definition: 2 * var_1 - var_2
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
      area: Area1
)"s;

struct AreaConnectionFixture
{
    // public function members :
    AreaConnectionFixture();

    std::unique_ptr<Solver::ModelerData> buildModelerSystem();
    
    // public data members :
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<Library> libraries;
};

AreaConnectionFixture::AreaConnectionFixture()
{
    modelerData = buildModelerSystem();
}

std::unique_ptr<Solver::ModelerData> AreaConnectionFixture::buildModelerSystem()
{
    IO::Inputs::YmlModel::Parser parserModel;
    libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));

    IO::Inputs::YmlSystem::Parser parserSystem;
    auto ymlSystem = parserSystem.parse(systemYaml);
    auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);
    modelerData = std::make_unique<Solver::ModelerData>();

    auto data = std::make_unique<Solver::ModelerData>();
    data->system = std::make_unique<System>(std::move(system));
    return data;
}


BOOST_AUTO_TEST_SUITE(_area_connections___spillage_)

BOOST_FIXTURE_TEST_CASE(dummy_test, AreaConnectionFixture)
{
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
