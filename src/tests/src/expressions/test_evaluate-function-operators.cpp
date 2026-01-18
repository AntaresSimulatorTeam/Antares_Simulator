#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/study/system-model/variabilityType.h>
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include "UtilMocks.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::ModelerStudy::SystemModel;

// =================================
// Fixture to build AST nodes tree
// =================================
struct CreateAST
{
    Node* literal(double value);
    Node* parameter(const std::string& name, const VariabilityType variability);
    Node* floor(Node* node);
    Node* ceil(Node* node);

private:
    Registry<Nodes::Node> registry_;
};

Node* CreateAST::literal(double value)
{
    return registry_.create<LiteralNode>(value);
}

Node* CreateAST::parameter(const std::string& name, const VariabilityType variability)
{
    return registry_.create<ParameterNode>(name, variability);
}

Node* CreateAST::floor(Node* node)
{
    return registry_.create<FunctionNode>(FunctionNodeType::floor, node);
}

Node* CreateAST::ceil(Node* node)
{
    return registry_.create<FunctionNode>(FunctionNodeType::ceil, node);
}

// =================================
// Fixture to create an EvalVisitor
// =================================
struct CreateEvalVisitor
{
    CreateEvalVisitor();

    std::unique_ptr<Visitors::EvalVisitor> evalVisitor;

private:
    LinearProblemDataImpl::LinearProblemData data_;
    MockLinearProblem linearProblem_;
    std::unique_ptr<OptimEntityContainer> optimEntityContainer_;
    LinearProblemApi::FillContext fillCtx_;
    Model model_;
    Component component_;
    std::vector<Component> components_;
    ScenarioGroupRepository scenarioGroupRepo_;
};

CreateEvalVisitor::CreateEvalVisitor():
    linearProblem_(true),
    fillCtx_(0, 2, 0, 2, 0),
    model_(
      createModelWithParameters({Parameter("p", TimeDependent::YES, ScenarioDependent::NO),
                                 Parameter("p-const", TimeDependent::NO, ScenarioDependent::NO)})),
    component_(createComponent(
      model_,
      "component-id",
      {{"p", {"p", VariabilityType::VARYING_IN_TIME_ONLY, "p"}},
       {"p-const", {"p-const", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, "4.5"}}},
      0)),
    scenarioGroupRepo_(makeScenarioGroupRepo(component_)),
    components_({component_})
{
    // Parameter p : make associated time-series
    auto ts = std::make_unique<TimeSeriesSet>("p", 3);
    ts->add({1.5, 2.3, 3.7});
    data_.addDataSeries(std::move(ts));

    // Creation of a OptimEntityContainer
    optimEntityContainer_ = std::make_unique<OptimEntityContainer>(linearProblem_,
                                                                   &data_,
                                                                   &scenarioGroupRepo_);
    optimEntityContainer_->addFromSystemComponents(components_);

    // And finally, creation of the evaluation visitor (purpose of this fixture)
    evalVisitor = std::make_unique<EvalVisitor>(*optimEntityContainer_, fillCtx_, component_);
}

// =================================================
// Test fixture : made from both previous fixtures
// =================================================
struct eval_function_op_fixture: CreateAST, CreateEvalVisitor
{
};

BOOST_AUTO_TEST_SUITE(test_floor_operator)

BOOST_FIXTURE_TEST_CASE(floor_applied_to_a_time_dependent_parameter, eval_function_op_fixture)
{
    // Expression : floor(p), where p = {1.5, 2.3, 3.7}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);
    Node* floor_node = floor(p);

    auto evalResult = evalVisitor->dispatch(floor_node);

    std::vector<double> expected_result = {1., 2., 3.};
    BOOST_CHECK(evalResult.valuesAsVector() == expected_result);
}

BOOST_FIXTURE_TEST_CASE(floor_applied_to_a_literal, eval_function_op_fixture)
{
    // Expression : floor(2.3)
    Node* p = literal(2.3);
    Node* floor_node = floor(p);

    auto evalResult = evalVisitor->dispatch(floor_node);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 2.);
}

BOOST_FIXTURE_TEST_CASE(floor_applied_to_a_constant_parameter, eval_function_op_fixture)
{
    // Expression : floor(p), where p = {4.5, 4.5, 4.5}
    Node* p = parameter("p-const", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    Node* floor_node = floor(p);

    auto evalResult = evalVisitor->dispatch(floor_node);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 4.);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_ceil_operator)

BOOST_FIXTURE_TEST_CASE(ceil_applied_to_a_time_dependent_parameter, eval_function_op_fixture)
{
    // Expression : ceil(p), where p = {1.5, 2.3, 3.7}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);
    Node* ceil_node = ceil(p);

    auto evalResult = evalVisitor->dispatch(ceil_node);

    std::vector<double> expected_result = {2., 3., 4.};
    BOOST_CHECK(evalResult.valuesAsVector() == expected_result);
}

BOOST_FIXTURE_TEST_CASE(ceil_applied_to_a_literal, eval_function_op_fixture)
{
    // Expression : ceil(2.3)
    Node* p = literal(2.3);
    Node* ceil_node = ceil(p);

    auto evalResult = evalVisitor->dispatch(ceil_node);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 3.);
}

BOOST_FIXTURE_TEST_CASE(ceil_applied_to_a_constant_parameter, eval_function_op_fixture)
{
    // Expression : ceil(p), where p = {4.5, 4.5, 4.5}
    Node* p = parameter("p-const", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    Node* ceil_node = ceil(p);

    auto evalResult = evalVisitor->dispatch(ceil_node);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 5.);
}

BOOST_AUTO_TEST_SUITE_END()
