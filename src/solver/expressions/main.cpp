#include <iostream>

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Node.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/PrintVisitor.h>
#include <antares/solver/expressions/StringVisitor.h>

template<class V>
V& visit(std::shared_ptr<Antares::Solver::Expressions::Node> node)
{
    static V visitor;
    node->accept(visitor);
    return visitor;
}

int main()
{
    using namespace Antares::Solver::Expressions;
    auto p1 = std::make_shared<Parameter>("hello");
    auto p2 = std::make_shared<Parameter>("world");
    auto neg = std::make_shared<Negate>(p2);
    auto root = std::make_shared<Add>(p1, neg);

    visit<PrintVisitor>(root);

    std::cout << std::endl;
    std::cout << visit<StringVisitor>(root).str();
    std::cout << std::endl;

    auto clone = visit<CloneVisitor>(root).ptr();
    visit<PrintVisitor>(clone);
}
