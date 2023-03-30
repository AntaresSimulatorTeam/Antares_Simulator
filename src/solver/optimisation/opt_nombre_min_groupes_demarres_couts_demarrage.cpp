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
#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include <yuni/io/file.h>
#include "opt_fonctions.h"

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"
}

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                           int*,
                                                                           int,
                                                                           int);

void OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->OptimisationAvecCoutsDeDemarrage)
        return;

    int NombreDePasDeTempsProblemeHebdo = problemeHebdo->NombreDePasDeTemps;
    double Eps = 1.e-3;
    double eps_prodTherm = 1.0;
    double eps_nbGroupes = 1.0;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
    {
        const RESULTATS_HORAIRES* ResultatsHoraires = problemeHebdo->ResultatsHoraires[pays];
        PRODUCTION_THERMIQUE_OPTIMALE** ProductionThermique
          = ResultatsHoraires->ProductionThermique;

        const PALIERS_THERMIQUES* PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        PDISP_ET_COUTS_HORAIRES_PAR_PALIER** PuissanceDisponibleEtCout
          = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            if (problemeHebdo->Expansion)
            {
                int* NombreMinDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->NombreMinDeGroupesEnMarcheDuPalierThermique;
                double* PuissanceDisponibleDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->PuissanceDisponibleDuPalierThermique;
                double* PuissanceMinDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->PuissanceMinDuPalierThermique;

                for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
                {
                    double ProductionThermiqueDuPalier
                      = ProductionThermique[pdtHebdo]->ProductionThermiqueDuPalier[index];

                    if (ProductionThermiqueDuPalier - eps_prodTherm
                        > PuissanceMinDuPalierThermique[pdtHebdo])
                        PuissanceMinDuPalierThermique[pdtHebdo]
                          = ProductionThermiqueDuPalier - eps_prodTherm;

                    if (ProductionThermiqueDuPalier + eps_prodTherm
                        < PuissanceDisponibleDuPalierThermique[pdtHebdo])
                        PuissanceDisponibleDuPalierThermique[pdtHebdo]
                          = ProductionThermiqueDuPalier + eps_prodTherm;

                    double NombreDeGroupesEnMarcheDuPalier
                      = ProductionThermique[pdtHebdo]->NombreDeGroupesEnMarcheDuPalier[index];

                    if (NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes
                        > NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo])
                        NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                          = (int)ceil(NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes);
                }
            }
            else
            {
                int* NombreMinDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->NombreMinDeGroupesEnMarcheDuPalierThermique;
                int* NombreMaxDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->NombreMaxDeGroupesEnMarcheDuPalierThermique;
                double* PuissanceDisponibleDuPalierThermique
                  = PuissanceDisponibleEtCout[index]->PuissanceDisponibleDuPalierThermique;
                double pminDUnGroupeDuPalierThermique
                  = PaliersThermiquesDuPays->pminDUnGroupeDuPalierThermique[index];
                double PmaxDUnGroupeDuPalierThermique
                  = PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[index];

                for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
                {
                    double X
                      = ProductionThermique[pdtHebdo]->NombreDeGroupesEnMarcheDuPalier[index];
                    if (X > NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo] + Eps)
                    {
                        printf(
                          "Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                        printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e max %d\n",
                               pays,
                               index,
                               X,
                               NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]);
                    }
                    if (X < NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] - Eps)
                    {
                        printf(
                          "Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                        printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e min %d\n",
                               pays,
                               index,
                               X,
                               NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo]);
                    }
                    double P = ProductionThermique[pdtHebdo]->ProductionThermiqueDuPalier[index];
                    if (P < X * pminDUnGroupeDuPalierThermique - Eps)
                    {
                        printf(
                          "Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                        printf(
                          "Pays %d palier dans le pays %d P %e < NbGroupe (%e) * PminGroupe (%e)\n",
                          pays,
                          index,
                          P,
                          X,
                          pminDUnGroupeDuPalierThermique);
                    }
                    if (P > X * PmaxDUnGroupeDuPalierThermique + Eps)
                    {
                        printf(
                          "Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                        printf(
                          "Pays %d palier dans le pays %d P %e > NbGroupe (%e) * PmaxGroupe (%e)\n",
                          pays,
                          index,
                          P,
                          X,
                          PmaxDUnGroupeDuPalierThermique);
                    }

                    NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = (int)ceil(X);
                }

                OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
                  problemeHebdo, NombreMinDeGroupesEnMarcheDuPalierThermique, pays, index);

                for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
                {
                    if (NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                        < NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo])
                        NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                          = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];

                    if (pminDUnGroupeDuPalierThermique
                          * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                        > PuissanceDisponibleDuPalierThermique[pdtHebdo])
                        PuissanceDisponibleDuPalierThermique[pdtHebdo]
                          = pminDUnGroupeDuPalierThermique
                            * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
                }
            }
        }
    }

    return;
}

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  int* NbMinOptDeGroupesEnMarche,
  int Pays,
  int Index)
{
    int NombreDePasDeTemps = problemeHebdo->NombreDePasDeTemps;

    const PALIERS_THERMIQUES* PaliersThermiquesDuPays
      = problemeHebdo->PaliersThermiquesDuPays[Pays];

    const int* NombreMaxDeGroupesEnMarcheDuPalierThermique
      = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]
          ->NombreMaxDeGroupesEnMarcheDuPalierThermique;
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[Index];
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index];

    PRODUCTION_THERMIQUE_OPTIMALE** ProductionThermique
      = problemeHebdo->ResultatsHoraires[Pays]->ProductionThermique;

    bool ResoudreLeProblemeLineaire = true;

    for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
    {
        int t1 = pdt;
        int t1moins1 = t1 - 1;
        if (t1moins1 < 0)
            t1moins1 = NombreDePasDeTemps + t1moins1;

        if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] < 0)
        {
            ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] = 0;
            ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index]
              = NbMinOptDeGroupesEnMarche[t1moins1] - NbMinOptDeGroupesEnMarche[t1];
            ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                < NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1])
            {
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                    < ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index])
                {
                    ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index]
                      = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                        - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                }
                else
                {
                    ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index]
                      = ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index];
                }
            }
        }
        else if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] > 0)
        {
            ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index]
              = NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1];
            ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] = 0;
            ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;
        }
        else
        {
            ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] = 0;
            ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] = 0;
            ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;
        }
    }

    for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
    {
        if (DureeMinimaleDeMarcheDUnGroupeDuPalierThermique > 0)
        {
            double SMarche = 0;
            int t1;
            for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
            {
                t1 = k;

                if (k < 0)
                    t1 = NombreDePasDeTemps + k;
                SMarche
                  += ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index]
                     - ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index];
            }
            if (NbMinOptDeGroupesEnMarche[t1] < SMarche)
            {
                ResoudreLeProblemeLineaire = true;
                break;
            }
        }

        int t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;

        if (t1 < 0)
            t1 = NombreDePasDeTemps + t1;
        double SArret = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            t1 = k;

            if (t1 < 0)
                t1 = NombreDePasDeTemps + t1;
            int t1moins1 = t1 - 1;
            if (t1moins1 < 0)
                t1moins1 = NombreDePasDeTemps + t1moins1;
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                  - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                > 0)
            {
                SArret += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                          - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
            }
            SArret -= ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index];
        }
        if (NbMinOptDeGroupesEnMarche[t1] > SArret)
        {
            ResoudreLeProblemeLineaire = true;
            break;
        }
    }

    if (!ResoudreLeProblemeLineaire)
        return;

    int NombreDeVariables = 0;
    NombreDeVariables += NombreDePasDeTemps;
    NombreDeVariables += NombreDePasDeTemps;
    NombreDeVariables += NombreDePasDeTemps;
    NombreDeVariables += NombreDePasDeTemps;

    auto NumeroDeVariableDeM = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    auto NumeroDeVariableDeMMoinsMoins = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    auto NumeroDeVariableDeMPlus = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    auto NumeroDeVariableDeMMoins = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));

    int NombreDeContraintes = 0;
    NombreDeContraintes += NombreDePasDeTemps;
    NombreDeContraintes += NombreDePasDeTemps;
    NombreDeContraintes += NombreDePasDeTemps;
    NombreDeContraintes += NombreDePasDeTemps;
    NombreDeContraintes += NombreDePasDeTemps;

    auto PositionDeLaVariable = (int*)MemAlloc(NombreDeVariables * sizeof(int));
    auto CoutLineaire = (double*)MemAlloc(NombreDeVariables * sizeof(double));
    auto Xsolution = (double*)MemAlloc(NombreDeVariables * sizeof(double));
    auto Xmin = (double*)MemAlloc(NombreDeVariables * sizeof(double));
    auto Xmax = (double*)MemAlloc(NombreDeVariables * sizeof(double));
    auto TypeDeVariable = (int*)MemAlloc(NombreDeVariables * sizeof(int));

    auto ComplementDeLaBase = (int*)MemAlloc(NombreDeContraintes * sizeof(int));
    auto IndicesDebutDeLigne = (int*)MemAlloc(NombreDeContraintes * sizeof(int));
    auto NombreDeTermesDesLignes = (int*)MemAlloc(NombreDeContraintes * sizeof(int));
    auto Sens = (char*)MemAlloc(NombreDeContraintes * sizeof(char));
    auto SecondMembre = (double*)MemAlloc(NombreDeContraintes * sizeof(double));

    int NbTermesMatrice = 0;
    NbTermesMatrice += 4 * NombreDePasDeTemps;
    NbTermesMatrice += 2 * NombreDePasDeTemps;
    NbTermesMatrice += 1 * NombreDePasDeTemps;
    NbTermesMatrice
      += NombreDePasDeTemps * (1 + (2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique));
    NbTermesMatrice += NombreDePasDeTemps * (1 + DureeMinimaleDArretDUnGroupeDuPalierThermique);

    auto IndicesColonnes = (int*)MemAlloc(NbTermesMatrice * sizeof(int));
    auto CoefficientsDeLaMatriceDesContraintes
      = (double*)MemAlloc(NbTermesMatrice * sizeof(double));

    if (NumeroDeVariableDeM == nullptr || NumeroDeVariableDeMMoinsMoins == nullptr
        || NumeroDeVariableDeMPlus == nullptr || NumeroDeVariableDeMMoins == nullptr
        || PositionDeLaVariable == nullptr || CoutLineaire == nullptr || Xsolution == nullptr
        || Xmin == nullptr || Xmax == nullptr || TypeDeVariable == nullptr
        || ComplementDeLaBase == nullptr || IndicesDebutDeLigne == nullptr
        || NombreDeTermesDesLignes == nullptr || Sens == nullptr || SecondMembre == nullptr
        || IndicesColonnes == nullptr || CoefficientsDeLaMatriceDesContraintes == nullptr)
    {
        logs.info();
        logs.error() << "Internal error: insufficient memory";
        logs.info();
        AntaresSolverEmergencyShutdown();
        return;
    }

    NombreDeVariables = 0;
    for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
    {
        NumeroDeVariableDeM[pdt] = NombreDeVariables;
        CoutLineaire[NombreDeVariables] = 1;
        Xsolution[NombreDeVariables] = 0;
        Xmin[NombreDeVariables] = NbMinOptDeGroupesEnMarche[pdt];
        Xmax[NombreDeVariables] = NombreMaxDeGroupesEnMarcheDuPalierThermique[pdt];
        TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
        NombreDeVariables++;

        NumeroDeVariableDeMMoinsMoins[pdt] = NombreDeVariables;
        CoutLineaire[NombreDeVariables] = 0;
        Xsolution[NombreDeVariables] = 0;
        Xmin[NombreDeVariables] = 0;
        TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
        NombreDeVariables++;

        NumeroDeVariableDeMPlus[pdt] = NombreDeVariables;
        CoutLineaire[NombreDeVariables] = 0;
        Xsolution[NombreDeVariables] = 0;
        Xmin[NombreDeVariables] = 0;

        Xmax[NombreDeVariables] = LINFINI_ANTARES;
        TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
        NombreDeVariables++;

        NumeroDeVariableDeMMoins[pdt] = NombreDeVariables;
        CoutLineaire[NombreDeVariables] = 0;
        Xsolution[NombreDeVariables] = 0;
        Xmin[NombreDeVariables] = 0;

        Xmax[NombreDeVariables] = LINFINI_ANTARES;
        TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
        NombreDeVariables++;
    }

    NbTermesMatrice = 0;
    NombreDeContraintes = 0;
    for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
    {
        int NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        int t1moins1 = pdt - 1;
        if (t1moins1 < 0)
            t1moins1 = NombreDePasDeTemps + t1moins1;
        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[t1moins1];
        NombreDeTermes++;
        NbTermesMatrice++;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMPlus[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
        Sens[NombreDeContraintes] = '=';
        SecondMembre[NombreDeContraintes] = 0;
        NombreDeContraintes++;

        NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoinsMoins[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
        Sens[NombreDeContraintes] = '<';
        SecondMembre[NombreDeContraintes] = 0;
        NombreDeContraintes++;

        int var = NumeroDeVariableDeMMoinsMoins[pdt];
        int t1 = pdt;
        t1moins1 = t1 - 1;
        if (t1moins1 < 0)
            t1moins1 = NombreDePasDeTemps + t1moins1;
        Xmax[var] = 0;
        if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
              - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
            > 0)
        {
            Xmax[var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                        - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
        }

        NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;
        for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int tmp = k;
            if (tmp < 0)
                tmp = NombreDePasDeTemps + tmp;

            CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
            IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMPlus[tmp];
            NombreDeTermes++;
            NbTermesMatrice++;

            CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
            IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoinsMoins[tmp];
            NombreDeTermes++;
            NbTermesMatrice++;
        }

        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
        Sens[NombreDeContraintes] = '>';
        SecondMembre[NombreDeContraintes] = 0;
        NombreDeContraintes++;

        NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int tmp = k;
            if (tmp < 0)
                tmp = NombreDePasDeTemps + tmp;
            CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
            IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[tmp];
            NombreDeTermes++;
            NbTermesMatrice++;
        }

        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
        Sens[NombreDeContraintes] = '<';
        t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;
        if (t1 < 0)
            t1 = NombreDePasDeTemps + t1;
        SecondMembre[NombreDeContraintes] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            t1 = k;

            if (t1 < 0)
                t1 = NombreDePasDeTemps + t1;
            t1moins1 = t1 - 1;
            if (t1moins1 < 0)
                t1moins1 = NombreDePasDeTemps + t1moins1;
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                  - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                > 0)
            {
                SecondMembre[NombreDeContraintes]
                  += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                     - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
            }
        }
        NombreDeContraintes++;
    }

#ifdef TRACES
    printf(
      "Optimisation auxiliaire: NombreDeVariables %d NombreDeContraintes %d NbTermesMatrice %d\n",
      NombreDeVariables,
      NombreDeContraintes,
      NbTermesMatrice);
#endif

    PROBLEME_SIMPLEXE Probleme;

    Probleme.Contexte = SIMPLEXE_SEUL;
    Probleme.BaseDeDepartFournie = NON_SPX;

    Probleme.NombreMaxDIterations = -1;
    Probleme.DureeMaxDuCalcul = -1.;

    Probleme.CoutLineaire = CoutLineaire;
    Probleme.X = Xsolution;
    Probleme.Xmin = Xmin;
    Probleme.Xmax = Xmax;
    Probleme.NombreDeVariables = NombreDeVariables;
    Probleme.TypeDeVariable = TypeDeVariable;

    Probleme.NombreDeContraintes = NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = IndicesDebutDeLigne;
    Probleme.NombreDeTermesDesLignes = NombreDeTermesDesLignes;
    Probleme.IndicesColonnes = IndicesColonnes;
    Probleme.CoefficientsDeLaMatriceDesContraintes = CoefficientsDeLaMatriceDesContraintes;
    Probleme.Sens = Sens;
    Probleme.SecondMembre = SecondMembre;

    Probleme.ChoixDeLAlgorithme = SPX_DUAL;

    Probleme.TypeDePricing = PRICING_STEEPEST_EDGE;
    Probleme.FaireDuScaling = OUI_SPX;
    Probleme.StrategieAntiDegenerescence = AGRESSIF;

    Probleme.PositionDeLaVariable = PositionDeLaVariable;
    Probleme.NbVarDeBaseComplementaires = 0;
    Probleme.ComplementDeLaBase = ComplementDeLaBase;

    Probleme.LibererMemoireALaFin = OUI_SPX;

    Probleme.UtiliserCoutMax = NON_SPX;
    Probleme.CoutMax = 0.0;

    Probleme.CoutsMarginauxDesContraintes = nullptr;
    Probleme.CoutsReduits = nullptr;

    Probleme.AffichageDesTraces = NON_SPX;

    Probleme.NombreDeContraintesCoupes = 0;

    PROBLEME_SPX* ProbSpx = nullptr;
    ProbSpx = SPX_Simplexe(&Probleme, ProbSpx);

    if (Probleme.ExistenceDUneSolution == OUI_SPX)
    {
        for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            NbMinOptDeGroupesEnMarche[pdt] = (int)ceil(Xsolution[NumeroDeVariableDeM[pdt]]);
        }
    }
    else
    {
#ifdef TRACES
        printf("Pas de solution au probleme auxiliaire\n");
#endif
    }

    MemFree(NumeroDeVariableDeM);
    MemFree(NumeroDeVariableDeMMoinsMoins);
    MemFree(NumeroDeVariableDeMPlus);
    MemFree(NumeroDeVariableDeMMoins);
    MemFree(PositionDeLaVariable);
    MemFree(CoutLineaire);
    MemFree(Xsolution);
    MemFree(Xmin);
    MemFree(Xmax);
    MemFree(TypeDeVariable);
    MemFree(ComplementDeLaBase);
    MemFree(IndicesDebutDeLigne);
    MemFree(NombreDeTermesDesLignes);
    MemFree(Sens);
    MemFree(SecondMembre);
    MemFree(IndicesColonnes);
    MemFree(CoefficientsDeLaMatriceDesContraintes);

    return;
}
