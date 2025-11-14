#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/study/system-model/constraint.h"

using namespace Antares;

struct TwoConstraintsCreator_OneSubPb_OneMaster
{
    static std::vector<ModelerStudy::SystemModel::Constraint> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};
