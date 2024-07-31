#include <antares/solver/expressions/Parameter.h>

namespace Antares::Solver::Expressions
{
Parameter::Parameter(const std::string name):
    name(name)
{
}

std::any Parameter::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
} // namespace Antares::Solver::Expressions
