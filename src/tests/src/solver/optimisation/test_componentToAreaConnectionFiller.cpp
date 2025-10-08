/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <string>

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
using namespace Optimisation::LinearProblemApi;
using namespace Optimisation::LinearProblemDataImpl;

static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: some_port_type
      description: port type with area connection
      fields:
        - id: some_field
        - id: connected_field
      area-connection:
        - injection-field: connected_field

  models:
    - id: connected_model_through_vars
      variables:
        - id: not_connected_var
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: connected_var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: connected_var_2
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: connection_port_var
          type: some_port_type
      port-field-definitions:
        - port: connection_port_var
          field: some_field
          definition: not_connected_var
        - port: connection_port_var
          field: connected_field
          definition: 5 * connected_var_1 - 37 * connected_var_2

    - id: connected_model_through_param
      parameters:
        - id: some_param
          time-dependent: true
          scenario-dependent: false
      ports:
        - id: connection_port_param
          type: some_port_type
      port-field-definitions:
        - port: connection_port_param
          field: some_field
          definition: some_param
        - port: connection_port_param
          field: connected_field
          definition: 2 * some_param - 6
    )"s;

static const auto systemYaml = R"(
system:
  id: my_system

  model-libraries: my_lib

  components:
  - id: connected_component_var
    model: my_lib.connected_model_through_vars
    scenario-group: sg
  - id: connected_component_param
    model: my_lib.connected_model_through_param
    scenario-group: sg
    parameters:
    - id: some_param
      time-dependent: true
      scenario-dependent: false
      value: some_param_value

  area-connections:
    - component: connected_component_var
      port: connection_port_var
      area: Area1
    - component: connected_component_param
      port: connection_port_param
      area: Area1
    )"s;

struct ComponentToAreaConnectionFillerFixture
{
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Modeler::Data> modelerData;
    std::vector<Library> libraries;
    Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem;
    Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    std::remove_reference_t<LinearProblemData&> data;

    ComponentToAreaConnectionFillerFixture():
        linearProblem(true, "scip")
    {
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
        setUpModelerSystem();

        auto scenarioPtr = std::make_unique<Scenario>("SG");
        scenarioPtr->setTimeSerieNumber(0, 1);
        scenarioGroupRepository.addScenario("SG", std::move(scenarioPtr));
    }

    void setData(const std::vector<double>& some_param_value)
    {
        auto tss = std::make_unique<TimeSeriesSet>("some_param_value", some_param_value.size());
        tss->add(some_param_value);
        DataSeriesRepository ds;
        ds.addDataSeries(std::move(tss));
        LinearProblemData d(std::move(ds));
        data = std::move(d);
    }

    void setUpModelerSystem()
    {
        IO::Inputs::YmlModel::Parser parserModel;
        libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));
        IO::Inputs::YmlSystem::Parser parserSystem;
        auto ymlSystem = parserSystem.parse(systemYaml);
        auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);
        modelerData = std::make_unique<Modeler::Data>();
        modelerData->system = std::make_unique<System>(std::move(system));
        problemeHebdo->modelerData = modelerData.get();
    }

    void setUpModelerVariables(unsigned int ts_start,
                               unsigned int ts_end,
                               Optimisation::OptimEntityContainer& optimEntityContainer)
    {
        const Optimisation::Dimensions dim({}, Optimisation::IntegerInterval(ts_start, ts_end));
        for (const auto& component: modelerData->system->Components())
        {
            for (const auto& variable: component.getModel()->Variables())
            {
                optimEntityContainer.addStartColumn();
                if (variable.isTimeDependent())
                {
                    for (auto t = ts_start; t <= ts_end; ++t)
                    {
                        auto name = Optimisation::buildVariableName(component.Id(),
                                                                    variable.Id(),
                                                                    {},
                                                                    t);
                        linearProblem.addVariable(-999, 999, false, name);
                    }
                }
                else
                {
                    auto name = Optimisation::buildVariableName(component.Id(),
                                                                variable.Id(),
                                                                std::nullopt,
                                                                std::nullopt);
                    linearProblem.addVariable(-999, 999, false, name);
                }
            }
        }
    }

    void addEmptyConstraintsToLinearProblem(std::vector<std::string>& names, double rhs)
    {
        for (const auto& name: names)
        {
            linearProblem.addConstraint(rhs, rhs, name);
        }
    }

    void addEmptyConstraints(std::vector<std::string>& constraintNames,
                             bool useNamedProblems,
                             double rhs)
    {
        problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes = constraintNames.size();
        if (useNamedProblems)
        {
            addEmptyConstraintsToLinearProblem(constraintNames, rhs);
        }
        else
        {
            std::vector<std::string> lpConstraintNames;
            for (unsigned int i = 0; i < constraintNames.size(); ++i)
            {
                lpConstraintNames.push_back("c" + std::to_string(i));
            }
            addEmptyConstraintsToLinearProblem(lpConstraintNames, rhs);
        }
    }

    void setUpLegacyLp(std::vector<std::string>& constraintNames, bool useNamedProblems, double rhs)
    {
        problemeHebdo->NamedProblems = useNamedProblems;
        addEmptyConstraints(constraintNames, useNamedProblems, rhs);
    }

    void fillProblem(const FillContext& fillCtx,
                     Optimisation::OptimEntityContainer& optimEntityContainer)
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = fillCtx.getLocalNumberOfTimeSteps();

        ComponentToAreaConnectionFiller filler(problemeHebdo.get(),
                                               optimEntityContainer,
                                               data,
                                               scenarioGroupRepository);

        filler.addVariables(fillCtx);
        filler.addConstraints(fillCtx);
        filler.addObjective(fillCtx);
    }
};

BOOST_FIXTURE_TEST_SUITE(_ComponentToAreaConnectionFiller_, ComponentToAreaConnectionFillerFixture)

BOOST_AUTO_TEST_CASE(add_one_term_to_balance_constraint_named)
{
    setData({4.0});

    Optimisation::OptimEntityContainer optimEntityContainer(linearProblem,
                                                            &data,
                                                            &scenarioGroupRepository);
    for (const auto& component: modelerData->system->Components())
    {
        optimEntityContainer.addFromSystemComponent(component);
    }
    setUpModelerVariables(0, 0, optimEntityContainer);
    std::vector<std::string> constraints({"whatever", "AreaBalance::area<area1>::hour<0>"});
    setUpLegacyLp(constraints, true, 10);
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);
    fillProblem({0, 0, 0, 0, 0}, optimEntityContainer);

    auto balance_ct = linearProblem.lookupConstraint("AreaBalance::area<area1>::hour<0>");
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t0")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_1_t0")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_2_t0")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct->getLb(), 10 + 2 * 4 - 6);
    BOOST_CHECK_EQUAL(balance_ct->getUb(), 10 + 2 * 4 - 6);

    auto other_ct = linearProblem.lookupConstraint("whatever");
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_1_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_2_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getLb(), 10);
    BOOST_CHECK_EQUAL(other_ct->getUb(), 10);
}

BOOST_AUTO_TEST_CASE(add_two_terms_to_balance_constraint_not_named)
{
    setData({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -51.0, 8.3});

    Optimisation::OptimEntityContainer optimEntityContainer(linearProblem,
                                                            &data,
                                                            &scenarioGroupRepository);
    for (const auto& component: modelerData->system->Components())
    {
        optimEntityContainer.addFromSystemComponent(component);
    }
    setUpModelerVariables(10, 11, optimEntityContainer);
    // Legacy indexing of TS always starts at 1
    std::vector<std::string> constraints(
      {"whatever", "AreaBalance::area<area1>::hour<0>", "AreaBalance::area<area1>::hour<1>"});
    setUpLegacyLp(constraints, false, -100);
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[1].NumeroDeContrainteDesBilansPays.push_back(2);
    fillProblem({0, 1, 10, 11, 0}, optimEntityContainer);

    auto balance_ct_t10 = linearProblem.lookupConstraint("c1");
    auto balance_ct_t11 = linearProblem.lookupConstraint("c2");

    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t10")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t10")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getLb(), -100 + 2 * -51 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t10->getUb(), -100 + 2 * -51 - 6);
    ;

    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t11")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t11")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct_t11->getLb(), -100 + 2 * 8.3 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t11->getUb(), -100 + 2 * 8.3 - 6);
}

BOOST_AUTO_TEST_CASE(fail_if_constraint_not_defined)
{
    setData({4.0});

    Optimisation::OptimEntityContainer optimEntityContainer(linearProblem,
                                                            &data,
                                                            &scenarioGroupRepository);
    for (const auto& component: modelerData->system->Components())
    {
        optimEntityContainer.addFromSystemComponent(component);
    }
    setUpModelerVariables(0, 0, optimEntityContainer);
    std::vector<std::string> constraints({"whatever"});
    setUpLegacyLp(constraints, true, 0);
    BOOST_CHECK_EXCEPTION(fillProblem({0, 0, 0, 0, 0}, optimEntityContainer),
                          std::runtime_error,
                          checkMessage("A component is connected to area \"area1\", that does not "
                                       "have a balance constraint defined for timestep 0"));
}

BOOST_AUTO_TEST_SUITE_END()
