#pragma once

#include "antares/expressions/expression.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/modeler/optimConfig/optimConfig.h"
#include "antares/modeler-optimisation-container/TimeIndex.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;
using namespace Antares::Expressions;

class ObjectivesCreator
{
public:
    explicit ObjectivesCreator(Registry<Nodes::Node>& nodeRegistry):
        nodeRegistry_(nodeRegistry)
    {
    }

    virtual std::vector<Objective> create() = 0;

protected:
    Registry<Nodes::Node>& nodeRegistry_;
};

class NoObjectiveCreator: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<Objective> create() override
    {
        return {};
    }
};

Objective makeObjectiveFromVariable(Registry<Nodes::Node>& nodeRegistry,
                                    const std::string varId,
                                    unsigned varIndex,
                                    const std::string objectiveId,
                                    Config::Location locoation)
{
    auto varNode = nodeRegistry.create<Nodes::VariableNode>(varId,
                                                            varIndex,
                                                            TimeIndex::VARYING_IN_TIME_ONLY);
    NodeRegistry node_registry(varNode, std::move(nodeRegistry));
    Expression expression("expr-" + objectiveId, std::move(node_registry));
    Objective objective(objectiveId, std::move(expression), locoation);
    return objective;
}

class TwoObjsCreator_OneSubPb_OneMaster: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<Objective> create() override
    {
        std::vector<Objective> objectives;
        auto obj_1 = makeObjectiveFromVariable(nodeRegistry_,
                                               "var-1",
                                               0,
                                               "obj-1",
                                               Config::Location::SUBPROBLEMS);
        auto obj_2 = makeObjectiveFromVariable(nodeRegistry_,
                                               "var-2",
                                               1,
                                               "obj-2",
                                               Config::Location::MASTER);

        objectives.emplace_back(std::move(obj_1));
        objectives.emplace_back(std::move(obj_2));
        return objectives;
    }
};

class TwoSubPbObjsCreator: public ObjectivesCreator
{
    using ObjectivesCreator::ObjectivesCreator;

public:
    std::vector<Objective> create() override
    {
        std::vector<Objective> objectives;
        auto obj_1 = makeObjectiveFromVariable(nodeRegistry_,
                                               "var-1",
                                               0,
                                               "obj-1",
                                               Config::Location::SUBPROBLEMS);
        auto obj_2 = makeObjectiveFromVariable(nodeRegistry_,
                                               "var-2",
                                               1,
                                               "obj-2",
                                               Config::Location::SUBPROBLEMS);

        objectives.emplace_back(std::move(obj_1));
        objectives.emplace_back(std::move(obj_2));
        return objectives;
    }
};
