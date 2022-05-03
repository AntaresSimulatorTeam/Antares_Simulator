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

using namespace Antares::Data;

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

    // // constraint: 2 * ENS > 10
    // for (Area = 0; Area < ProblemeHebdo->NombreDePays; ++Area)
    // {
    //     if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
    //     {
    //         NombreDeTermes = 0;
    //         Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Area];
    //         Pi[NombreDeTermes] = 2.0;
    //         Colonne[NombreDeTermes] = Var;
    //         NombreDeTermes++;

    //         hourlyCsrProblem.numberOfConstraintCsr[Area]= ProblemeAResoudre->NombreDeContraintes;
    //         NomDeLaContrainte = "dummy 2*ENS > 10. area:" + std::to_string(Area) + "; " + ProblemeHebdo->NomsDesPays[Area];
    //         logs.debug() << "C: " << ProblemeAResoudre->NombreDeContraintes << ": " << NomDeLaContrainte ;
    //         OPT_ChargerLaContrainteDansLaMatriceDesContraintes(ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>', NomDeLaContrainte);

    //     }
    // }

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
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) = 
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
                            ->NumeroDeVariableDeLInterconnexion[Interco]; //flow (A->2)
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                        logs.debug() << "S-Interco number: [" << std::to_string(Interco)
                                    << "] between: [" << ProblemeHebdo->NomsDesPays[Area] << "]-["
                                    << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]] << "]";
                    }
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            // or + import flows
            Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
                    == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDeLInterconnexion[Interco]; // flow (2 -> A)
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                        logs.debug() << "E-Interco number: [" << std::to_string(Interco)
                                    << "] between: [" << ProblemeHebdo->NomsDesPays[Area] << "]-["
                                    << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]] << "]";
                    }
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

    int NbInterco;
    double Poids; // Weight
    // int Offset;
    // int hour1;
    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;

    // Study::Ptr study = Study::Current::Get();
    // uint numSpace = 0; // todo: not in problem hebdo, we need to pass it as function argument if we whant to use timeStepInYear.
    // int timeStepInYear = study->runtime->weekInTheYear[numSpace] * 168 + hour;

    // Special case of the binding constraints
    for (int CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        MatriceDesContraintesCouplantes
          = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

        // hourlyCsrProblem.bindingConstraintContainsIntercoInsideAdqPatch[CntCouplante] = false; //init as false, if there is interco 2-2, then change to true
        // not used.
        if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
        {
            NbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            NombreDeTermes = 0;
            for (int Index = 0; Index < NbInterco; Index++)
            {
                Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
                // Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];

                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
                      == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
                    && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco]
                         == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    // if (Offset >= 0)
                    // {
                    //     hour1
                    //       = (hour + Offset) % ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
                    // }
                    // else
                    // {
                    //     hour1 = (hour + Offset + ProblemeHebdo->NombreDePasDeTemps)
                    //             % ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
                    // }

                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour]
                            ->NumeroDeVariableDeLInterconnexion[Interco];

                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;

                        logs.debug()
                          << "Interco:" + std::to_string(Interco) << ". Between:["
                          << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]]
                          << "]-["
                          << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]]
                          << "], with Poids(coeff):" + std::to_string(Poids) + " inserted to LHS!";
                    }
                }
                // CSR todo: cluster becomes RHS parameters
            }

            if (NombreDeTermes > 0) //current binding constraint contains an interco type 2<->2
            {
                // hourlyCsrProblem.bindingConstraintContainsIntercoInsideAdqPatch[CntCouplante] = true; // not used

                hourlyCsrProblem.numberOfConstraintCsrHourlyBinding[CntCouplante]
                = ProblemeAResoudre->NombreDeContraintes;

                // NomDeLaContrainte = "bc::hourly::" + std::to_string(timeStepInYear + 1) // timeStepInYear not properly defined!
                //                     + "::" 
                //                     + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;
                NomDeLaContrainte = "bc::hourly::" + std::to_string(hour)
                                    + "::" 
                                    + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;// + ". Interco:" + std::to_string(Interco);

                logs.debug() << "C (bc): " << ProblemeAResoudre->NombreDeContraintes << ": "
                            << NomDeLaContrainte;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                ProblemeAResoudre,
                Pi,
                Colonne,
                NombreDeTermes,
                MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
                NomDeLaContrainte);
            }
        }
    }

    MemFree(Pi);
    MemFree(Colonne);
}