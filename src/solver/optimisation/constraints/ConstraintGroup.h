#pragma once

#include "sim_structure_probleme_economique.h"
#include "../opt_structure_probleme_a_resoudre.h"
#include "constraint_builder_utils.h"

class ConstraintGroup
{
public:
    explicit ConstraintGroup(PROBLEME_HEBDO* problemeHebdo) :
     problemeHebdo_(problemeHebdo),
     builder_(NewGetConstraintBuilderFromProblemHebdo(problemeHebdo_))
    {
    }

    
    virtual void Build() = 0;
    PROBLEME_HEBDO* problemeHebdo_;
    std::shared_ptr<ConstraintBuilder> builder_;
};