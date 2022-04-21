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

#include <math.h>
//#include <yuni/core/math.h>

using namespace Yuni;

void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO* ProblemeHebdo, int PdtHebdo)
{
    int Interco;
    int Var;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeResistances;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    memset((char*)ProblemeAResoudre->CoutLineaire,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];
    ValeursDeResistances = ProblemeHebdo->ValeursDeNTC[PdtHebdo];

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            ProblemeAResoudre->CoutQuadratique[Var]
              = ValeursDeResistances->ResistanceApparente[Interco];
    }
}

void OPT_InitialiserLesCoutsQuadratiques_CSR(PROBLEME_HEBDO* ProblemeHebdo,
                                             HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    // CSR todo initialize the cost for variables in objective function of hourly CSR quadratic
    // problem.

    int Interco;
    int Var;
    int hour;
    double pto;
    double coeff;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    COUTS_DE_TRANSPORT* TransportCost;

    hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    memset((char*)ProblemeAResoudre->CoutLineaire,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    // CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]; //CSR
    // todo: ??? this should be 0 not hour

    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            // if (ProblemeHebdo->adqPatch->PriceTakingOrder ==Data::AdequacyPatch::adqPatchPTOIsLoad) 
                //todo !! I cannot find load values per area in ProblemeHebdo!!!!!!!!!!!!!!!!!!
            // else
                pto = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour];
                // todo !!what if pto = 0.0 e.g. dens for one area is zero!?

            coeff = 1 / Math::Power(pto, 2);
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                ProblemeAResoudre->CoutQuadratique[Var] = coeff;
        }
    }

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
              == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco]
                 == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            TransportCost = ProblemeHebdo->CoutDeTransport[Interco];

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0.0;
            }

            if (TransportCost->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = TransportCost->CoutDeTransportOrigineVersExtremite[hour];
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = TransportCost->CoutDeTransportExtremiteVersOrigine[hour];
                }
            }
        }
    }

    return;
}