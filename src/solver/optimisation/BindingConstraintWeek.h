#include "constraint_builder.h"

struct BindingConstraintWeek : public Constraint
{
    using Constraint::Constraint;
    void add(int cntCouplante);
};