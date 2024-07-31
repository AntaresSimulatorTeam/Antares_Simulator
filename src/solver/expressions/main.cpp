#include <any>
#include <iostream>

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Node.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/PrintVisitor.h>
#include <antares/solver/expressions/StringVisitor.h>

template<class V>
std::any visit(std::shared_ptr<Antares::Solver::Expressions::Node> node)
{
    V visitor;
    return node->accept(visitor);
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
    std::cout << std::any_cast<std::string>(visit<StringVisitor>(root));
    std::cout << std::endl;

    std::any clone = visit<CloneVisitor>(root);
    auto root2 = std::any_cast<std::shared_ptr<Node>>(clone);
    visit<PrintVisitor>(root2);
}
