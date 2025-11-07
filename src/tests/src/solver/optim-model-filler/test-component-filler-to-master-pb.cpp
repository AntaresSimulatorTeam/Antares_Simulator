#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/variable.h"

#include "component-filler-utils/objectives-creators.h"
#include "component-filler-utils/variables-creators.h"

using namespace Antares::Optimisation;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;

template<class VariablesCreator, class ObjectivesCreator>
class FactoryFixture
{
public:
    FactoryFixture():
        linear_pb(false, "sirius"),
        optimEntityContainer(linear_pb, &dummy_data, &scenario_group_repo),
        variables(VariablesCreator::Create(nodeRegistry)),
        objectives(ObjectivesCreator::Create(nodeRegistry))
    {
        createModel();
        createComponent();
        setOptimEntityContainer();
    }

    // Data members
    Antares::Expressions::Registry<Node>
      nodeRegistry; // Storing AST Nodes (to destroy them at end of test)
    std::vector<Variable> variables;
    std::vector<Objective> objectives;
    Model model;
    // We define a component under the form of a smart ptr because class Component default
    // constructor is forbidden, so we can't have : Component component;
    std::unique_ptr<Component> component;
    OrtoolsLinearProblem linear_pb;

    LinearProblemData dummy_data;
    ScenarioGroupRepository scenario_group_repo;
    OptimEntityContainer optimEntityContainer;

    FillContext time_scenario_ctx = {0, 0, 0, 0, 0};

private:
    // Function members
    void createModel()
    {
        ModelBuilder model_builder;
        model_builder.withId("my-model")
          .withVariables(std::move(variables))
          .withObjectives(std::move(objectives));

        model = model_builder.build();
    }

    void createComponent()
    {
        ComponentBuilder component_builder;
        component_builder.withModel(&model).withId("my-component");
        component = std::make_unique<Component>(component_builder.build());
    }

    void setOptimEntityContainer()
    {
        std::vector<Component> components = {*component};
        optimEntityContainer.addFromSystemComponents(components);
    }
};

namespace Fixtures
{
using _1 = FactoryFixture<TwoVarsCreator_OneSubPb_OneMaster, NoObjectiveCreator>;
using _2 = FactoryFixture<TwoVarsCreator_OneSubPb_OneMaster, NoObjectiveCreator>;
using _3 = FactoryFixture<TwoSubPbVarsCreator, TwoObjsCreator_OneSubPb_OneMaster>;
using _4 = FactoryFixture<TwoSubPbVarsCreator, TwoObjsCreator_OneSubPb_OneMaster>;
} // namespace Fixtures

BOOST_AUTO_TEST_SUITE(add_variables_to_master_linear_problem)

BOOST_FIXTURE_TEST_CASE(adding_variables_to_master_pb_actually_adds_only_master_variables,
                        Fixtures::_1)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Antares::Modeler::Config::Location::MASTER);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var);
}

BOOST_FIXTURE_TEST_CASE(adding_variables_to_pb_actually_adds_only_subproblem_variables,
                        Fixtures::_2)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Antares::Modeler::Config::Location::SUBPROBLEMS);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-1");
    BOOST_REQUIRE(var);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(add_constraints_to_master_linear_problem)

BOOST_FIXTURE_TEST_CASE(adding_objectives_to_pb_actually_adds_only_subproblem_objectives,
                        Fixtures::_3)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Antares::Modeler::Config::Location::SUBPROBLEMS);

    // Act
    componentFiller.addVariables(time_scenario_ctx);
    componentFiller.addObjectives(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 2);
    auto* var1 = linear_pb.lookupVariable("my-component.var-1");
    auto* var2 = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var1);
    BOOST_REQUIRE(var2);

    BOOST_CHECK_EQUAL(linear_pb.getObjectiveCoefficient(var1), 1);
    // No objective associated to var2 found in problem
    BOOST_CHECK_EQUAL(linear_pb.getObjectiveCoefficient(var2), 0);
}

BOOST_FIXTURE_TEST_CASE(adding_objectives_to_master_pb_actually_adds_only_master_objectives,
                        Fixtures::_4)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Antares::Modeler::Config::Location::MASTER);

    componentFiller.addVariables(time_scenario_ctx);
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
