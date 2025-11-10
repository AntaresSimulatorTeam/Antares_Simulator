#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/study/system-model/variable.h"

using namespace Antares;

struct TwoVarsCreator_OneSubPb_OneMaster
{
    static std::vector<ModelerStudy::SystemModel::Variable> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};

struct TwoSubPbVarsCreator
{
    static std::vector<ModelerStudy::SystemModel::Variable> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};
