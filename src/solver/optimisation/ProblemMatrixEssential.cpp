// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ProblemMatrixEssential.h"

ProblemMatrixEssential::ProblemMatrixEssential(PROBLEME_HEBDO* problemeHebdo):
    problemeHebdo_(problemeHebdo)
{
}

void ProblemMatrixEssential::Run()
{
    for (auto& group: constraintgroups_)
    {
        group->BuildConstraints();
    }
}

void ProblemMatrixEssential::InitializeProblemAResoudreCounters()
{
    auto& ProblemeAResoudre = problemeHebdo_->ProblemeAResoudre;
    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
}
