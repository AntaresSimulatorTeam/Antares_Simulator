// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/simplex/SimplexResult.h"

#include <cassert>

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

namespace Antares::Solver::Optimization::Simplex
{

void applyResults(PROBLEME_HEBDO& problemeHebdo,
                  const SimplexResult& result,
                  int NumIntervalle,
                  int optimizationNumber)
{
    const auto& ProblemeAResoudre = problemeHebdo.ProblemeAResoudre;
    double optimizationCost = result.objectiveValue;

    for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
    {
        double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
        if (pt != nullptr)
        {
            *pt = ProblemeAResoudre->X[static_cast<std::size_t>(i)];
        }

        pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[i];
        if (pt != nullptr)
        {
            *pt = ProblemeAResoudre->CoutsReduits[static_cast<std::size_t>(i)];
        }
    }
    {
        const int opt = optimizationNumber - 1;
        assert(opt >= 0 && opt < 2);
        problemeHebdo.timeMeasure[opt] = result.timeMeasure;
    }

    if (optimizationNumber == PREMIERE_OPTIMISATION)
    {
        problemeHebdo.coutOptimalSolution1[static_cast<unsigned int>(NumIntervalle)]
          = optimizationCost;
    }
    else
    {
        problemeHebdo.coutOptimalSolution2[static_cast<unsigned int>(NumIntervalle)]
          = optimizationCost;
    }
    for (int Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
    {
        double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
        if (pt != nullptr)
        {
            *pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[static_cast<std::size_t>(Cnt)];
        }
    }
}

} // namespace Antares::Solver::Optimization::Simplex
