#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_fonctions.h"

// #TODO namespace
class ConstraintBuilder
{
public:
    ConstraintBuilder(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre) :
     ProblemeAResoudre_(ProblemeAResoudre)
    {
    }
    ConstraintBuilder& AddVariable(int var, double coeff);
    [[nodiscard]] int build(char constraint_operator);

private:
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre_;
    int nombreDeTermes_ = 0;
};