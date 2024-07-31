#include <any>
#include <iostream>

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/PrintVisitor.h>
#include <antares/solver/expressions/StringVisitor.h>

int main()

{
    auto* p1 = new Parameter("hello");
    auto* p2 = new Parameter("world");
    auto* neg = new Negate(p2);
    Add root(p1, neg);

    {
        PrintVisitor printVisitor;
        root.accept(printVisitor);
    }
    std::cout << std::endl;
    {
        StringVisitor stringVisitor;
        std::cout << std::any_cast<std::string>(root.accept(stringVisitor));
    }
    std::cout << std::endl;
    {
        CloneVisitor cloneVisitor;
        std::any clone = root.accept(cloneVisitor);
        PrintVisitor printVisitor;
        std::unique_ptr<Node> root2(std::any_cast<Node*>(clone));
        root2->accept(printVisitor);
    }
}
