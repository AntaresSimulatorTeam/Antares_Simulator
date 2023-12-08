/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include "../utils/ortools_utils.h"

extern "C"
{
#include "spx_fonctions.h"
}

using namespace Antares;

void OPT_LiberationProblemesSimplexe(const OptimizationOptions& options, const PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsPourUneOptimisation;
    if (!problemeHebdo->OptimisationAuPasHebdomadaire)
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsDUneJournee;
    else
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;

    int nbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    if (!ProblemeAResoudre)
        return;

    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
        for (int numIntervalle = 0; numIntervalle < nbIntervalles; numIntervalle++)
        {
            auto ProbSpx
              = (PROBLEME_SPX*)(ProblemeAResoudre->ProblemesSpx[numIntervalle]);
            auto solver = (MPSolver*)(ProblemeAResoudre->ProblemesSpx[numIntervalle]);

            if (options.useOrtools && solver != NULL)
            {
                ORTOOLS_LibererProbleme(solver);
                solver = NULL;
            }
            else if (ProbSpx != NULL)
            {
                SPX_LibererProbleme(ProbSpx);
                ProbSpx = NULL;
            }
        }
    }

    return;
}
