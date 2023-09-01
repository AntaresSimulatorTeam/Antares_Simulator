#include "constraint_builder.h"

struct HydroPowerSmoothingUsingVariationMaxUp : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int pdt);
};