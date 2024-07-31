#include <memory>
#include <iostream>
#include <string>
#include <any>

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

class Node {
public:
  virtual ~Node() = default;
  virtual std::any accept(Visitor& visitor) = 0;
};

class Add : public Node
{
public:
  virtual ~Add() = default;
  Add(Node* n1, Node* n2) :
    n1(n1), n2(n2) {}
    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }
  //private:
  std::unique_ptr<Node> n1, n2;
};

class Negate : public Node
{
public:
    virtual ~Negate() = default;
    Negate(Node* n) : n(n) {}
    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }
  //private:
  std::unique_ptr<Node> n;
};

class Parameter : public Node
{
public:
  virtual ~Parameter() = default;
  Parameter(const std::string name) : name(name) {}
    std::any accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }
  //private:
  std::string name;
};

class Print : public Visitor
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

class String : public Visitor
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
}
