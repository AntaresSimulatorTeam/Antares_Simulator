// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_gems_csr_adapter
#define WIN32_LEAN_AND_MEAN

#include <map>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/NodeRegistry.h>
#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/EqualNode.h>
#include <antares/expressions/nodes/GreaterThanOrEqualNode.h>
#include <antares/expressions/nodes/LessThanOrEqualNode.h>
#include <antares/expressions/nodes/LiteralNode.h>
#include <antares/expressions/nodes/MultiplicationNode.h>
#include <antares/expressions/nodes/ParameterNode.h>
#include <antares/expressions/nodes/SumNode.h>
#include <antares/expressions/nodes/VariableNode.h>
#include <antares/study/system-model-base/variabilityType.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/constraint.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/parameter.h>
#include <antares/study/system-model/port.h>
#include <antares/study/system-model/portFieldDefinition.h>
#include <antares/study/system-model/portType.h>
#include <antares/study/system-model/system.h>
#include <antares/study/system-model/variable.h>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"

using namespace Antares;
using namespace Antares::AdequacyPatch;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static Expression makeExpr(Node* root, Registry<Node>& reg)
{
    NodeRegistry nr{root, std::move(reg)};
    return Expression{"expr", std::move(nr)};
}

// Constraint: lhs <= rhs (all nodes allocated in reg)
static Constraint makeLEConstraint(const std::string& id,
                                   Node* lhs,
                                   Node* rhs,
                                   Registry<Node>& reg)
{
    auto* le = reg.create<LessThanOrEqualNode>(lhs, rhs);
    return Constraint{id, makeExpr(le, reg)};
}

// PortFieldDefinition with expression pointing to a single VariableNode
static PortFieldDefinition makePortVarDef(const std::string& portId,
                                          const std::string& fieldId,
                                          unsigned int varIdx,
                                          const PortType& portType,
                                          Registry<Node>& reg)
{
    auto* varNode = reg.create<VariableNode>("var", varIdx);
    Port port{portId, portType};
    PortField field{fieldId};
    return PortFieldDefinition{std::move(port), std::move(field), makeExpr(varNode, reg)};
}

// Mock ILinearProblemData — returns pre-loaded scalar values keyed by (dataSetId, tsNumber).
class MockLinearProblemData final
    : public Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
public:
    std::map<std::pair<std::string, unsigned>, double> values;

    double getData(const std::string& dataSetId,
                   unsigned tsNumber,
                   unsigned /*hour*/) const override
    {
        auto it = values.find({dataSetId, tsNumber});
        return (it != values.end()) ? it->second : 0.0;
    }

    std::span<const double> getData(const std::string& /*dataSetId*/,
                                    unsigned /*tsNumber*/,
                                    unsigned /*firstHour*/,
                                    unsigned /*lastHour*/) const override
    {
        return {};
    }
};

// Simple mock CsrProblemBuilder
class MockBuilder final: public CsrProblemBuilder
{
public:
    struct ColInfo
    {
        std::string name;
        double lb;
        double ub;
    };

    int allocateColumn(const std::string& name, double lb, double ub) override
    {
        allocated.push_back({name, lb, ub});
        return nextCol_++;
    }

    std::vector<ColInfo> allocated;

private:
    int nextCol_ = 0;
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(GemsCsrAdapterSuite)

BOOST_AUTO_TEST_CASE(no_match_default_filter)
{
    // Constraint name doesn't match "^flow_based_constraint_" — rows should be empty
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(100.0);
    auto constraint = makeLEConstraint("some_other_constraint", var, lit, reg);

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("model1").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp1")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues({})
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_CHECK(rows.empty());
}

BOOST_AUTO_TEST_CASE(simple_variable_le_literal_extra_column)
{
    // Constraint: "flow_based_constraint_x: x <= 100.0"
    // No area connection → variable gets an extra column via MockBuilder
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(100.0);
    auto constraint = makeLEConstraint("flow_based_constraint_x", var, lit, reg);

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp1")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues({})
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    // One column should have been allocated for "comp1.x"
    BOOST_REQUIRE_EQUAL(builder.allocated.size(), 1u);
    BOOST_CHECK_EQUAL(builder.allocated[0].name, "comp1.x");

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);

    const CsrRow& row = rows[0];
    BOOST_CHECK_EQUAL(row.constraintId, "comp1.flow_based_constraint_x");
    BOOST_CHECK(row.sense == CsrRowSense::LE);
    BOOST_CHECK_CLOSE(row.rhs, 100.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 1u);
    BOOST_CHECK_EQUAL(row.terms[0].column, 0); // first allocated column
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 1.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(area_connected_variable_maps_to_csr_column)
{
    // Variable "flow" is exposed through port "p1" connected to area "north"
    // PortFieldDefinition: p1.net_pos = VariableNode(0)
    // areaToColumnIndex["north"] = 7
    // Expected: row term has column=7

    PortType portType{"area_port_t", {PortField{"net_pos"}}, "net_pos"};

    Registry<Node> reg;

    // PortFieldDefinition expression: VariableNode(idx=0)
    auto pfd = makePortVarDef("p1", "net_pos", 0u, portType, reg);

    // Constraint: flow_based_constraint_link: var0 <= 200.0
    auto* var = reg.create<VariableNode>("flow", 0u);
    auto* lit = reg.create<LiteralNode>(200.0);
    auto constraint = makeLEConstraint("flow_based_constraint_link", var, lit, reg);

    Variable v{"flow", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m2")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withPorts({Port{"p1", portType}})
      .withPortFieldDefinitions({std::move(pfd)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp2")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues({})
                       .build();
    comp.addAreaConnection("p1", "north");

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap{{"north", 7}};
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);
    // Variable is area-connected → no extra column needed
    BOOST_CHECK(builder.allocated.empty());

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    const CsrRow& row = rows[0];
    BOOST_CHECK(row.sense == CsrRowSense::LE);
    BOOST_CHECK_CLOSE(row.rhs, 200.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 1u);
    BOOST_CHECK_EQUAL(row.terms[0].column, 7);
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 1.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(constant_parameter_resolves_to_rhs)
{
    // Constraint: "flow_based_constraint_param: x <= cap" where cap=75.0 (constant)
    using Optimisation::VariabilityType;

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* param = reg.create<ParameterNode>("cap",
                                            VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    auto* le = reg.create<LessThanOrEqualNode>(var, param);
    Constraint constraint{"flow_based_constraint_param", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m3")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withParameters({Parameter{"cap", TimeDependent::NO, ScenarioDependent::NO}});
    Model model = mb.build();

    std::map<std::string, ParameterTypeAndValue> paramValues{
      {"cap", {.id = "cap",
               .type = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
               .value = "75.0"}}};

    ComponentBuilder cb;
    Component comp = cb.withId("comp3")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues(std::move(paramValues))
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    const CsrRow& row = rows[0];
    BOOST_CHECK_CLOSE(row.rhs, 75.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 1u);
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 1.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(multiplication_literal_variable)
{
    // Constraint: "flow_based_constraint_mul: 3.0 * x <= 90.0"
    // → terms: {col: 3.0}, rhs: 90.0

    Registry<Node> reg;
    auto* lit3 = reg.create<LiteralNode>(3.0);
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* mul = reg.create<MultiplicationNode>(lit3, var);
    auto* rhs = reg.create<LiteralNode>(90.0);
    auto* le = reg.create<LessThanOrEqualNode>(mul, rhs);
    Constraint constraint{"flow_based_constraint_mul", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m4").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp4")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues({})
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    const CsrRow& row = rows[0];
    BOOST_CHECK(row.sense == CsrRowSense::LE);
    BOOST_CHECK_CLOSE(row.rhs, 90.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 1u);
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 3.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(custom_filter_matches_differently_named_constraint)
{
    // Custom filter matches "^csr_constr_"
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("y", 0u);
    auto* lit = reg.create<LiteralNode>(55.0);
    auto constraint = makeLEConstraint("csr_constr_abc", var, lit, reg);

    Variable v{"y", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m5").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp5")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues({})
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx, std::regex(R"(^csr_constr_)")};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK_EQUAL(rows[0].constraintId, "comp5.csr_constr_abc");
    BOOST_CHECK_CLOSE(rows[0].rhs, 55.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(scenario_dependent_parameter_rhs_differs_per_mc_year)
{
    // Parameter "ram_direct_fbc_0001" is VARYING_IN_SCENARIO_ONLY.
    // The adapter maps:  tsNumber = mcYear + 1  (1-based convention).
    //   mcYear=0  →  tsNumber=1  →  100.0
    //   mcYear=1  →  tsNumber=2  →  200.0
    using Antares::Optimisation::VariabilityType;

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* param = reg.create<ParameterNode>("ram_direct_fbc_0001",
                                            VariabilityType::VARYING_IN_SCENARIO_ONLY);
    auto* le = reg.create<LessThanOrEqualNode>(var, param);
    Constraint constraint{"flow_based_constraint_fbc", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_mc")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withParameters({Parameter{"ram_direct_fbc_0001", TimeDependent::NO, ScenarioDependent::YES}});
    Model model = mb.build();

    std::map<std::string, ParameterTypeAndValue> paramValues{
      {"ram_direct_fbc_0001",
       {.id = "ram_direct_fbc_0001",
        .type = VariabilityType::VARYING_IN_SCENARIO_ONLY,
        .value = "ram_direct_fbc_0001"}}};

    ComponentBuilder cb;
    Component comp = cb.withId("ccr_mc")
                       .withModel(&model)
                       .withIndex(0)
                       .withParameterValues(std::move(paramValues))
                       .build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys_mc").withComponents(std::move(comps)).build();

    MockLinearProblemData ds;
    ds.values[{"ram_direct_fbc_0001", 1u}] = 100.0; // mcYear=0 → tsNumber=1
    ds.values[{"ram_direct_fbc_0001", 2u}] = 200.0; // mcYear=1 → tsNumber=2

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, &ds};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    const auto rows0 = adapter.rowsForHour(0, 0); // mcYear=0
    BOOST_REQUIRE_EQUAL(rows0.size(), 1u);
    BOOST_CHECK_CLOSE(rows0[0].rhs, 100.0, 1e-9);

    const auto rows1 = adapter.rowsForHour(0, 1); // mcYear=1
    BOOST_REQUIRE_EQUAL(rows1.size(), 1u);
    BOOST_CHECK_CLOSE(rows1[0].rhs, 200.0, 1e-9);

    // The two scenarios must produce distinct RHS values.
    BOOST_CHECK_NE(rows0[0].rhs, rows1[0].rhs);
}

BOOST_AUTO_TEST_CASE(ge_sense_constraint)
{
    // Constraint: "flow_based_constraint_ge: x >= 50.0" → sense GE
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(50.0);
    auto* ge = reg.create<GreaterThanOrEqualNode>(var, lit);
    Constraint constraint{"flow_based_constraint_ge", makeExpr(ge, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_ge").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("c_ge").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK(rows[0].sense == CsrRowSense::GE);
    BOOST_CHECK_CLOSE(rows[0].rhs, 50.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(eq_sense_constraint)
{
    // Constraint: "flow_based_constraint_eq: x == 30.0" → sense EQ
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(30.0);
    auto* eq = reg.create<EqualNode>(var, lit);
    Constraint constraint{"flow_based_constraint_eq", makeExpr(eq, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_eq").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("c_eq").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK(rows[0].sense == CsrRowSense::EQ);
    BOOST_CHECK_CLOSE(rows[0].rhs, 30.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(sum_expression_ptdf_two_area_variables)
{
    // Constraint: "flow_based_constraint_ptdf: 0.65*north + 0.35*south <= 300"
    // north (varIdx=0) → port "p_north" → area "north" → column 3
    // south (varIdx=1) → port "p_south" → area "south" → column 7
    // Both area-connected: no extra columns, two terms in row.

    PortType portType{"area_port_t", {PortField{"ens"}}, "ens"};

    Registry<Node> reg;

    auto* northNode = reg.create<VariableNode>("north_var", 0u);
    auto* southNode = reg.create<VariableNode>("south_var", 1u);
    auto* lit065 = reg.create<LiteralNode>(0.65);
    auto* lit035 = reg.create<LiteralNode>(0.35);
    auto* mulNorth = reg.create<MultiplicationNode>(lit065, northNode);
    auto* mulSouth = reg.create<MultiplicationNode>(lit035, southNode);
    auto* sumNode = reg.create<SumNode>(mulNorth, mulSouth);
    auto* lit300 = reg.create<LiteralNode>(300.0);
    auto* le = reg.create<LessThanOrEqualNode>(sumNode, lit300);
    Constraint constraint{"flow_based_constraint_ptdf", makeExpr(le, reg)};

    auto pfdNorth = makePortVarDef("p_north", "ens", 0u, portType, reg);
    auto pfdSouth = makePortVarDef("p_south", "ens", 1u, portType, reg);

    Variable vNorth{"north_var", Expression{}, Expression{}, ValueType::FLOAT,
                    TimeDependent::NO, ScenarioDependent::NO};
    Variable vSouth{"south_var", Expression{}, Expression{}, ValueType::FLOAT,
                    TimeDependent::NO, ScenarioDependent::NO};

    ModelBuilder mb;
    Model model = mb.withId("ptdf_model")
                    .withVariables({std::move(vNorth), std::move(vSouth)})
                    .withConstraints({std::move(constraint)})
                    .withPorts({Port{"p_north", portType}, Port{"p_south", portType}})
                    .withPortFieldDefinitions({std::move(pfdNorth), std::move(pfdSouth)})
                    .build();

    ComponentBuilder cb;
    Component comp = cb.withId("ccr_ptdf").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();
    comp.addAreaConnection("p_north", "north");
    comp.addAreaConnection("p_south", "south");

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap{{"north", 3}, {"south", 7}};
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);
    BOOST_CHECK(builder.allocated.empty()); // both area-connected, no extra columns

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    const CsrRow& row = rows[0];
    BOOST_CHECK(row.sense == CsrRowSense::LE);
    BOOST_CHECK_CLOSE(row.rhs, 300.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 2u);

    // colTerms in LinearExpr is std::map<int,double>; iteration is ascending by column.
    BOOST_CHECK_EQUAL(row.terms[0].column, 3);
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 0.65, 1e-9);
    BOOST_CHECK_EQUAL(row.terms[1].column, 7);
    BOOST_CHECK_CLOSE(row.terms[1].coefficient, 0.35, 1e-9);
}

BOOST_AUTO_TEST_CASE(zero_coefficient_term_dropped)
{
    // Constraint: "flow_based_constraint_zero: 0.0 * x <= 10.0"
    // The term for x has coefficient 0.0 and must be filtered out → terms is empty.
    Registry<Node> reg;
    auto* zero = reg.create<LiteralNode>(0.0);
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* mul = reg.create<MultiplicationNode>(zero, var);
    auto* rhs = reg.create<LiteralNode>(10.0);
    auto* le = reg.create<LessThanOrEqualNode>(mul, rhs);
    Constraint constraint{"flow_based_constraint_zero", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_zero").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("c_zero").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK(rows[0].terms.empty()); // zero-coefficient term filtered
    BOOST_CHECK_CLOSE(rows[0].rhs, 10.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(multiple_constraints_from_one_component)
{
    // Model with two FBC constraints → rowsForHour returns two rows.
    Registry<Node> reg;
    auto* varA = reg.create<VariableNode>("a", 0u);
    auto* litA = reg.create<LiteralNode>(80.0);
    auto constraintA = makeLEConstraint("flow_based_constraint_a", varA, litA, reg);

    auto* varB = reg.create<VariableNode>("b", 1u);
    auto* litB = reg.create<LiteralNode>(120.0);
    auto constraintB = makeLEConstraint("flow_based_constraint_b", varB, litB, reg);

    Variable vA{"a", Expression{}, Expression{}, ValueType::FLOAT,
                TimeDependent::NO, ScenarioDependent::NO};
    Variable vB{"b", Expression{}, Expression{}, ValueType::FLOAT,
                TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_multi")
      .withVariables({std::move(vA), std::move(vB)})
      .withConstraints({std::move(constraintA), std::move(constraintB)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("c_multi").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 2u);
    BOOST_CHECK_CLOSE(rows[0].rhs + rows[1].rhs, 80.0 + 120.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(two_components_produce_rows_from_both)
{
    // Two components in the system, each with one FBC constraint → two rows total.
    Registry<Node> reg;

    auto* var1 = reg.create<VariableNode>("x1", 0u);
    auto* lit1 = reg.create<LiteralNode>(60.0);
    auto c1 = makeLEConstraint("flow_based_constraint_c1", var1, lit1, reg);

    Variable v1{"x1", Expression{}, Expression{}, ValueType::FLOAT,
                TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb1;
    mb1.withId("model1").withVariables({std::move(v1)}).withConstraints({std::move(c1)});
    Model model1 = mb1.build();

    auto* var2 = reg.create<VariableNode>("x2", 0u);
    auto* lit2 = reg.create<LiteralNode>(90.0);
    auto c2 = makeLEConstraint("flow_based_constraint_c2", var2, lit2, reg);

    Variable v2{"x2", Expression{}, Expression{}, ValueType::FLOAT,
                TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb2;
    mb2.withId("model2").withVariables({std::move(v2)}).withConstraints({std::move(c2)});
    Model model2 = mb2.build();

    ComponentBuilder cb;
    Component comp1 = cb.withId("comp_a").withModel(&model1).withIndex(0)
                        .withParameterValues({}).build();
    Component comp2 = cb.withId("comp_b").withModel(&model2).withIndex(1)
                        .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp1));
    comps.push_back(std::move(comp2));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);
    BOOST_CHECK_EQUAL(builder.allocated.size(), 2u); // one extra column per component

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 2u);
}

BOOST_AUTO_TEST_CASE(unknown_area_name_falls_back_to_extra_column)
{
    // Port "p1" is connected to "unknown_area" which is not in areaToColumnIndex.
    // buildAreaVarMap logs a warning and skips → variable gets an extra column.
    PortType portType{"area_port_t", {PortField{"ens"}}, "ens"};

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(50.0);
    auto constraint = makeLEConstraint("flow_based_constraint_uk", var, lit, reg);

    auto pfd = makePortVarDef("p1", "ens", 0u, portType, reg);

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_uk")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withPorts({Port{"p1", portType}})
      .withPortFieldDefinitions({std::move(pfd)});
    Model model = mb.build();

    ComponentBuilder cb;
    Component comp = cb.withId("c_uk").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();
    comp.addAreaConnection("p1", "unknown_area");

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    // areaMap does NOT contain "unknown_area"
    std::map<std::string, int> areaMap{{"north", 0}};
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);
    // Variable should fall back to an extra column
    BOOST_REQUIRE_EQUAL(builder.allocated.size(), 1u);
    BOOST_CHECK_EQUAL(builder.allocated[0].name, "c_uk.x");

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK_EQUAL(rows[0].terms[0].column, 0); // first (and only) extra column
}

BOOST_AUTO_TEST_CASE(varying_in_time_only_reads_from_dataseries)
{
    // VARYING_IN_TIME_ONLY parameter: adapter calls getData(dataSetId, 0, hour)
    using Antares::Optimisation::VariabilityType;

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* param = reg.create<ParameterNode>("hourly_cap",
                                            VariabilityType::VARYING_IN_TIME_ONLY);
    auto* le = reg.create<LessThanOrEqualNode>(var, param);
    Constraint constraint{"flow_based_constraint_time", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_time")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withParameters({Parameter{"hourly_cap", TimeDependent::YES, ScenarioDependent::NO}});
    Model model = mb.build();

    std::map<std::string, ParameterTypeAndValue> paramValues{
      {"hourly_cap", {.id = "hourly_cap",
                      .type = VariabilityType::VARYING_IN_TIME_ONLY,
                      .value = "hourly_dataset"}}};

    ComponentBuilder cb;
    Component comp = cb.withId("c_time").withModel(&model).withIndex(0)
                       .withParameterValues(std::move(paramValues)).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    MockLinearProblemData ds;
    // VARYING_IN_TIME_ONLY: getData("hourly_dataset", 0, hour) — mock ignores hour,
    // keyed on ("hourly_dataset", tsNumber=0).
    ds.values[{"hourly_dataset", 0u}] = 42.0;

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, &ds};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    // Any (hour, mcYear) → tsNumber=0 for time-only parameters.
    auto rows = adapter.rowsForHour(5, 3);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);
    BOOST_CHECK_CLOSE(rows[0].rhs, 42.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(varying_in_time_and_scenario_reads_from_dataseries)
{
    // VARYING_IN_TIME_AND_SCENARIO: adapter calls getData(dataSetId, tsNumber, hour).
    // tsNumber = mcYear + 1 (1-based convention).
    using Antares::Optimisation::VariabilityType;

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* param = reg.create<ParameterNode>("ts_cap",
                                            VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    auto* le = reg.create<LessThanOrEqualNode>(var, param);
    Constraint constraint{"flow_based_constraint_ts", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_ts")
      .withVariables({std::move(v)})
      .withConstraints({std::move(constraint)})
      .withParameters({Parameter{"ts_cap", TimeDependent::YES, ScenarioDependent::YES}});
    Model model = mb.build();

    std::map<std::string, ParameterTypeAndValue> paramValues{
      {"ts_cap", {.id = "ts_cap",
                  .type = VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                  .value = "ts_dataset"}}};

    ComponentBuilder cb;
    Component comp = cb.withId("c_ts").withModel(&model).withIndex(0)
                       .withParameterValues(std::move(paramValues)).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    MockLinearProblemData ds;
    ds.values[{"ts_dataset", 1u}] = 100.0; // mcYear=0 → tsNumber=1
    ds.values[{"ts_dataset", 2u}] = 200.0; // mcYear=1 → tsNumber=2

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, &ds};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows0 = adapter.rowsForHour(0, 0); // tsNumber=1
    BOOST_REQUIRE_EQUAL(rows0.size(), 1u);
    BOOST_CHECK_CLOSE(rows0[0].rhs, 100.0, 1e-9);

    auto rows1 = adapter.rowsForHour(0, 1); // tsNumber=2
    BOOST_REQUIRE_EQUAL(rows1.size(), 1u);
    BOOST_CHECK_CLOSE(rows1[0].rhs, 200.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(missing_parameter_returns_empty_rows)
{
    // Parameter "missing_cap" is referenced in the constraint expression but absent
    // from the component's parameter map → resolveParameter throws → buildRow returns
    // false → rowsForHour returns no rows.
    using Optimisation::VariabilityType;

    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* param = reg.create<ParameterNode>("missing_cap",
                                            VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    auto* le = reg.create<LessThanOrEqualNode>(var, param);
    Constraint constraint{"flow_based_constraint_missing", makeExpr(le, reg)};

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    mb.withId("m_missing").withVariables({std::move(v)}).withConstraints({std::move(constraint)});
    Model model = mb.build();

    // Model declares no parameters; paramValues is empty → sizes match and build() passes.
    // The ParameterNode("missing_cap") is resolved at rowsForHour time: resolveParameter
    // finds nothing in the empty map and throws, which buildRow catches → no row emitted.
    ComponentBuilder cb;
    Component comp = cb.withId("c_missing").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    // resolveParameter("missing_cap") throws (not found); buildRow catches → no row.
    auto rows = adapter.rowsForHour(0, 0);
    BOOST_CHECK(rows.empty());
}

BOOST_AUTO_TEST_SUITE_END()