#include <string>

#include <antares/solver/expressions/ExpressionsNodes.h>
#include <antares/solver/expressions/StringVisitor.h>

std::any StringVisitor::visit(const Add& add)
{
    std::string result;
    result += std::any_cast<std::string>(add.n1->accept(*this));
    result += "+";
    result += std::any_cast<std::string>(add.n2->accept(*this));
    return result;
}

std::any StringVisitor::visit(const Negate& neg)
{
    std::string result;
    result += "-(";
    result += std::any_cast<std::string>(neg.n->accept(*this));
    result += ")";
    return result;
}

std::any StringVisitor::visit(const Parameter& param)
{
    return param.name;
}
