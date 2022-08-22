/*
** Copyright 2007-2022 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_donnees.h"
#include "../solver/simulation/sim_extern_variables_globales.h"
#include "../solver/optimisation/opt_fonctions.h"

using namespace Antares::Data;

void setConstraintsOnFlows(PROBLEME_HEBDO* ProblemeHebdo,
                           HOURLY_CSR_PROBLEM& hourlyCsrProblem,
                           double* Pi,
                           int* Colonne)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Var;
    int NombreDeTermes;
    int Interco;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    std::string NomDeLaContrainte;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
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
            logs.debug() << "C Interco: " << ProblemeAResoudre->NombreDeContraintes << ": "
                         << NomDeLaContrainte;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
    }
}

void setNodeBalanceConstraints(PROBLEME_HEBDO* ProblemeHebdo,
                               HOURLY_CSR_PROBLEM& hourlyCsrProblem,
                               double* Pi,
                               int* Colonne)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Area;
    int Var;
    int NombreDeTermes;
    int Interco;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    std::string NomDeLaContrainte;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // constraint:
    // ENS(node A) +
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) =
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    for (Area = 0; Area < ProblemeHebdo->NombreDePays; ++Area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
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
                if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                    == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDeLInterconnexion[Interco]; // flow (A->2)
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                        logs.debug()
                          << "S-Interco number: [" << std::to_string(Interco) << "] between: ["
                          << ProblemeHebdo->NomsDesPays[Area] << "]-["
                          << ProblemeHebdo
                               ->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]]
                          << "]";
                    }
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            // or + import flows
            Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
                    == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDeLInterconnexion[Interco]; // flow (2 -> A)
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                        logs.debug()
                          << "E-Interco number: [" << std::to_string(Interco) << "] between: ["
                          << ProblemeHebdo->NomsDesPays[Area] << "]-["
                          << ProblemeHebdo
                               ->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]]
                          << "]";
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

            hourlyCsrProblem.numberOfConstraintCsrAreaBalance[Area]
              = ProblemeAResoudre->NombreDeContraintes;

            NomDeLaContrainte = "Area Balance, Area:" + std::to_string(Area) + "; "
                                + ProblemeHebdo->NomsDesPays[Area];
            ;
            logs.debug() << "C: " << ProblemeAResoudre->NombreDeContraintes << ": "
                         << NomDeLaContrainte;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
    }
}

void setBindingConstraints(PROBLEME_HEBDO* ProblemeHebdo,
                           HOURLY_CSR_PROBLEM& hourlyCsrProblem,
                           double* Pi,
                           int* Colonne)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Var;
    int NombreDeTermes;
    int Interco;
    int NbInterco;
    double Poids;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    std::string NomDeLaContrainte;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // Special case of the binding constraints
    for (int CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        MatriceDesContraintesCouplantes
          = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

        if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
        {
            NbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            NombreDeTermes = 0;
            for (int Index = 0; Index < NbInterco; Index++)
            {
                Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];

                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
                      == Data::AdequacyPatch::physicalAreaInsideAdqPatch
                    && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                         == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                {
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour]
                            ->NumeroDeVariableDeLInterconnexion[Interco];

                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }
            }

            if (NombreDeTermes > 0) // current binding constraint contains an interco type 2<->2
            {
                hourlyCsrProblem.numberOfConstraintCsrHourlyBinding[CntCouplante]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "bc::hourly::" + std::to_string(hour) + "::"
                                    + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;

                logs.debug() << "C (bc): " << ProblemeAResoudre->NombreDeContraintes << ": "
                             << NomDeLaContrainte;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre,
                  Pi,
                  Colonne,
                  NombreDeTermes,
                  MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante);
            }
        }
    }
}

void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique_CSR(
  PROBLEME_HEBDO* ProblemeHebdo,
  HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] constraint list:";
    double* Pi;
    int* Colonne;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    Pi = (double*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(double));
    Colonne = (int*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int));

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;

    setConstraintsOnFlows(ProblemeHebdo, hourlyCsrProblem, Pi, Colonne);
    setNodeBalanceConstraints(ProblemeHebdo, hourlyCsrProblem, Pi, Colonne);
    setBindingConstraints(ProblemeHebdo, hourlyCsrProblem, Pi, Colonne);

    MemFree(Pi);
    MemFree(Colonne);
}