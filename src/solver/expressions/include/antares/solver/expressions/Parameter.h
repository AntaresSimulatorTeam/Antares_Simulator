#pragma once
#include <memory>
#include <string>

#include <antares/solver/expressions/Node.h>

namespace Antares::Solver::Expressions
{
class Parameter: public Node
{
public:
    virtual ~Parameter() = default;

    Parameter(const std::string name);

    std::any accept(Visitor& visitor) override;

    // private:
    std::string name;
};
} // namespace Antares::Solver::Expressions
