#pragma once

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/Visitor.h>

class StringVisitor: public Visitor
{
    std::any visit(const Add& add) override
    {
        std::string result;
        result += std::any_cast<std::string>(add.n1->accept(*this));
        result += "+";
        result += std::any_cast<std::string>(add.n2->accept(*this));
        return result;
    }

    std::any visit(const Negate& neg) override
    {
        std::string result;
        result += "-(";
        result += std::any_cast<std::string>(neg.n->accept(*this));
        result += ")";
        return result;
    }

    std::any visit(const Parameter& param) override
    {
        return param.name;
    }
};
