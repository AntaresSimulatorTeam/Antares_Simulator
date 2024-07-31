#pragma once

#include <antares/solver/expressions/Visitor.h>

namespace Antares::Solver::Expressions
{
class StringVisitor: public Visitor
{
    std::any visit(const Add& add) override;

    std::any visit(const Negate& neg) override;

    std::any visit(const Parameter& param) override;
};
} // namespace Antares::Solver::Expressions
