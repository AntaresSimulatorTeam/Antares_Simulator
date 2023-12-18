#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "constraint_builder_utils.h"

class ConstraintGroup
{
public:
    explicit ConstraintGroup(PROBLEME_HEBDO* problemeHebdo, ConstraintBuilder& builder) :
     problemeHebdo_(problemeHebdo), builder_(builder)
    {
    }

    virtual ~ConstraintGroup() = default;
    virtual void BuildConstraints() = 0;
    PROBLEME_HEBDO* problemeHebdo_;
    ConstraintBuilder& builder_;
};