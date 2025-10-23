#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/modeler/optimConfig/optimConfig.h"
#include "antares/modeler-optimisation-container/TimeIndex.h"
#include "antares/study/system-model/objective.h"

using namespace Antares;

class ObjectivesCreator
{
public:
    explicit ObjectivesCreator(Expressions::Registry<Expressions::Nodes::Node>& nodeRegistry):
        nodeRegistry_(nodeRegistry)
    {
    }

    virtual std::vector<ModelerStudy::SystemModel::Objective> create() = 0;

protected:
    Expressions::Registry<Expressions::Nodes::Node>& nodeRegistry_;
};

class NoObjectiveCreator: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<ModelerStudy::SystemModel::Objective> create() override;
};

class TwoObjsCreator_OneSubPb_OneMaster: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<ModelerStudy::SystemModel::Objective> create() override;
};

class TwoSubPbObjsCreator: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<ModelerStudy::SystemModel::Objective> create() override;
};
