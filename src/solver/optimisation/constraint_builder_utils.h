#pragma once
#include "constraint_builder.h"
ConstraintBuilder GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre);

inline ConstraintBuilder GetConstraintBuilderFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();
    return GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                   ProblemeAResoudre);
}
