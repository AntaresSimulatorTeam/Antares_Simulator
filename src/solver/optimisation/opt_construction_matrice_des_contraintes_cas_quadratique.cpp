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

void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique(PROBLEME_HEBDO* ProblemeHebdo)
{
    int Interco;
    int Pays;
    int Var;
    int NombreDeTermes;
    double* Pi;
    int* Colonne;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    Pi = (double*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(double));
    Colonne = (int*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int));

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays - 1; Pays++)
    {
        NombreDeTermes = 0;

        Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Pays];
        while (Interco >= 0)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
        }
        Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Pays];
        while (Interco >= 0)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
        }

        ProblemeHebdo->NumeroDeContrainteDeSoldeDEchange[Pays]
          = ProblemeAResoudre->NombreDeContraintes;

        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
    }

    MemFree(Pi);
    MemFree(Colonne);
}


void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique_CSR(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] constraint list:";
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Area;
    int Var;
    int NombreDeTermes;
    double* Pi;
    int* Colonne;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    std::string NomDeLaContrainte;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    Pi = (double*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(double));
    Colonne = (int*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int));

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // constraint: 2 * ENS > 10
    for (Area = 0; Area < ProblemeHebdo->NombreDePays; ++Area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            NombreDeTermes = 0;
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Area];
            Pi[NombreDeTermes] = 2.0;
            Colonne[NombreDeTermes] = Var;
            NombreDeTermes++;

            hourlyCsrProblem.numberOfConstraintCsr[Area]= ProblemeAResoudre->NombreDeContraintes;
            NomDeLaContrainte = "dummy 2*ENS > 10. area:" + std::to_string(Area) + "; " + ProblemeHebdo->NomsDesPays[Area];
            logs.debug() << "C: " << ProblemeAResoudre->NombreDeContraintes << ": " << NomDeLaContrainte ;
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>', NomDeLaContrainte);

        }
    }

    int Interco;
    COUTS_DE_TRANSPORT* TransportCost;

    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of type 2.
    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {

        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
        && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {

            TransportCost = ProblemeHebdo->CoutDeTransport[Interco];
            // if (TransportCost->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                NombreDeTermes = 0;
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }

                hourlyCsrProblem.numberOfConstraintCsrFlowDissociation[Interco]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "flow=d-i, Interco:" + std::to_string(Interco);
                logs.debug() << "C Interco: " << ProblemeAResoudre->NombreDeContraintes << ": " << NomDeLaContrainte ;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=', NomDeLaContrainte);
            }
        }
    }

    // constraint: 
    // ENS(node A) + 
    // [ ∑flow_direct(node 2 upstream -> node A) + ∑flow_indirect(node A <- node 2 downstream) – 
    // ∑flow_indirect(node 2 upstream <- node A) – ∑flow_direct(node A -> node 2 downstream) ] – 
    // spillage(node A) = 
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch
    for (Area = 0; Area < ProblemeHebdo->NombreDePays; ++Area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area]
            == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            // + ENS
            NombreDeTermes = 0;
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Area];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            
            // - export flows
            Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco]
                    == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDeLInterconnexion[Interco];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                    logs.debug() << "S-Interco number: [" << std::to_string(Interco)
                                 << "] between: [" << ProblemeHebdo->NomsDesPays[Area] << "]-["
                                 << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]] << "]";
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            // + import flows
            Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
                    == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDeLInterconnexion[Interco];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                    logs.debug() << "E-Interco number: [" << std::to_string(Interco)
                                 << "] between: [" << ProblemeHebdo->NomsDesPays[Area] << "]-["
                                 << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]] << "]";
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
            }

            // - Spilled Energy
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Area];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }

            hourlyCsrProblem.numberOfConstraintCsrAreaBalance[Area] = ProblemeAResoudre->NombreDeContraintes;

            NomDeLaContrainte = "Area Balance, Area:" + std::to_string(Area) + "; " + ProblemeHebdo->NomsDesPays[Area];;
            logs.debug() << "C: " << ProblemeAResoudre->NombreDeContraintes << ": "
                         << NomDeLaContrainte;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=', NomDeLaContrainte);
        }
    }

    // CSR todo. Add, only hourly, user defined Binding constraints between transmission flows
    // and/or power generated from generating units.

    MemFree(Pi);
    MemFree(Colonne);
}