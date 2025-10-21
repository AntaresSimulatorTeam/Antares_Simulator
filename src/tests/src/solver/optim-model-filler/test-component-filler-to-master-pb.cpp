#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/variable.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"

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

BOOST_AUTO_TEST_CASE(dummy)
{
    // ======================
    // Step : arrange
    // ======================
    // Register used to store all AST Nodes (in order to destroy them at end of test)
    Registry<Node> nodeRegistry;

    // Creating variables
    Variable var_1("var-1",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::BOOL,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::SUBPROBLEMS);

    Variable var_2("var-2",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::BOOL,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::MASTER);

    // Creating a vector of variables
    std::vector<Variable> variables;
    variables.emplace_back(std::move(var_1));
    variables.emplace_back(std::move(var_2));

    // Creating a model
    ModelBuilder model_builder;
    model_builder.withVariables(std::move(variables));
    auto model = model_builder.build();

    // Creating a component
    ComponentBuilder component_builder;
    component_builder.withModel(&model).withId("my-component");
    auto component = component_builder.build();

    // Creating an empty linear problem
    OrtoolsLinearProblem linear_pb(false, "sirius");
    
    // Creating an OptimEntityContainer
    LinearProblemData dummy_data;
    ScenarioGroupRepository scenario_group_repo;
    OptimEntityContainer optimEntityContainer(linear_pb, &dummy_data, &scenario_group_repo);

    std::vector<Component> components = {component};
    optimEntityContainer.addFromSystemComponents(components);

    // Creating a component filler
    ComponentFiller componentFiller(component, optimEntityContainer, scenario_group_repo);

    // Creating a fill context
    FillContext time_scenario_ctx = {0, 0, 0, 0, 0};

    // ======================
    // Step : act
    // ======================
    componentFiller.addVariablesToMaster(time_scenario_ctx);

    // ======================
    // Step : assert
    // ======================
    BOOST_CHECK_EQUAL(linear_pb.variableCount(), 1);

    auto* var = linear_pb.lookupVariable("my-component.var-2");
    BOOST_REQUIRE(var);
}
