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

#include "component-filler-utils/constraints-creators.h"
#include "component-filler-utils/objectives-creators.h"
#include "component-filler-utils/variables-creators.h"

using namespace Antares::Optimisation;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::Modeler::Config;

template<class VariablesCreator, class ObjectivesCreator, class ConstraintsCreators>
class FactoryFixture
{
public:
    FactoryFixture():
        variables(VariablesCreator::Create(nodeRegistry)),
        objectives(ObjectivesCreator::Create(nodeRegistry)),
        constraints(ConstraintsCreators::Create(nodeRegistry)),
        linear_pb(false, "sirius"),
        optimEntityContainer(linear_pb, &dummy_data, &scenario_group_repo)
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
    std::vector<Constraint> constraints;
    Model model;
    // We define a component under the form of a smart ptr because class Component default
    // constructor is forbidden, so we can't have : Component component;
    std::unique_ptr<Component> component;
    OrtoolsLinearProblem linear_pb;

    LinearProblemData dummy_data;
    ScenarioGroupRepository scenario_group_repo;
    OptimEntityContainer optimEntityContainer;

    FillContext time_scenario_ctx = {0, 0, 0, 0, 0};

    BendersDecomposition bendersDecomposition;

private:
    // Function members
    void createModel()
    {
        ModelBuilder model_builder;
        model_builder.withId("my-model")
          .withVariables(std::move(variables))
          .withObjectives(std::move(objectives))
          .withConstraints(std::move(constraints));

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
using VarOneSubOneMasterNoObjective = FactoryFixture<TwoVarsCreator_OneSubPb_OneMaster,
                                                     NoObjectiveCreator,
                                                     NoConstraintCreator>;
using VarTwoSubObjeOneSubOneMaster = FactoryFixture<TwoSubPbVarsCreator,
                                                    TwoObjsCreator_OneSubPb_OneMaster,
                                                    NoConstraintCreator>;
using SingleMixedVarNoObjective = FactoryFixture<SingleMixedVariable,
                                                 NoObjectiveCreator,
                                                 NoConstraintCreator>;

using VarTwoSubNoObjConstrOneSubOneMaster = FactoryFixture<
  TwoSubPbVarsCreator,
  NoObjectiveCreator,
  TwoConstraintsCreator_OneSubPb_OneMaster>;
} // namespace Fixtures

BOOST_AUTO_TEST_SUITE(add_variables_to_master_linear_problem)

BOOST_FIXTURE_TEST_CASE(adding_variables_to_master_pb_actually_adds_only_master_variables,
                        Fixtures::VarOneSubOneMasterNoObjective)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Location::MASTER,
                                    &bendersDecomposition);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var);

    BOOST_CHECK(bendersDecomposition.connections().empty());
}

BOOST_FIXTURE_TEST_CASE(adding_variables_to_pb_actually_adds_only_subproblem_variables,
                        Fixtures::VarOneSubOneMasterNoObjective)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Location::SUBPROBLEMS,
                                    &bendersDecomposition);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-1");
    BOOST_REQUIRE(var);
    BOOST_CHECK(bendersDecomposition.connections().empty());
}

BOOST_FIXTURE_TEST_CASE(adding_objectives_to_pb_actually_adds_only_subproblem_objectives,
                        Fixtures::VarTwoSubObjeOneSubOneMaster)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Location::SUBPROBLEMS,
                                    &bendersDecomposition);

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
    BOOST_CHECK(bendersDecomposition.connections().empty());
}

BOOST_FIXTURE_TEST_CASE(adding_objectives_to_master_pb_actually_adds_only_master_objectives,
                        Fixtures::VarTwoSubObjeOneSubOneMaster)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Location::MASTER,
                                    &bendersDecomposition);

    componentFiller.addVariables(time_scenario_ctx);
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 0);
    BOOST_CHECK(bendersDecomposition.connections().empty());
}

// TODO expand with 1 mixed variable located in 1 master and N subproblems
// For now we only have MASTER, mostly because the fixture has a single optimEntityContainer
// There should be optimEntityContainer_master and optimEntityContainer_subproblems
BOOST_FIXTURE_TEST_CASE(mixed_variable_listed_in_benders_decomposition,
                        Fixtures::SingleMixedVarNoObjective)
{
    ComponentFiller masterFiller(*component,
                                 optimEntityContainer,
                                 scenario_group_repo,
                                 Location::MASTER,
                                 &bendersDecomposition);

    masterFiller.addVariables(time_scenario_ctx);
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);

    BOOST_REQUIRE_EQUAL(bendersDecomposition.connections().size(), 1);
    // connections = {{"master", {"my-component.var-1", 0}}
    const auto& [name, connections] = *bendersDecomposition.connections().begin();
    BOOST_CHECK_EQUAL(name, "master");
    const auto& connection = connections[0];
    BOOST_CHECK_EQUAL(connection.name, "my-component.var-1");
    BOOST_CHECK_EQUAL(connection.indexInProblem, 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(add_constraints_to_master_linear_problem)

BOOST_FIXTURE_TEST_CASE(adding_two_constraints_one_sub_one_master_in_sub,
                        Fixtures::VarTwoSubNoObjConstrOneSubOneMaster)
{
    ComponentFiller componentFiller(*component,
                                    optimEntityContainer,
                                    scenario_group_repo,
                                    Location::SUBPROBLEMS,
                                    &bendersDecomposition);

    componentFiller.addVariables(time_scenario_ctx);
    componentFiller.addConstraints(time_scenario_ctx);
    BOOST_CHECK_EQUAL(linear_pb.getConstraints().size(), 1);
}

BOOST_FIXTURE_TEST_CASE(adding_two_constraints_one_sub_one_master_in_master,
                        Fixtures::VarTwoSubNoObjConstrOneSubOneMaster)
{
    ComponentFiller masterFiller(*component,
                                 optimEntityContainer,
                                 scenario_group_repo,
                                 Location::MASTER,
                                 &bendersDecomposition);

    masterFiller.addVariables(time_scenario_ctx);
    masterFiller.addConstraints(time_scenario_ctx);
    BOOST_CHECK_EQUAL(linear_pb.getConstraints().size(), 1);
}
BOOST_AUTO_TEST_SUITE_END()
