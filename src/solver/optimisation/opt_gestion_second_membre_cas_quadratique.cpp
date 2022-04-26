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
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

void OPT_InitialiserLeSecondMembreDuProblemeQuadratique(PROBLEME_HEBDO* ProblemeHebdo, int PdtHebdo)
{
    int Cnt;
    int Pays;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays - 1; Pays++)
    {
        Cnt = ProblemeHebdo->NumeroDeContrainteDeSoldeDEchange[Pays];
        ProblemeAResoudre->SecondMembre[Cnt]
          = ProblemeHebdo->SoldeMoyenHoraire[PdtHebdo]->SoldeMoyenDuPays[Pays];
    }
}

void OPT_InitialiserLeSecondMembreDuProblemeQuadratique_CSR(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    //CSR todo initialize RHS right hand side of constraints for hourly CSR quadratic problem.
    int Cnt;
    int Area;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    //constraint for each area inside adq patch: 2 * ENS > 1000
    for (Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            Cnt = hourlyCsrProblem.numberOfConstraintCsr[Area];
            ProblemeAResoudre->SecondMembre[Cnt] = 1000;
        }
    }

    // I Kirchhoff's law contstraint for all areas.
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    COUTS_DE_TRANSPORT* TransportCost;
    double test;
    for (Area = 0; Area < ProblemeHebdo->NombreDePays - 1; Area++)
    {
        Cnt = hourlyCsrProblem.numberOfConstraintCsrAreaBalance[Area];
        ProblemeAResoudre->SecondMembre[Cnt]
          = ProblemeHebdo->SoldeMoyenHoraire[hour]->SoldeMoyenDuPays[Area]; // todo what to use: 

        // ProblemeHebdo->SoldeMoyenHoraire[hour]->SoldeMoyenDuPays[Area]; // average hourly balance per area! OR
        // -ProblemeHebdo->ConsommationsAbattues[hour]->ConsommationAbattueDuPays[Area]; // = Reduced consumption per area
    }

    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for all links.
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        TransportCost = ProblemeHebdo->CoutDeTransport[Interco];
        if (TransportCost->IntercoGereeAvecDesCouts == OUI_ANTARES)
        {
            Cnt = hourlyCsrProblem.numberOfConstraintCsrFlowDissociation[Interco];
            if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                ProblemeAResoudre->SecondMembre[Cnt] = ProblemeHebdo->ValeursDeNTC[hour]
                                      ->ValeurDeLoopFlowOrigineVersExtremite[Interco];
            else
                ProblemeAResoudre->SecondMembre[Cnt] = 0.;   
        }
    }

    // CSR todo. Add, only hourly, user defined Binding constraints between transmission flows
    // and/or power generated from generating units.
}