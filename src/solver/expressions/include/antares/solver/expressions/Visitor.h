#pragma once

namespace Antares::Solver::Expressions
{
class Add;
class Negate;
class Parameter;

class Visitor
{
public:
    virtual void visit(const Add&) = 0;
    virtual void visit(const Negate&) = 0;
    virtual void visit(const Parameter&) = 0;
};
} // namespace Antares::Solver::Expressions
