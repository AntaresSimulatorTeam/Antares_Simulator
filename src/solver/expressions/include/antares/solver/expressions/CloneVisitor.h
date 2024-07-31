#pragma once

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/Visitor.h>

class CloneVisitor: public Visitor
{
    std::any visit(const Add& add) override
    {
        auto* n1 = std::any_cast<Node*>(add.n1->accept(*this));
        Node* n2 = std::any_cast<Node*>(add.n2->accept(*this));

        Node* result = new Add(n1, n2);
        return result;
    }

    std::any visit(const Negate& neg) override
    {
        Node* n = std::any_cast<Node*>(neg.n->accept(*this));
        Node* result = new Negate(n);
        return result;
    }

    std::any visit(const Parameter& param) override
    {
        Node* result = new Parameter(param.name);
        return result;
    }
};
