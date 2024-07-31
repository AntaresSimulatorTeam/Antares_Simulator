#pragma once
#include <antares/solver/expressions/Node.h>

class Negate: public Node
{
public:
    virtual ~Negate() = default;

    Negate(Node* n):
        n(n)
    {
    }

    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    // private:
    std::unique_ptr<Node> n;
};
