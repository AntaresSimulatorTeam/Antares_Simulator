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

struct FactoryFixture
{
    FactoryFixture():
        linear_pb(false, "sirius"),
        optimEntityContainer(linear_pb, &dummy_data, &scenario_group_repo)
    {
    }

    void initialize(std::unique_ptr<VariablesCreator> varCreator)
    {
        variables = varCreator->create();
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
    model_builder.withId("my-model").withVariables(std::move(variables));
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
    initialize(std::make_unique<TwoVarsCreator_OneSubPb_OneMaster>(nodeRegistry));
    ComponentFiller componentFiller(*component, optimEntityContainer, scenario_group_repo);

    // Act
    componentFiller.addVariablesToMaster(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var);
}

BOOST_AUTO_TEST_CASE(adding_variables_pb_actually_adds_only_subproblem_variables)
{
    // Arrange
    initialize(std::make_unique<TwoVarsCreator_OneSubPb_OneMaster>(nodeRegistry));
    ComponentFiller componentFiller(*component, optimEntityContainer, scenario_group_repo);

    // Act
    componentFiller.addVariables(time_scenario_ctx);

    // Assert
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);
    auto* var = linear_pb.lookupVariable("my-component.var-1");
    BOOST_REQUIRE(var);
}

BOOST_AUTO_TEST_SUITE_END()
