#pragma once

#include <stack>

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

    // Copy constructor
    ASTPreOrderIterator(const ASTPreOrderIterator&) = default;

    // Move constructor
    ASTPreOrderIterator(ASTPreOrderIterator&&) noexcept = default;

    // Copy assignment
    ASTPreOrderIterator& operator=(const ASTPreOrderIterator&) = default;

    // Move assignment
    ASTPreOrderIterator& operator=(ASTPreOrderIterator&&) noexcept = default;

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

// PreOrder Iterator for AST const version
class ASTPreOrderIteratorConst
{
    std::stack<const Node*> nodeStack;

public:
    // Iterator type aliases
    using iterator_category = std::forward_iterator_tag;
    using value_type = Node;
    using difference_type = std::ptrdiff_t;
    using pointer = const Node*;
    using reference = const Node&;

    // Constructor
    explicit ASTPreOrderIteratorConst(const Node* root = nullptr);

    // Copy constructor
    ASTPreOrderIteratorConst(const ASTPreOrderIteratorConst&) = default;

    // Move constructor
    ASTPreOrderIteratorConst(ASTPreOrderIteratorConst&&) noexcept = default;

    // Copy assignment
    ASTPreOrderIteratorConst& operator=(const ASTPreOrderIteratorConst&) = default;

    // Move assignment
    ASTPreOrderIteratorConst& operator=(ASTPreOrderIteratorConst&&) noexcept = default;

    // Dereference operator
    reference operator*() const;

    // Pointer access operator
    pointer operator->() const;

    // Increment operator (pre-order traversal)
    ASTPreOrderIteratorConst& operator++();

    // Equality comparison
    bool operator==(const ASTPreOrderIteratorConst& other) const;

    // Inequality comparison
    bool operator!=(const ASTPreOrderIteratorConst& other) const;
};

// AST container class to expose begin/end iterators
class ASTconst
{
    const Node* root;

public:
    explicit ASTconst(const Node* rootNode);

    // Begin iterator
    ASTPreOrderIteratorConst begin();

    // End iterator (indicating traversal is complete)
    ASTPreOrderIteratorConst end();
};
} // namespace Antares::Expressions::Nodes
