#pragma once
#include <memory>

#include <antares/solver/expressions/Node.h>

namespace Antares::Solver::Expressions
{
class Negate: public Node
{
public:
    virtual ~Negate() = default;

    Negate(std::shared_ptr<Node> n);

    std::any accept(Visitor& visitor) override;

    // private:
    std::shared_ptr<Node> n;
};
} // namespace Antares::Solver::Expressions
