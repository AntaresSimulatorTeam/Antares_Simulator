#include "constraints-creators.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler;

std::vector<Constraint> TwoConstraintsCreator_OneSubPb_OneMaster::Create(
  Antares::Expressions::Registry<Node>& nodeRegistry)
{
    Constraint constr_1("constraint-1",
                        Expression("expr-1",
                                   NodeRegistry(nodeRegistry.create<LiteralNode>(0.),
                                                std::move(nodeRegistry))));

    Constraint constr_2("constraint-2",
                        Expression("expr-2",
                                   NodeRegistry(nodeRegistry.create<LiteralNode>(0.),
                                                std::move(nodeRegistry))));
    constr_1.setLocation(Config::Location::SUBPROBLEMS);
    constr_2.setLocation(Config::Location::MASTER);

    std::vector<Constraint> constraints;
    constraints.emplace_back(std::move(constr_1));
    constraints.emplace_back(std::move(constr_2));

    return constraints;
}
