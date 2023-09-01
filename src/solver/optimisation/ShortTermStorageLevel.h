#include "constraint_builder.h"
struct ShortTermStorageLevel : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pays);
};
