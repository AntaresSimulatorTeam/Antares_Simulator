#pragma once
#include <memory>

#include <antares/solver/expressions/Node.h>

namespace Antares::Solver::Expressions
{
class Add: public Node
{
public:
    virtual ~Add() = default;

    Add(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);

    void accept(Visitor& visitor) override;

    // private:
    std::shared_ptr<Node> n1, n2;
};
} // namespace Antares::Solver::Expressions
