#pragma once
#include <string>

#include <antares/solver/expressions/Node.h>

class Parameter: public Node
{
public:
    virtual ~Parameter() = default;

    Parameter(const std::string name);

    std::any accept(Visitor& visitor) override;

    // private:
    std::string name;
};
