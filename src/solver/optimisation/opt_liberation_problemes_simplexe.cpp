/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <spx_fonctions.h>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares::Solver::Optimization;

void OPT_LiberationProblemesSimplexe(const PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsPourUneOptimisation;
    if (!problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
    {
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;
    }

    int nbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    if (!ProblemeAResoudre)
    {
        return;
    }

    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
        for (int numIntervalle = 0; numIntervalle < nbIntervalles; numIntervalle++)
        {
            auto solver = (MPSolver*)(ProblemeAResoudre->ProblemesSpx[numIntervalle]);

            if (solver)
            {
                ORTOOLS_LibererProbleme(solver);
                solver = nullptr;
            }
        }
    }
}
