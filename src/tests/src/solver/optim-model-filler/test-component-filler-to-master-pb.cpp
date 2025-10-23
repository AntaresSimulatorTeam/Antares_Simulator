#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/variable.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;

Expression createLiteral(std::string name, double value, Registry<Nodes::Node>& nodeRegistry)
{
    NodeRegistry node_registry(nodeRegistry.create<LiteralNode>(value), std::move(nodeRegistry));
    return Expression(name, std::move(node_registry));
}

class VariablesCreator
{
public:
    explicit VariablesCreator(Registry<Nodes::Node>& nodeRegistry):
        nodeRegistry_(nodeRegistry)
    {
    }

    virtual std::vector<Variable> create() = 0;

protected:
    Registry<Nodes::Node>& nodeRegistry_;
};

class TwoVarsCreator_OneSubPb_OneMaster: public VariablesCreator
{
    using VariablesCreator::VariablesCreator;

public:
    std::vector<Variable> create() override
    {
        Variable var_1("var-1",
                       createLiteral("low-bound", 0., nodeRegistry_),
                       createLiteral("up-bound", 1., nodeRegistry_),
                       ValueType::FLOAT,
                       TimeDependent::NO,
                       ScenarioDependent::NO,
                       Config::Location::SUBPROBLEMS);
        Variable var_2("var-2",
                       createLiteral("low-bound", 0., nodeRegistry_),
                       createLiteral("up-bound", 1., nodeRegistry_),
                       ValueType::FLOAT,
                       TimeDependent::NO,
                       ScenarioDependent::NO,
                       Config::Location::MASTER);

        std::vector<Variable> variables;
        variables.emplace_back(std::move(var_1));
        variables.emplace_back(std::move(var_2));
        return variables;
    }
};

class TwoSubPbVarsCreator: public VariablesCreator
{
    using VariablesCreator::VariablesCreator;

public:
    std::vector<Variable> create() override
    {
        Variable var_1("var-1",
                       createLiteral("low-bound", 0., nodeRegistry_),
                       createLiteral("up-bound", 1., nodeRegistry_),
                       ValueType::FLOAT,
                       TimeDependent::NO,
                       ScenarioDependent::NO,
                       Config::Location::SUBPROBLEMS);
        Variable var_2("var-2",
                       createLiteral("low-bound", 0., nodeRegistry_),
                       createLiteral("up-bound", 1., nodeRegistry_),
                       ValueType::FLOAT,
                       TimeDependent::NO,
                       ScenarioDependent::NO,
                       Config::Location::SUBPROBLEMS);

        std::vector<Variable> variables;
        variables.emplace_back(std::move(var_1));
        variables.emplace_back(std::move(var_2));
        return variables;
    }
};

class ObjectivesCreator
{
public:
    explicit ObjectivesCreator(Registry<Nodes::Node>& nodeRegistry):
        nodeRegistry_(nodeRegistry)
    {
    }

    virtual std::vector<Objective> create() = 0;

protected:
    Registry<Nodes::Node>& nodeRegistry_;
};

class NoObjectiveCreator: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<Objective> create() override
    {
        return {};
    }
};

Objective makeSimpleObjective(Registry<Nodes::Node>& nodeRegistry,
                              const std::string varId,
                              unsigned varIndex,
                              const std::string objectiveId,
                              Config::Location locoation)
{
    auto varNode = nodeRegistry.create<Nodes::VariableNode>(varId,
                                                            varIndex,
                                                            TimeIndex::VARYING_IN_TIME_ONLY);
    NodeRegistry node_registry(varNode, std::move(nodeRegistry));
    Expression expression("expr-" + objectiveId, std::move(node_registry));
    Objective objective(objectiveId, std::move(expression), locoation);
    return objective;
}

class TwoObjsCreator_OneSubPb_OneMaster: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;
public:
    std::vector<Objective> create() override
    {
        std::vector<Objective> objectives;
        auto obj_1 = makeSimpleObjective(nodeRegistry_,
                                         "var-1",
                                         0,
                                         "obj-1",
                                         Config::Location::SUBPROBLEMS);
        auto obj_2 = makeSimpleObjective(nodeRegistry_,
                                         "var-2",
                                         1,
                                         "obj-2",
                                         Config::Location::MASTER);

        objectives.emplace_back(std::move(obj_1));
        objectives.emplace_back(std::move(obj_2));
        return objectives;
    }
};

struct FactoryFixture
{
    FactoryFixture():
        linear_pb(false, "sirius"),
        optimEntityContainer(linear_pb, &dummy_data, &scenario_group_repo)
    {
    }

    void initialize(std::unique_ptr<VariablesCreator> varCreator,
                    std::unique_ptr<ObjectivesCreator> objCreator)
    {
        variables = varCreator->create();
        objectives = objCreator->create();
        createModel();
        createComponent();
        setOptimEntityContainer();
    }

    // Function members
    void createModel();
    void createComponent();
    void setOptimEntityContainer();

    // Data members
    Registry<Node> nodeRegistry; // Storing AST Nodes (to destroy them at end of test)
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
};

void FactoryFixture::createModel()
{
    ModelBuilder model_builder;
    model_builder.withId("my-model")
      .withVariables(std::move(variables))
      .withObjectives(std::move(objectives));

    model = model_builder.build();
}

void FactoryFixture::createComponent()
{
    ComponentBuilder component_builder;
    component_builder.withModel(&model).withId("my-component");
    component = std::make_unique<Component>(component_builder.build());
}

void FactoryFixture::setOptimEntityContainer()
{
    std::vector<Component> components = {*component};
    optimEntityContainer.addFromSystemComponents(components);
}

BOOST_FIXTURE_TEST_SUITE(add_variables_to_master_linear_problem, FactoryFixture)

BOOST_AUTO_TEST_CASE(adding_variables_to_master_pb_actually_adds_only_master_variables)
{
    // Arrange
    initialize(std::make_unique<TwoVarsCreator_OneSubPb_OneMaster>(nodeRegistry),
               std::make_unique<NoObjectiveCreator>(nodeRegistry));
    ComponentFiller componentFiller(*component, optimEntityContainer, scenario_group_repo);

    // Act
    componentFiller.addVariablesToMaster(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var);
}

BOOST_AUTO_TEST_CASE(adding_variables_to_pb_actually_adds_only_subproblem_variables)
{
    // Arrange
    initialize(std::make_unique<TwoVarsCreator_OneSubPb_OneMaster>(nodeRegistry),
               std::make_unique<NoObjectiveCreator>(nodeRegistry));
    ComponentFiller componentFiller(*component, optimEntityContainer, scenario_group_repo);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-1");
    BOOST_REQUIRE(var);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(add_constraints_to_master_linear_problem, FactoryFixture)

BOOST_AUTO_TEST_CASE(adding_objectives_to_pb_actually_adds_only_subproblem_objectives)
{
    // Arrange
    initialize(std::make_unique<TwoSubPbVarsCreator>(nodeRegistry),
               std::make_unique<TwoObjsCreator_OneSubPb_OneMaster>(nodeRegistry));
    ComponentFiller componentFiller(*component, optimEntityContainer, scenario_group_repo);

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
    BOOST_CHECK_EQUAL(linear_pb.getObjectiveCoefficient(var2), 0);
}

BOOST_AUTO_TEST_SUITE_END()
