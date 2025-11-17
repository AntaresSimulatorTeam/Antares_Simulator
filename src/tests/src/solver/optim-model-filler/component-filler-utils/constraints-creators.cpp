#include "constraints-creators.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler;

std::vector<Constraint> TwoConstraintsCreator_OneSubPb_OneMaster::Create(
  Antares::Expressions::Registry<Node>& nodeRegistry)
{
    auto greaterNode = nodeRegistry.create<GreaterThanOrEqualNode>(
      nodeRegistry.create<LiteralNode>(1.0),
      nodeRegistry.create<LiteralNode>(0.0));
    Constraint constr_1("constraint-1",
                        Expression("1 >= 0", NodeRegistry(greaterNode, std::move(nodeRegistry))));

    auto greaterNode2 = nodeRegistry.create<GreaterThanOrEqualNode>(
      nodeRegistry.create<LiteralNode>(2.0),
      nodeRegistry.create<LiteralNode>(0.0));
    Constraint constr_2("constraint-2",
                        Expression("2 >= 0", NodeRegistry(greaterNode2, std::move(nodeRegistry))));

    constr_1.setLocation(Config::Location::SUBPROBLEMS);
    constr_2.setLocation(Config::Location::MASTER);

    std::vector<Constraint> constraints;
    constraints.emplace_back(std::move(constr_1));
    constraints.emplace_back(std::move(constr_2));

    return constraints;
}

std::vector<Constraint> NoConstraintCreator::Create(Antares::Expressions::Registry<Node>&)
{
    return {};
}
