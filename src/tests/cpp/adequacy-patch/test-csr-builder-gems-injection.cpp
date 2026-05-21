// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_csr_builder_gems_injection
#define WIN32_LEAN_AND_MEAN

#include <map>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/NodeRegistry.h>
#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/LessThanOrEqualNode.h>
#include <antares/expressions/nodes/LiteralNode.h>
#include <antares/expressions/nodes/MultiplicationNode.h>
#include <antares/expressions/nodes/VariableNode.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/constraint.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/port.h>
#include <antares/study/system-model/portFieldDefinition.h>
#include <antares/study/system-model/portType.h>
#include <antares/study/system-model/system.h>
#include <antares/study/system-model/variable.h>

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"

using namespace Antares;
using namespace Antares::AdequacyPatch;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

// ---------------------------------------------------------------------------
// Minimal helpers (mirrors helpers in test-gems-csr-adapter.cpp)
// ---------------------------------------------------------------------------

static Expression makeExpr(Node* root, Registry<Node>& reg)
{
    NodeRegistry nr{root, std::move(reg)};
    return Expression{"expr", std::move(nr)};
}

static Constraint makeLEConstraint(const std::string& id,
                                   Node* lhs,
                                   Node* rhs,
                                   Registry<Node>& reg)
{
    auto* le = reg.create<LessThanOrEqualNode>(lhs, rhs);
    return Constraint{id, makeExpr(le, reg)};
}

template<typename First, typename... Rest>
static auto moveVec(First&& first, Rest&&... rest)
{
    using T = std::decay_t<First>;
    std::vector<T> v;
    v.reserve(1 + sizeof...(rest));
    v.push_back(std::forward<First>(first));
    (v.push_back(std::forward<Rest>(rest)), ...);
    return v;
}

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
        int col = nextCol_++;
        allocated.push_back({name, lb, ub});
        return col;
    }

    std::vector<ColInfo> allocated;

private:
    int nextCol_ = 0;
};

// ---------------------------------------------------------------------------
// Build a system with:
//   - 1 component "ccr"
//   - model has 2 variables: var0 (area-connected via port "p_north" → "north"),
//                             var1 (internal — not area-connected)
//   - 1 constraint "flow_based_constraint_test": 2.0 * var1 <= 50.0
// ---------------------------------------------------------------------------
struct Fixture
{
    PortType portType{"area_port_t", {PortField{"ens"}}, "ens"};
    Model model; // must outlive system (components hold a Model*)
    std::optional<System> system;
    std::map<std::string, int> areaMap; // "north"→0, "south"→1

    Fixture()
    {
        // constraint expression: 2.0 * var1 <= 50.0
        auto* var1Node = reg.create<VariableNode>("var1", 1u);
        auto* two = reg.create<LiteralNode>(2.0);
        auto* mul = reg.create<MultiplicationNode>(two, var1Node);
        auto* fifty = reg.create<LiteralNode>(50.0);
        auto constraint = makeLEConstraint("flow_based_constraint_test", mul, fifty, reg);

        Variable v0{"var0", Expression{}, Expression{}, ValueType::FLOAT,
                    TimeDependent::NO, ScenarioDependent::NO};
        Variable v1{"var1", Expression{}, Expression{}, ValueType::FLOAT,
                    TimeDependent::NO, ScenarioDependent::NO};

        // port-field definition: p_north.ens = VariableNode(0)
        auto pfd = makePortVarDef("p_north", "ens", 0u, portType, reg);

        ModelBuilder mb;
        model = mb.withId("fb_model")
                  .withVariables(moveVec(std::move(v0), std::move(v1)))
                  .withConstraints(moveVec(std::move(constraint)))
                  .withPorts({Port{"p_north", portType}})
                  .withPortFieldDefinitions(moveVec(std::move(pfd)))
                  .build();

        ComponentBuilder cb;
        Component comp = cb.withId("ccr")
                           .withModel(&model)
                           .withIndex(0)
                           .withParameterValues({})
                           .build();
        comp.addAreaConnection("p_north", "north");

        std::vector<Component> comps;
        comps.push_back(std::move(comp));
        system = SystemBuilder{}.withId("sys").withComponents(std::move(comps)).build();

        areaMap["north"] = 0;
        areaMap["south"] = 1;
    }

    Registry<Node> reg;
};

// ---------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE(CsrBuilderGemsInjectionSuite)
// ---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(counts_one_matching_constraint, Fixture)
{
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{*system, ctx};

    BOOST_CHECK_EQUAL(adapter.countMatchingConstraints(), 1);
}

BOOST_FIXTURE_TEST_CASE(counts_one_extra_variable, Fixture)
{
    // var0 is area-connected (maps to column 0 in areaMap) → not extra.
    // var1 has no area connection → extra.
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{*system, ctx};

    BOOST_CHECK_EQUAL(adapter.countExtraVariables(), 1);
}

BOOST_FIXTURE_TEST_CASE(register_allocates_one_column, Fixture)
{
    // Simulate "legacy columns are 0..1" by starting MockBuilder at 0.
    // The adapter already mapped var0 → area column 0; only var1 gets a new column.
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{*system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    BOOST_REQUIRE_EQUAL(builder.allocated.size(), 1u);
    BOOST_CHECK_EQUAL(builder.allocated[0].name, "ccr.var1");
}

BOOST_FIXTURE_TEST_CASE(rows_for_hour_has_correct_coefficient_and_rhs, Fixture)
{
    // The constraint is: 2.0 * var1 <= 50.0
    // After registerExtraVariables, var1 gets column index 0 (first allocated).
    CsrProblemContext ctx{&areaMap, nullptr};
    GemsCsrAdapter adapter{*system, ctx};

    MockBuilder builder;
    adapter.registerExtraVariables(builder);

    auto rows = adapter.rowsForHour(0, 0);
    BOOST_REQUIRE_EQUAL(rows.size(), 1u);

    const CsrRow& row = rows[0];
    BOOST_CHECK(row.sense == CsrRowSense::LE);
    BOOST_CHECK_CLOSE(row.rhs, 50.0, 1e-9);
    BOOST_REQUIRE_EQUAL(row.terms.size(), 1u);
    BOOST_CHECK_EQUAL(row.terms[0].column, 0); // first extra column allocated
    BOOST_CHECK_CLOSE(row.terms[0].coefficient, 2.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(flag_false_no_extra_column_or_row)
{
    // When the filter matches nothing, the adapter reports 0 extras and 0 rows.
    // This mirrors the useGemsFbConstraints=false guard in countVariables/countConstraints.
    Registry<Node> reg;
    auto* var = reg.create<VariableNode>("x", 0u);
    auto* lit = reg.create<LiteralNode>(99.0);
    auto constraint = makeLEConstraint("some_unrelated_constraint", var, lit, reg);

    Variable v{"x", Expression{}, Expression{}, ValueType::FLOAT,
               TimeDependent::NO, ScenarioDependent::NO};
    ModelBuilder mb;
    Model model = mb.withId("m").withVariables(moveVec(std::move(v)))
                    .withConstraints(moveVec(std::move(constraint))).build();

    ComponentBuilder cb;
    Component comp = cb.withId("comp").withModel(&model).withIndex(0)
                       .withParameterValues({}).build();

    std::vector<Component> comps;
    comps.push_back(std::move(comp));
    System sys = SystemBuilder{}.withId("s").withComponents(std::move(comps)).build();

    std::map<std::string, int> areaMap;
    CsrProblemContext ctx{&areaMap, nullptr};
    // Default filter "^flow_based_constraint_" does NOT match "some_unrelated_constraint"
    GemsCsrAdapter adapter{sys, ctx};

    BOOST_CHECK_EQUAL(adapter.countMatchingConstraints(), 0);
    BOOST_CHECK_EQUAL(adapter.countExtraVariables(), 0);

    MockBuilder builder;
    adapter.registerExtraVariables(builder);
    BOOST_CHECK(builder.allocated.empty());

    // rowsForHour before registerExtraVariables on a fresh adapter with area map
    GemsCsrAdapter adapter2{sys, ctx};
    auto rows = adapter2.rowsForHour(0, 0);
    BOOST_CHECK(rows.empty());
}

BOOST_AUTO_TEST_SUITE_END()