#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/study/system-model/variable.h"
#include "antares/modeler/optimConfig/optimConfig.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler;

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
