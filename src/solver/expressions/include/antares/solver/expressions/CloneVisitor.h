#pragma once

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/Visitor.h>

class CloneVisitor: public Visitor
{
    std::any visit(const Add& add) override;
    std::any visit(const Negate& neg) override;

    std::any visit(const Parameter& param) override;
};
