#pragma once
#include <antares/solver/expressions/Node.h>

class Parameter: public Node
{
public:
    virtual ~Parameter() = default;

    Parameter(const std::string name):
        name(name)
    {
    }

    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    // private:
    std::string name;
};
