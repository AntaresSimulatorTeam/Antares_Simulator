/*
** Copyright 2007-2018 RTE
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

bool OPT_PilotageOptimisationLineaire(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    char CalculerLesPmin;
    char CalculerLesPmax;
    char FaireDerniereOptimisation;

    int Pays;

    if (ProblemeHebdo->LeProblemeADejaEteInstancie == NON_ANTARES)
    {
        if (ProblemeHebdo->TypeDOptimisation == OPTIMISATION_LINEAIRE)
        {
            ProblemeHebdo->NombreDeZonesDeReserveJMoins1 = ProblemeHebdo->NombreDePays;
            for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
            {
                ProblemeHebdo->NumeroDeZoneDeReserveJMoins1[Pays] = Pays;
                ProblemeHebdo->CoutDeDefaillanceEnReserve[Pays] = 1.e+6;
            }

            ProblemeHebdo->ContrainteDeReserveJMoins1ParZone = NON_ANTARES;
            ProblemeHebdo->NombreDePasDeTempsRef = ProblemeHebdo->NombreDePasDeTemps;
            ProblemeHebdo->NombreDePasDeTempsDUneJourneeRef
              = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
            ProblemeHebdo->NombreDeJours = (int)(ProblemeHebdo->NombreDePasDeTemps
                                                 / ProblemeHebdo->NombreDePasDeTempsDUneJournee);

            if (ProblemeHebdo->OptimisationAuPasHebdomadaire == NON_ANTARES)
            {
                ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation
                  = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
            }
            else
            {
                ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation
                  = ProblemeHebdo->NombreDePasDeTemps;
            }

            OPT_AllocDuProblemeAOptimiser(ProblemeHebdo);

            OPT_ChainagesDesIntercoPartantDUnNoeud(ProblemeHebdo);
        }

        ProblemeHebdo->LeProblemeADejaEteInstancie = OUI_ANTARES;
    }

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    OPT_VerifierPresenceReserveJmoins1(ProblemeHebdo);

    OPT_SauvegarderLesPmaxThermiques(ProblemeHebdo);

    OPT_InitialiserLesPminHebdo(ProblemeHebdo);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(ProblemeHebdo);

    OPT_MaxDesPmaxHydrauliques(ProblemeHebdo);

    if (ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(ProblemeHebdo);
    }

    if (!OPT_OptimisationLineaire(
          ProblemeHebdo, numSpace))
        return false;

    return true;
}
