#pragma once
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
