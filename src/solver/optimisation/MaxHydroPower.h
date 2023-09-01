#include "constraint_builder.h"

struct MaxHydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};