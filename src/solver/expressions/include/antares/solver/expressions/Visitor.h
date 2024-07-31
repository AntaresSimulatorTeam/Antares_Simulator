#pragma once
#include <any>

namespace Antares::Solver::Expressions
{
class Add;
class Negate;
class Parameter;

class Visitor
{
public:
    virtual std::any visit(const Add&) = 0;
    virtual std::any visit(const Negate&) = 0;
    virtual std::any visit(const Parameter&) = 0;
};
} // namespace Antares::Solver::Expressions
