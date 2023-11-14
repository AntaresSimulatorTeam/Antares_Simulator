#pragma once

#include "sim_structure_probleme_economique.h"
#include "../opt_structure_probleme_a_resoudre.h"
#include "constraint_builder_utils.h"

class ConstraintGroup
{
public:
    explicit ConstraintGroup(ConstraintBuilder& builder) : builder_(builder)
    {
    }

    
    virtual void Build() = 0;
    ConstraintBuilder& builder_;
};