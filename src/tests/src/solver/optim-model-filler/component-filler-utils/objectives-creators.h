#pragma once

#include <antares/study/system-model-base/variabilityType.h>
#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/study/system-model/objective.h"

using namespace Antares;

struct NoObjectiveCreator
{
    static std::vector<ModelerStudy::SystemModel::Objective> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};

struct TwoObjsCreator_OneSubPb_OneMaster
{
    static std::vector<ModelerStudy::SystemModel::Objective> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};

struct TwoSubPbObjsCreator
{
    static std::vector<ModelerStudy::SystemModel::Objective> Create(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry);
};
