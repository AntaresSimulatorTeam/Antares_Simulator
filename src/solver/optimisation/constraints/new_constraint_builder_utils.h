#pragma once
#include "ConstraintBuilder.h"
#include <memory>

std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre);

inline std::shared_ptr<NewConstraintBuilder>
  NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
    const PROBLEME_HEBDO* problemeHebdo,
    std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre)
{
    /* not good!!!!!!!!!!*/
    auto& problemAResoudreRef = *ProblemeAResoudre.get();
    return NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                      problemAResoudreRef);
}

inline std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    auto ret = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                          ProblemeAResoudre);
    ret->data->NbTermesContraintesPourLesCoutsDeDemarrage
      = &problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage;
    return ret;
}
