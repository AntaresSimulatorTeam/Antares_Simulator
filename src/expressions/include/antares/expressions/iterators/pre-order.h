#pragma once

#include <stack>
#include <vector>

namespace Antares::Expressions::Nodes
{
// Forward-declaration is enough

class Node;

// PreOrder Iterator for AST
class ASTPreOrderIterator
{
    std::stack<Node*> nodeStack;

public:
    // Iterator type aliases
    using iterator_category = std::forward_iterator_tag;
    using value_type = Node;
    using difference_type = std::ptrdiff_t;
    using pointer = Node*;
    using reference = Node&;

    // Constructor
    explicit ASTPreOrderIterator(Node* root = nullptr);

    // Dereference operator
    reference operator*() const;

    // Pointer access operator
    pointer operator->() const;

    // Increment operator (pre-order traversal)
    ASTPreOrderIterator& operator++();

    // Equality comparison
    bool operator==(const ASTPreOrderIterator& other) const;

    // Inequality comparison
    bool operator!=(const ASTPreOrderIterator& other) const;
};

// AST container class to expose begin/end iterators
class AST
{
    Node* root;

public:
    explicit AST(Node* rootNode);

    // Begin iterator
    ASTPreOrderIterator begin();

    // End iterator (indicating traversal is complete)
    ASTPreOrderIterator end();
};
} // namespace Antares::Expressions::Nodes
