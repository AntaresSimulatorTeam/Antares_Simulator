/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <math.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include <antares/emergency.h>
#include <antares/logs.h>

#include "../utils/ortools_utils.h"

extern "C"
{
#include "spx_fonctions.h"
}

using namespace Antares;

void OPT_LiberationProblemesSimplexe(const PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsPourUneOptimisation;
    if (problemeHebdo->OptimisationAuPasHebdomadaire == NON_ANTARES)
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsDUneJournee;
    else
        NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;

    int nbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    const PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    if (!ProblemeAResoudre)
        return;

    const auto& study = *Data::Study::Current::Get();
    bool ortoolsUsed = study.parameters.ortoolsUsed;

    if (problemeHebdo->LeProblemeADejaEteInstancie == NON_ANTARES)
    {
        for (int numIntervalle = 0; numIntervalle < nbIntervalles; numIntervalle++)
        {
            auto ProbSpx
              = (PROBLEME_SPX*)(ProblemeAResoudre->ProblemesSpx->ProblemeSpx[numIntervalle]);
            auto solver = (MPSolver*)(ProblemeAResoudre->ProblemesSpx->ProblemeSpx[numIntervalle]);

            if (ortoolsUsed && solver != NULL)
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
