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
std::any visit(Antares::Solver::Expressions::Node& node)
{
    V visitor;
    return node.accept(visitor);
}

int main()
{
    using namespace Antares::Solver::Expressions;
    auto* p1 = new Parameter("hello");
    auto* p2 = new Parameter("world");
    auto* neg = new Negate(p2);
    Add root(p1, neg);

    visit<PrintVisitor>(root);

    std::cout << std::endl;
    std::cout << std::any_cast<std::string>(visit<StringVisitor>(root));
    std::cout << std::endl;

    std::any clone = visit<CloneVisitor>(root);
    std::unique_ptr<Node> root2(std::any_cast<Node*>(clone));
    visit<PrintVisitor>(*root2);
}
