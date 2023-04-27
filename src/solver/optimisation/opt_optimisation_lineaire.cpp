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

#include "../simulation/simulation.h"

#include "opt_fonctions.h"

#include <antares/logs.h>
#include <antares/emergency.h>
#include "../utils/filename.h"

using namespace Antares;
using namespace Yuni;

double OPT_ObjectiveFunctionResult(const PROBLEME_HEBDO* Probleme,
                                   const int NumeroDeLIntervalle,
                                   const int optimizationNumber)
{
    if (optimizationNumber == PREMIERE_OPTIMISATION)
        return Probleme->coutOptimalSolution1[NumeroDeLIntervalle];
    else
        return Probleme->coutOptimalSolution2[NumeroDeLIntervalle];
}

bool OPT_OptimisationLineaire(PROBLEME_HEBDO* problemeHebdo, AdqPatchParams& adqPatchParams, uint numSpace)
{
    int optimizationNumber = PREMIERE_OPTIMISATION;

    problemeHebdo->NombreDePasDeTemps = problemeHebdo->NombreDePasDeTempsRef;
    problemeHebdo->NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJourneeRef;

    if (!problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;
    }

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    OPT_NumeroDeJourDuPasDeTemps(problemeHebdo);

    OPT_NumeroDIntervalleOptimiseDuPasDeTemps(problemeHebdo);

    OPT_RestaurerLesDonnees(problemeHebdo, optimizationNumber);

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(problemeHebdo);

    OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(problemeHebdo);

OptimisationHebdo:
    int DernierPdtDeLIntervalle;
    for (int pdtHebdo = 0, numeroDeLIntervalle = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps;
         pdtHebdo = DernierPdtDeLIntervalle, numeroDeLIntervalle++)
    {
        int PremierPdtDeLIntervalle = pdtHebdo;
        DernierPdtDeLIntervalle = pdtHebdo + NombreDePasDeTempsPourUneOptimisation;

        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(problemeHebdo, 
                                                               adqPatchParams,
                                                               numeroDeLIntervalle,
                                                               PremierPdtDeLIntervalle, 
                                                               DernierPdtDeLIntervalle, 
                                                               optimizationNumber);

        OPT_InitialiserLeSecondMembreDuProblemeLineaire(problemeHebdo,
                                                        PremierPdtDeLIntervalle,
                                                        DernierPdtDeLIntervalle,
                                                        numeroDeLIntervalle,
                                                        optimizationNumber);

        OPT_InitialiserLesCoutsLineaire(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle, numSpace);

        // An optimization period represents a sequence as <year>-<week> or <year>-<week>-<day>,
        // depending whether the optimization is daily or weekly.
        // These sequences are used when building the names of MPS or criterion files.
        auto optPeriodStringGenerator
          = createOptPeriodAsString(problemeHebdo->OptimisationAuPasHebdomadaire,
                                    numeroDeLIntervalle,
                                    problemeHebdo->weekInTheYear,
                                    problemeHebdo->year);

        if (!OPT_AppelDuSimplexe(
              problemeHebdo, numeroDeLIntervalle, optimizationNumber, optPeriodStringGenerator))
            return false;

        if (problemeHebdo->ExportMPS != Data::mpsExportStatus::NO_EXPORT
            || problemeHebdo->Expansion)
        {
            double optimalSolutionCost
              = OPT_ObjectiveFunctionResult(problemeHebdo, numeroDeLIntervalle, optimizationNumber);
            OPT_EcrireResultatFonctionObjectiveAuFormatTXT(
              optimalSolutionCost, optPeriodStringGenerator, optimizationNumber);
        }
    }

    if (optimizationNumber == PREMIERE_OPTIMISATION)
    {
        if (!problemeHebdo->OptimisationAvecCoutsDeDemarrage)
        {
            OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(problemeHebdo);
        }
        else if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
        {
            OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(problemeHebdo);
        }
        else
            printf("BUG: l'indicateur problemeHebdo->OptimisationAvecCoutsDeDemarrage doit etre "
                   "initialise a true ou false\n");

        optimizationNumber = DEUXIEME_OPTIMISATION;

        if (!problemeHebdo->Expansion)
            goto OptimisationHebdo;
    }

    return true;
}
