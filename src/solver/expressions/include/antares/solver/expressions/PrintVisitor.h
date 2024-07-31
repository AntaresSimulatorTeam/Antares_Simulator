#pragma once

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/Visitor.h>

class PrintVisitor: public Visitor
{
    std::any visit(const Add& add) override
    {
        add.n1->accept(*this);
        std::cout << "+";
        add.n2->accept(*this);
        return {};
    }

    std::any visit(const Negate& neg) override
    {
        std::cout << "-(";
        neg.n->accept(*this);
        std::cout << ")";
        return {};
    }

    std::any visit(const Parameter& param) override
    {
        std::cout << param.name;
        return {};
    }
};
