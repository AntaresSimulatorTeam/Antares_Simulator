#include "constraint_builder.h"

struct HydroPowerSmoothingUsingVariationMaxDown : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int pdt);
};