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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

bool OPT_PilotageOptimisationLineaire(PROBLEME_HEBDO* problemeHebdo, uint numSpace)
{
    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
        if (problemeHebdo->TypeDOptimisation == OPTIMISATION_LINEAIRE)
        {
            for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
            {
                problemeHebdo->CoutDeDefaillanceEnReserve[pays] = 1.e+6;
            }

            problemeHebdo->NombreDePasDeTempsRef = problemeHebdo->NombreDePasDeTemps;
            problemeHebdo->NombreDePasDeTempsDUneJourneeRef
              = problemeHebdo->NombreDePasDeTempsDUneJournee;
            problemeHebdo->NombreDeJours = (int)(problemeHebdo->NombreDePasDeTemps
                                                 / problemeHebdo->NombreDePasDeTempsDUneJournee);

            if (!problemeHebdo->OptimisationAuPasHebdomadaire)
            {
                problemeHebdo->NombreDePasDeTempsPourUneOptimisation
                  = problemeHebdo->NombreDePasDeTempsDUneJournee;
            }
            else
            {
                problemeHebdo->NombreDePasDeTempsPourUneOptimisation
                  = problemeHebdo->NombreDePasDeTemps;
            }

            OPT_AllocDuProblemeAOptimiser(problemeHebdo);

            OPT_ChainagesDesIntercoPartantDUnNoeud(problemeHebdo);
        }

        problemeHebdo->LeProblemeADejaEteInstancie = true;
    }

    OPT_VerifierPresenceReserveJmoins1(problemeHebdo);

    OPT_SauvegarderLesPmaxThermiques(problemeHebdo);

    OPT_InitialiserLesPminHebdo(problemeHebdo);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(problemeHebdo);

    OPT_MaxDesPmaxHydrauliques(problemeHebdo);

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(problemeHebdo);
    }

    return OPT_OptimisationLineaire(problemeHebdo, numSpace);
}
