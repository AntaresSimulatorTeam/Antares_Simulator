#pragma once

#include <string>

#include <antares/solver/expressions/Visitor.h>

namespace Antares::Solver::Expressions
{
class StringVisitor: public Visitor
{
public:
    void visit(const Add& add) override;
    void visit(const Negate& neg) override;
    void visit(const Parameter& param) override;

    const std::string& str() const
    {
        return str_;
    }

private:
    std::string str_;
};
} // namespace Antares::Solver::Expressions
