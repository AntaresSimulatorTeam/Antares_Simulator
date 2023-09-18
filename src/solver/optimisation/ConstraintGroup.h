#pragma once

#include "sim_structure_probleme_economique.h"
#include "opt_structure_probleme_a_resoudre.h"

class ConstraintGroup
{
public:
    explicit ConstraintGroup(PROBLEME_HEBDO* problemeHebdo) : problemeHebdo_(problemeHebdo)
    {
    }

    /*TODO Rename this*/
    virtual void Build() = 0;
    PROBLEME_HEBDO* problemeHebdo_;
};