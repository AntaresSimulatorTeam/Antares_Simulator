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
    logs.debug() << "[CSR] RHS: ";
    //CSR todo initialize RHS right hand side of constraints for hourly CSR quadratic problem.
    int Cnt;
    int Area;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    // //constraint for each area inside adq patch: 2 * ENS > 10
    // for (Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    // {
    //     if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
    //     {
    //         // Cnt = hourlyCsrProblem.numberOfConstraintCsr.find(Area)->second;
    //         std::map<int, int>::iterator it = hourlyCsrProblem.numberOfConstraintCsr.find(Area);
    //         if(it != hourlyCsrProblem.numberOfConstraintCsr.end())
    //             Cnt = it->second;
    //         ProblemeAResoudre->SecondMembre[Cnt] = 10;
    //         logs.debug() << Cnt << ": dummy 2*ENS > 10: RHS[" << Cnt << "] = " << ProblemeAResoudre->SecondMembre[Cnt];
    //     }
    // }

    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    COUTS_DE_TRANSPORT* TransportCost;
    
    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of type 2.
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
        && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {

            TransportCost = ProblemeHebdo->CoutDeTransport[Interco];
            // if (TransportCost->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                std::map<int, int>::iterator it = hourlyCsrProblem.numberOfConstraintCsrFlowDissociation.find(Interco);
                if(it != hourlyCsrProblem.numberOfConstraintCsrFlowDissociation.end())
                    Cnt = it->second;
                // if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                //     ProblemeAResoudre->SecondMembre[Cnt] = ProblemeHebdo->ValeursDeNTC[hour]
                //                           ->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                // else
                    ProblemeAResoudre->SecondMembre[Cnt] = 0.;  
                    logs.debug() << Cnt << "Flow=D-I: RHS[" << Cnt << "] = " << ProblemeAResoudre->SecondMembre[Cnt]; 
            }
        }
    }

    // constraint: 
    // ENS(node A) + 
    // [ Sum flow_direct(node 2 upstream -> node A) + Sum flow_indirect(node A <- node 2 downstream) – 
    // Sum flow_indirect(node 2 upstream <- node A) – Sum flow_direct(node A -> node 2 downstream) ] – 
    // spillage(node A) = 
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch
    double rhsAreaBalance;
    for (Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area]
            == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it
              = hourlyCsrProblem.numberOfConstraintCsrAreaBalance.find(Area);
            if (it != hourlyCsrProblem.numberOfConstraintCsrAreaBalance.end())
                Cnt = it->second;
            std::map<int, double>::iterator itt = hourlyCsrProblem.rhsAreaBalanceValues.find(Area);
            if (itt != hourlyCsrProblem.rhsAreaBalanceValues.end())
                rhsAreaBalance = itt->second;

            ProblemeAResoudre->SecondMembre[Cnt] = rhsAreaBalance;
            logs.debug() << Cnt << ": Area Balance: RHS[" << Cnt << "] = " << ProblemeAResoudre->SecondMembre[Cnt] << " (Area = " << Area <<")";
        }
    }

    // CSR todo. Add, only hourly, user defined Binding constraints between transmission flows
    // and/or power generated from generating units.
    int CntCouplante;
    int Interco;
    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;
    double* SecondMembre = ProblemeAResoudre->SecondMembre;
    std::map<int, int> bingdingConstraintNumber = hourlyCsrProblem.numberOfConstraintCsrHourlyBinding;
    
    int NbInterco;
    double Poids;
    double ValueOfFlow; 
    int Index;

    for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
            CntCouplante++)
    {
        if(bingdingConstraintNumber.find(CntCouplante) != bingdingConstraintNumber.end()) //ok
        // if(hourlyCsrProblem.bindingConstraintContainsIntercoInsideAdqPatch[CntCouplante] == true)
        {

            MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

            Cnt = bingdingConstraintNumber[CntCouplante];

            //1. this is the original RHS of bingding constraint
            SecondMembre[Cnt] = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[hour];
            logs.debug() << Cnt << ": Hourly bc: Existing-RHS[" << Cnt
                         << "] = " << SecondMembre[Cnt] << " (CntCouplante = " << CntCouplante
                         << ")";
            // SecondMembre[Cnt] = MatriceDesContraintesCouplantes
            //                         ->SecondMembreDeLaContrainteCouplante[PdtHebdo];
            // AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL; // ok. costs are not of interest to us here!

            //2. CSR todo: RHS part 2: flow other than 2<->2
            NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            for (Index = 0; Index < NbInterco; Index++)
            {
                Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];

                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco] != Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
                    || ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco] != Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    //we have an interco other than type 2-2
                    // Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour]->NumeroDeVariableDeLInterconnexion[Interco];
                    ValueOfFlow = ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]; //todo check do we care about the export/import
                    SecondMembre[Cnt] -= ValueOfFlow * Poids;
                    logs.debug()
                      << Cnt << ": Hourly bc: IntercoFlow-RHS[" << Cnt
                      << "] = " << SecondMembre[Cnt] << " (CntCouplante = " << CntCouplante << ")"
                      << ". Interco;" + std::to_string(Interco) << ". Between:["
                      << ProblemeHebdo
                           ->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]]
                      << "]-["
                      << ProblemeHebdo
                           ->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]]
                      << "]" << ". ValueOfFlow: " <<ValueOfFlow << ". Poids: " << Poids;
                }
            }

            //3. CSR todo: RHS part 3: - cluster
            int NbClusters = MatriceDesContraintesCouplantes
                            ->NombreDePaliersDispatchDansLaContrainteCouplante;
            int Area;
            PALIERS_THERMIQUES* PaliersThermiquesDuPays;
            int Palier;
            int IndexNumeroDuPalierDispatch;
            double ValueOfVar; // to be consistent code as above
            
            for (Index = 0; Index < NbClusters; Index++)
            {
                Area = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Area];

                IndexNumeroDuPalierDispatch = MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index];

                Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                        [IndexNumeroDuPalierDispatch];
                Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];

                //CSR todo, value of Var "Cluster Output" check if this is correct?
                // Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDuPalierThermique[Palier];
                ValueOfVar = ProblemeHebdo->ResultatsHoraires[Area]
                                        ->ProductionThermique[hour]
                                        ->ProductionThermiqueDuPalier[IndexNumeroDuPalierDispatch];

                SecondMembre[Cnt] -= ValueOfVar * Poids;
                logs.debug() << Cnt << ": Hourly bc: ThermalCluster-RHS[" << Cnt
                             << "] = " << SecondMembre[Cnt] << " (CntCouplante = " << CntCouplante
                             << ")"
                             << ". Area:" << Area
                             << ", Palier:" << Palier << ", Poids" << Poids
                             << ", ValueOfVar:" << ValueOfVar;
            }
        }
    }    
}
