#pragma once

#include <antares/solver/expressions/Visitor.h>

namespace Antares::Solver::Expressions
{
class PrintVisitor: public Visitor
{
    void visit(const Add& add) override;

    void visit(const Negate& neg) override;

    void visit(const Parameter& param) override;
};
} // namespace Antares::Solver::Expressions
