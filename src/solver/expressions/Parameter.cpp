#include <antares/solver/expressions/Parameter.h>

Parameter::Parameter(const std::string name):
    name(name)
{
}

std::any Parameter::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
