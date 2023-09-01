#include "constraintsbuilder.h"

struct AreaHydroLevel : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int pdt);
};