#include "objectives-creators.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;
using namespace Antares::Expressions;
using namespace Antares::Modeler;

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

std::vector<Objective> NoObjectiveCreator::Create(Registry<Nodes::Node>&)
{
    return {};
}

std::vector<Objective> TwoObjsCreator_OneSubPb_OneMaster::Create(
  Registry<Nodes::Node>& nodeRegistry)
{
    std::vector<Objective> objectives;
    auto obj_1 = makeObjectiveFromVariable(nodeRegistry,
                                           "var-1",
                                           0,
                                           "obj-1",
                                           Config::Location::SUBPROBLEMS);
    auto obj_2 = makeObjectiveFromVariable(nodeRegistry,
                                           "var-2",
                                           1,
                                           "obj-2",
                                           Config::Location::MASTER);

    objectives.emplace_back(std::move(obj_1));
    objectives.emplace_back(std::move(obj_2));
    return objectives;
}

std::vector<Objective> TwoSubPbObjsCreator::Create(Registry<Nodes::Node>& nodeRegistry)
{
    std::vector<Objective> objectives;
    auto obj_1 = makeObjectiveFromVariable(nodeRegistry,
                                           "var-1",
                                           0,
                                           "obj-1",
                                           Config::Location::SUBPROBLEMS);
    auto obj_2 = makeObjectiveFromVariable(nodeRegistry,
                                           "var-2",
                                           1,
                                           "obj-2",
                                           Config::Location::SUBPROBLEMS);

    objectives.emplace_back(std::move(obj_1));
    objectives.emplace_back(std::move(obj_2));
    return objectives;
}
