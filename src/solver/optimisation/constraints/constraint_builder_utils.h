#pragma once
#include "ConstraintBuilder.h"
#include <memory>

ConstraintBuilderData GetConstraintBuilderDataFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre);

inline ConstraintBuilderData GetConstraintBuilderDataFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre)
{
    /* not good!!!!!!!!!!*/
    auto& problemAResoudreRef = *ProblemeAResoudre.get();
    return GetConstraintBuilderDataFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                       problemAResoudreRef);
}

inline ConstraintBuilderData GetConstraintBuilderDataFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    return GetConstraintBuilderDataFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                       ProblemeAResoudre);
}
