// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <boost/test/unit_test.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/modeler/checks/checkPortFields.h>
#include <antares/study/system-model/model.h>

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::Checks;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Solver::Config;

namespace
{
template<class NodeType>
Expression makePortExpression(const std::string& field, const std::string& text)
{
    Antares::Expressions::Registry<Node> registry;
    Node* node = registry.create<NodeType>("port", field);
    return Expression(text, Antares::Expressions::NodeRegistry(node, std::move(registry)));
}
} // namespace

BOOST_AUTO_TEST_SUITE(check_port_fields)

BOOST_AUTO_TEST_CASE(reports_all_direct_port_fields)
{
    std::vector<Constraint> constraints;
    constraints.emplace_back("c1",
                             makePortExpression<PortFieldNode>("field1", "port.field1"),
                             Location::SUBPROBLEMS);
    std::vector<Objective> objectives;
    objectives.emplace_back("obj", makePortExpression<PortFieldNode>("field2", "port.field2"));
    std::vector<ExtraOutput> outputs;
    outputs.emplace_back("output", makePortExpression<PortFieldNode>("field3", "port.field3"));

    auto model = ModelBuilder()
                   .withId("test_model")
                   .withConstraints(std::move(constraints))
                   .withObjectives(std::move(objectives))
                   .withExtraOutputs(std::move(outputs))
                   .build();

    BOOST_CHECK_EXCEPTION(checkModelPortFields(model),
                          PortFieldError,
                          [](const std::exception& e)
                          {
                              std::string msg = e.what();
                              return msg.find("Model 'test_model'") != std::string::npos
                                     && msg.find("port.field1") != std::string::npos
                                     && msg.find("port.field2") != std::string::npos
                                     && msg.find("port.field3") != std::string::npos
                                     && msg.find("sum_connections(port.field1)")
                                          != std::string::npos;
                          });
}

BOOST_AUTO_TEST_CASE(allows_sum_connections)
{
    std::vector<Constraint> constraints;
    constraints.emplace_back("c",
                             makePortExpression<PortFieldSumNode>("field",
                                                                  "sum_connections(port.field)"),
                             Location::SUBPROBLEMS);
    auto model = ModelBuilder()
                   .withId("test_model")
                   .withConstraints(std::move(constraints))
                   .build();
    BOOST_CHECK_NO_THROW(checkModelPortFields(model));
}

BOOST_AUTO_TEST_CASE(checks_all_models_in_libraries)
{
    std::vector<Model> models;
    for (const auto& field: {"first", "second"})
    {
        std::vector<Constraint> constraints;
        constraints.emplace_back("c",
                                 makePortExpression<PortFieldNode>(field, field),
                                 Location::SUBPROBLEMS);
        models.push_back(
          ModelBuilder().withId(field).withConstraints(std::move(constraints)).build());
    }
    Antares::Solver::ModelerData data;
    data.libraries.push_back(
      LibraryBuilder().withId("library").withModels(std::move(models)).build());
    BOOST_CHECK_EXCEPTION(checkPortFields(data),
                          PortFieldError,
                          [](const std::exception& e)
                          {
                              const std::string message = e.what();
                              return message.find("port.first") != std::string::npos
                                     && message.find("port.second") != std::string::npos;
                          });
}

BOOST_AUTO_TEST_CASE(checks_port_field_definitions)
{
    PortField field("field");
    PortType type("type", std::vector<PortField>{field}, {"field", "", "", ""});
    Port port("port", type);
    std::vector<PortFieldDefinition> definitions;
    definitions.emplace_back(port, field, makePortExpression<PortFieldNode>("other", "port.other"));
    auto model = ModelBuilder()
                   .withId("test_model")
                   .withPortFieldDefinitions(std::move(definitions))
                   .build();
    BOOST_CHECK_EXCEPTION(checkModelPortFields(model),
                          PortFieldError,
                          [](const std::exception& e) {
                              return std::string(e.what()).find("port.other") != std::string::npos;
                          });
}

BOOST_AUTO_TEST_SUITE_END()
