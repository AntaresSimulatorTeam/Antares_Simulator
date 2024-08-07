#pragma once

#include <Eigen/Dense>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Expressions
{
class LiteralNode: public Leaf<Eigen::MatrixXd>
{
public:
    using Leaf<Eigen::MatrixXd>::Leaf;

    LiteralNode(double x):
        Leaf<Eigen::MatrixXd>::Leaf(Eigen::MatrixXd(x))
    {
    }
};
} // namespace Antares::Solver::Expressions
