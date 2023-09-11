#pragma once
#include "constraint_builder.h"

class FlowDissociation : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int interco,
                           std::vector<int>& NumeroDeContrainteDeDissociationDeFlux);
};
