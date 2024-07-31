#include <any>
#include <iostream>
#include <memory>
#include <string>

class Add;
class Negate;
class Parameter;

class Visitor
{
public:
    virtual std::any visit(const Add&) = 0;
    virtual std::any visit(const Negate&) = 0;
    virtual std::any visit(const Parameter&) = 0;
};

class Node
{
public:
    virtual ~Node() = default;
    virtual std::any accept(Visitor& visitor) = 0;
};

class Add: public Node
{
public:
    virtual ~Add() = default;

    Add(Node* n1, Node* n2):
        n1(n1),
        n2(n2)
    {
    }

    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    // private:
    std::unique_ptr<Node> n1, n2;
};

class Negate: public Node
{
public:
    virtual ~Negate() = default;

    Negate(Node* n):
        n(n)
    {
    }

    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    // private:
    std::unique_ptr<Node> n;
};

class Parameter: public Node
{
public:
    virtual ~Parameter() = default;

    Parameter(const std::string name):
        name(name)
    {
    }

    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    // private:
    std::string name;
};

class Print: public Visitor
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

class String: public Visitor
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

class Clone: public Visitor
{
    std::any visit(const Add& add) override
    {
        auto* n1 = std::any_cast<Node*>(add.n1->accept(*this));
        Node* n2 = std::any_cast<Node*>(add.n2->accept(*this));

        Node* result = new Add(n1, n2);
        return result;
    }

    std::any visit(const Negate& neg) override
    {
        Node* n = std::any_cast<Node*>(neg.n->accept(*this));
        Node* result = new Negate(n);
        return result;
    }

    std::any visit(const Parameter& param) override
    {
        Node* result = new Parameter(param.name);
        return result;
    }
};

int main()

{
    auto* p1 = new Parameter("hello");
    auto* p2 = new Parameter("world");
    auto* neg = new Negate(p2);
    Add root(p1, neg);

    {
        Print printVisitor;
        root.accept(printVisitor);
    }
    std::cout << std::endl;
    {
        String stringVisitor;
        std::cout << std::any_cast<std::string>(root.accept(stringVisitor));
    }
    std::cout << std::endl;
    {
        Clone cloneVisitor;
        std::any clone = root.accept(cloneVisitor);
        Print printVisitor;
        Node* root2 = std::any_cast<Node*>(clone);
        root2->accept(printVisitor);
        delete root2; // only memory leak is at the root, use std::unique_ptr everywhere ?
    }
}
