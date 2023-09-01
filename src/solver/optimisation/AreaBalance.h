#include "constraint_builder.h"
struct AreaBalance : public Constraint
{
    using Constraint::Constraint;

    void add(int pdt, int pays);
};