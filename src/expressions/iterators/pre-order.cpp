#include <ranges>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Expressions::Nodes
{
namespace
{
// Children, left to right
std::vector<Node*> childrenLeftToRight(Node* node)
{
    if (auto* bin = dynamic_cast<BinaryNode*>(node))
    {
        return {bin->left(), bin->right()};
    }
    else if (auto* unary = dynamic_cast<UnaryNode*>(node))
    {
        return {unary->child()};
    }
    else if (auto* sum = dynamic_cast<SumNode*>(node))
    {
        return sum->getOperands();
    }
    return {};
}

// const version
std::vector<const Node*> childrenLeftToRight(const Node* node)
{
    if (auto* bin = dynamic_cast<const BinaryNode*>(node))
    {
        return {bin->left(), bin->right()};
    }
    else if (auto* unary = dynamic_cast<const UnaryNode*>(node))
    {
        return {unary->child()};
    }
    else if (auto* sum = dynamic_cast<const SumNode*>(node))
    {
        return sum->getConstOperands();
    }
    return {};
}
} // namespace

// Constructor
ASTPreOrderIterator::ASTPreOrderIterator(Node* root)
{
    if (root)
    {
        nodeStack.push(root);
    }
}

// Dereference operator
ASTPreOrderIterator::reference ASTPreOrderIterator::operator*() const
{
    return *nodeStack.top();
}

// Pointer access operator
ASTPreOrderIterator::pointer ASTPreOrderIterator::operator->() const
{
    return nodeStack.top();
}

// Increment operator (pre-order traversal)
ASTPreOrderIterator& ASTPreOrderIterator::operator++()
{
    if (nodeStack.empty())
    {
        return *this;
    }

    Node* current = nodeStack.top();
    nodeStack.pop();

    const auto children = childrenLeftToRight(current);
    // Push children in reverse order to process them in left-to-right order
    for (auto* it: children | std::views::reverse)
    {
        nodeStack.push(it);
    }

    return *this;
}

// Equality comparison
bool ASTPreOrderIterator::operator==(const ASTPreOrderIterator& other) const
{
    if (nodeStack.empty() && other.nodeStack.empty())
    {
        return true;
    }
    if (nodeStack.empty() || other.nodeStack.empty())
    {
        return false;
    }
    return nodeStack.top() == other.nodeStack.top();
}

// Inequality comparison
bool ASTPreOrderIterator::operator!=(const ASTPreOrderIterator& other) const
{
    return !(*this == other);
}

AST::AST(Node* rootNode):
    root(rootNode)
{
}

// Begin iterator
ASTPreOrderIterator AST::begin()
{
    return ASTPreOrderIterator(root);
}

// End iterator (indicating traversal is complete)
ASTPreOrderIterator AST::end()
{
    return ASTPreOrderIterator(nullptr);
}

//
// CONST VERSION
//

// Constructor
ASTPreOrderIteratorConst::ASTPreOrderIteratorConst(const Node* root)
{
    if (root)
    {
        nodeStack.push(root);
    }
}

// Dereference operator
ASTPreOrderIteratorConst::reference ASTPreOrderIteratorConst::operator*() const
{
    return *nodeStack.top();
}

// Pointer access operator
ASTPreOrderIteratorConst::pointer ASTPreOrderIteratorConst::operator->() const
{
    return nodeStack.top();
}

// Increment operator (pre-order traversal)
ASTPreOrderIteratorConst& ASTPreOrderIteratorConst::operator++()
{
    if (nodeStack.empty())
    {
        return *this;
    }

    const Node* current = nodeStack.top();
    nodeStack.pop();

    const auto children = childrenLeftToRight(current);
    // Push children in reverse order to process them in left-to-right order
    for (auto* it: children | std::views::reverse)
    {
        nodeStack.push(it);
    }

    return *this;
}

// Equality comparison
bool ASTPreOrderIteratorConst::operator==(const ASTPreOrderIteratorConst& other) const
{
    if (nodeStack.empty() && other.nodeStack.empty())
    {
        return true;
    }
    if (nodeStack.empty() || other.nodeStack.empty())
    {
        return false;
    }
    return nodeStack.top() == other.nodeStack.top();
}

// Inequality comparison
bool ASTPreOrderIteratorConst::operator!=(const ASTPreOrderIteratorConst& other) const
{
    return !(*this == other);
}

ASTconst::ASTconst(const Node* rootNode):
    root(rootNode)
{
}

// Begin iterator
ASTPreOrderIteratorConst ASTconst::begin()
{
    return ASTPreOrderIteratorConst(root);
}

// End iterator (indicating traversal is complete)
ASTPreOrderIteratorConst ASTconst::end()
{
    return ASTPreOrderIteratorConst(nullptr);
}
} // namespace Antares::Expressions::Nodes
