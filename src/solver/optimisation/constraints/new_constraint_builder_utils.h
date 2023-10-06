#pragma once
#include "new_constraint_builder.h"
#include <memory>

std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre);

inline std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    return NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                      ProblemeAResoudre);
}
