// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "ConstraintBuilder.h"
struct PROBLEME_HEBDO;

ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre);

inline ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    return NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                      *ProblemeAResoudre);
}
