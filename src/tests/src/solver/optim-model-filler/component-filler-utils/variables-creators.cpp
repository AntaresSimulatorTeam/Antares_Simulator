#include "variables-creators.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler;

Expression createLiteral(std::string name, double value, Registry<Nodes::Node>& nodeRegistry)
{
    NodeRegistry node_registry(nodeRegistry.create<LiteralNode>(value), std::move(nodeRegistry));
    return Expression(name, std::move(node_registry));
}

std::vector<Variable> TwoVarsCreator_OneSubPb_OneMaster::Create(
  Antares::Expressions::Registry<Node>& nodeRegistry)
{
    Variable var_1("var-1",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::FLOAT,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::SUBPROBLEMS);
    Variable var_2("var-2",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::FLOAT,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::MASTER);

    std::vector<Variable> variables;
    variables.emplace_back(std::move(var_1));
    variables.emplace_back(std::move(var_2));
    return variables;
}

std::vector<Variable> TwoSubPbVarsCreator::Create(
  Antares::Expressions::Registry<Node>& nodeRegistry)
{
    Variable var_1("var-1",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::FLOAT,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::SUBPROBLEMS);
    Variable var_2("var-2",
                   createLiteral("low-bound", 0., nodeRegistry),
                   createLiteral("up-bound", 1., nodeRegistry),
                   ValueType::FLOAT,
                   TimeDependent::NO,
                   ScenarioDependent::NO,
                   Config::Location::SUBPROBLEMS);

    std::vector<Variable> variables;
    variables.emplace_back(std::move(var_1));
    variables.emplace_back(std::move(var_2));
    return variables;
}
