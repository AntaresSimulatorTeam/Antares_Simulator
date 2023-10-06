#pragma once
#include "new_constraint_builder.h"
#include <memory>

std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre);

inline std::shared_ptr<NewConstraintBuilder> NewGetConstraintBuilderFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    return NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                      ProblemeAResoudre);
}
