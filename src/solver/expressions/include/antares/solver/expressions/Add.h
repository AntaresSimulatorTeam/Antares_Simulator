#pragma once
#include <memory>

#include <antares/solver/expressions/Node.h>

class Add: public Node
{
public:
    virtual ~Add() = default;

    Add(Node* n1, Node* n2);

    std::any accept(Visitor& visitor) override;

    // private:
    std::unique_ptr<Node> n1, n2;
};
