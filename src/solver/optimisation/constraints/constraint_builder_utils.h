#pragma once
#include "ConstraintBuilder.h"
#include <memory>

ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre);

inline ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre)
{
    /* not good!!!!!!!!!!*/
    auto& problemAResoudreRef = *ProblemeAResoudre.get();
    return NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                      problemAResoudreRef);
}

inline ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    auto data = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                           ProblemeAResoudre);
    data.NbTermesContraintesPourLesCoutsDeDemarrage
      = &problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage;
    return data;
}
