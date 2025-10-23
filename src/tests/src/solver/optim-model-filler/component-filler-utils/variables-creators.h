#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/modeler/optimConfig/optimConfig.h"
#include "antares/study/system-model/variable.h"

using namespace Antares;

class VariablesCreator
{
public:
    explicit VariablesCreator(Expressions::Registry<Expressions::Nodes::Node>& nodeRegistry):
        nodeRegistry_(nodeRegistry)
    {
    }

    virtual std::vector<ModelerStudy::SystemModel::Variable> create() = 0;

protected:
    Expressions::Registry<Expressions::Nodes::Node>& nodeRegistry_;
};

class TwoVarsCreator_OneSubPb_OneMaster: public VariablesCreator
{
    using VariablesCreator::VariablesCreator;

public:
    std::vector<ModelerStudy::SystemModel::Variable> create() override;
};

class TwoSubPbVarsCreator: public VariablesCreator
{
    using VariablesCreator::VariablesCreator;

public:
    std::vector<ModelerStudy::SystemModel::Variable> create() override;
};
