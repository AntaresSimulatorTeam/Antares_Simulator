#include "constraintsbuilder.h"

struct FinalStockExpression : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
