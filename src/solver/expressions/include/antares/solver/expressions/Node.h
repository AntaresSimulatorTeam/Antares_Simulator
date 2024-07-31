#pragma once
#include <antares/solver/expressions/Visitor.h>

class Node
{
public:
    virtual ~Node() = default;
    virtual std::any accept(Visitor& visitor) = 0;
};
