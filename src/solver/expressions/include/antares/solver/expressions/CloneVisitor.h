#pragma once

#include <memory>

#include <antares/solver/expressions/Node.h>
#include <antares/solver/expressions/Visitor.h>

namespace Antares::Solver::Expressions
{
class CloneVisitor: public Visitor
{
public:
    void visit(const Add& add) override;
    void visit(const Negate& neg) override;
    void visit(const Parameter& param) override;

    std::shared_ptr<Node> ptr() const
    {
        return ptr_;
    }

private:
    std::shared_ptr<Node> ptr_;
};
} // namespace Antares::Solver::Expressions
