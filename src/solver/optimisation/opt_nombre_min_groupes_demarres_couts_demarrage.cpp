// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <spx_fonctions.h>

#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  int Pays,
  int index,
  int NombreDePasDeTempsProblemeHebdo,
  RESULTATS_HORAIRES& ResultatsHoraires,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays);

void OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  int NombreDePasDeTempsProblemeHebdo,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays,
  RESULTATS_HORAIRES& ResultatsHoraires,
  bool expansion,
  int index,
  bool OptimisationAvecVariablesEntieres,
  int pays)
{
    double Eps = 1.e-3;
    double eps_prodTherm = 1.0;
    double eps_nbGroupes = 1.0;

    std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;
    const std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermique = ResultatsHoraires
                                                                              .ProductionThermique;
    if (expansion)
    {
        std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
          = PuissanceDisponibleEtCout[index].NombreMinDeGroupesEnMarcheDuPalierThermique;
        std::vector<double>& PuissanceDisponibleDuPalierThermique
          = PuissanceDisponibleEtCout[index].PuissanceDisponibleDuPalierThermique;
        std::vector<double>& PuissanceMinDuPalierThermique = PuissanceDisponibleEtCout[index]
                                                               .PuissanceMinDuPalierThermique;

        for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
        {
            double ProductionThermiqueDuPalier = ProductionThermique[pdtHebdo]
                                                   .ProductionThermiqueDuPalier[index];

            if (ProductionThermiqueDuPalier - eps_prodTherm
                > PuissanceMinDuPalierThermique[pdtHebdo])
            {
                PuissanceMinDuPalierThermique[pdtHebdo] = ProductionThermiqueDuPalier
                                                          - eps_prodTherm;
            }

            if (ProductionThermiqueDuPalier + eps_prodTherm
                < PuissanceDisponibleDuPalierThermique[pdtHebdo])
            {
                PuissanceDisponibleDuPalierThermique[pdtHebdo] = ProductionThermiqueDuPalier
                                                                 + eps_prodTherm;
            }

            double NombreDeGroupesEnMarcheDuPalier = ProductionThermique[pdtHebdo]
                                                       .NombreDeGroupesEnMarcheDuPalier[index];

            if (NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes
                > NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo])
            {
                NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = (int)ceil(
                  NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes);
            }
        }
    }
    else
    {
        std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
          = PuissanceDisponibleEtCout[index].NombreMinDeGroupesEnMarcheDuPalierThermique;
        std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PuissanceDisponibleEtCout[index].NombreMaxDeGroupesEnMarcheDuPalierThermique;
        std::vector<double>& PuissanceDisponibleDuPalierThermique
          = PuissanceDisponibleEtCout[index].PuissanceDisponibleDuPalierThermique;
        double pminDUnGroupeDuPalierThermique = PaliersThermiquesDuPays
                                                  .pminDUnGroupeDuPalierThermique[index];
        double PmaxDUnGroupeDuPalierThermique = PaliersThermiquesDuPays
                                                  .PmaxDUnGroupeDuPalierThermique[index];

        for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
        {
            double X = ProductionThermique[pdtHebdo].NombreDeGroupesEnMarcheDuPalier[index];
            if (X > NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo] + Eps)
            {
                printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e max %d\n",
                       pays,
                       index,
                       X,
                       NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]);
            }
            if (X < NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] - Eps)
            {
                printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e min %d\n",
                       pays,
                       index,
                       X,
                       NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo]);
            }
            double P = ProductionThermique[pdtHebdo].ProductionThermiqueDuPalier[index];
            if (P < X * pminDUnGroupeDuPalierThermique - Eps)
            {
                printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                printf("Pays %d palier dans le pays %d P %e < NbGroupe (%e) * PminGroupe (%e)\n",
                       pays,
                       index,
                       P,
                       X,
                       pminDUnGroupeDuPalierThermique);
            }
            if (P > X * PmaxDUnGroupeDuPalierThermique + Eps)
            {
                printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
                printf("Pays %d palier dans le pays %d P %e > NbGroupe (%e) * PmaxGroupe (%e)\n",
                       pays,
                       index,
                       P,
                       X,
                       PmaxDUnGroupeDuPalierThermique);
            }

            NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = (int)Antares::Utils::ceil(X);
        }

        if (!OptimisationAvecVariablesEntieres)
        {
            OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
              NombreMinDeGroupesEnMarcheDuPalierThermique,
              pays,
              index,
              NombreDePasDeTempsProblemeHebdo,
              ResultatsHoraires,
              PaliersThermiquesDuPays);

            for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
            {
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                    < NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo])
                {
                    NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                      = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
                }

                if (pminDUnGroupeDuPalierThermique
                      * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                    > PuissanceDisponibleDuPalierThermique[pdtHebdo])
                {
                    PuissanceDisponibleDuPalierThermique[pdtHebdo]
                      = pminDUnGroupeDuPalierThermique
                        * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
                }
            }
        }
        else
        {
            for (int pdtHebdo = 0; pdtHebdo < NombreDePasDeTempsProblemeHebdo; pdtHebdo++)
            {
                NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                  = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
            }
        }
    }
}

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  int Pays,
  int index,
  int NombreDePasDeTempsProblemeHebdo,
  RESULTATS_HORAIRES& ResultatsHoraires,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays)
{
    std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index]
          .NombreMaxDeGroupesEnMarcheDuPalierThermique;
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[index];

    std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermique = ResultatsHoraires
                                                                        .ProductionThermique;

    bool ResoudreLeProblemeLineaire = true;

    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
    {
        int t1 = pdt;
        int t1moins1 = t1 - 1;
        if (t1moins1 < 0)
        {
            t1moins1 = NombreDePasDeTempsProblemeHebdo + t1moins1;
        }

        if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] < 0)
        {
            ProductionThermique[t1].NombreDeGroupesQuiDemarrentDuPalier[index] = 0;
            ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index]
              = NbMinOptDeGroupesEnMarche[t1moins1] - NbMinOptDeGroupesEnMarche[t1];
            ProductionThermique[t1].NombreDeGroupesQuiTombentEnPanneDuPalier[index] = 0;
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                < NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1])
            {
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                    < ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index])
                {
                    ProductionThermique[t1].NombreDeGroupesQuiTombentEnPanneDuPalier[index]
                      = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                        - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                }
                else
                {
                    ProductionThermique[t1].NombreDeGroupesQuiTombentEnPanneDuPalier[index]
                      = ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index];
                }
            }
        }
        else if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] > 0)
        {
            ProductionThermique[t1].NombreDeGroupesQuiDemarrentDuPalier[index]
              = NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1];
            ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index] = 0;
            ProductionThermique[t1].NombreDeGroupesQuiTombentEnPanneDuPalier[index] = 0;
        }
        else
        {
            ProductionThermique[t1].NombreDeGroupesQuiDemarrentDuPalier[index] = 0;
            ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index] = 0;
            ProductionThermique[t1].NombreDeGroupesQuiTombentEnPanneDuPalier[index] = 0;
        }
    }

    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
    {
        if (DureeMinimaleDeMarcheDUnGroupeDuPalierThermique > 0)
        {
            double SMarche = 0;
            int t1;
            for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
            {
                t1 = k;

                if (k < 0)
                {
                    t1 = NombreDePasDeTempsProblemeHebdo + k;
                }
                SMarche += ProductionThermique[t1].NombreDeGroupesQuiDemarrentDuPalier[index]
                           - ProductionThermique[t1]
                               .NombreDeGroupesQuiTombentEnPanneDuPalier[index];
            }
            if (NbMinOptDeGroupesEnMarche[t1] < SMarche)
            {
                ResoudreLeProblemeLineaire = true;
                break;
            }
        }

        int t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;

        if (t1 < 0)
        {
            t1 = NombreDePasDeTempsProblemeHebdo + t1;
        }
        double SArret = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            t1 = k;

            if (t1 < 0)
            {
                t1 = NombreDePasDeTempsProblemeHebdo + t1;
            }
            int t1moins1 = t1 - 1;
            if (t1moins1 < 0)
            {
                t1moins1 = NombreDePasDeTempsProblemeHebdo + t1moins1;
            }
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                  - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                > 0)
            {
                SArret += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                          - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
            }
            SArret -= ProductionThermique[t1].NombreDeGroupesQuiSArretentDuPalier[index];
        }
        if (NbMinOptDeGroupesEnMarche[t1] > SArret)
        {
            ResoudreLeProblemeLineaire = true;
            break;
        }
    }

    if (!ResoudreLeProblemeLineaire)
    {
        return;
    }

    int NombreDeVariables = 0;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;

    std::vector<int> NumeroDeVariableDeM(NombreDePasDeTempsProblemeHebdo);
    std::vector<int> NumeroDeVariableDeMMoinsMoins(NombreDePasDeTempsProblemeHebdo);
    std::vector<int> NumeroDeVariableDeMPlus(NombreDePasDeTempsProblemeHebdo);
    std::vector<int> NumeroDeVariableDeMMoins(NombreDePasDeTempsProblemeHebdo);

    int NombreDeContraintes = 0;
    NombreDeContraintes += NombreDePasDeTempsProblemeHebdo;
    NombreDeContraintes += NombreDePasDeTempsProblemeHebdo;
    NombreDeContraintes += NombreDePasDeTempsProblemeHebdo;
    NombreDeContraintes += NombreDePasDeTempsProblemeHebdo;
    NombreDeContraintes += NombreDePasDeTempsProblemeHebdo;

    std::vector<int> PositionDeLaVariable(NombreDeVariables);
    std::vector<double> CoutLineaire(NombreDeVariables);
    std::vector<double> Xsolution(NombreDeVariables);
    std::vector<double> Xmin(NombreDeVariables);
    std::vector<double> Xmax(NombreDeVariables);
    std::vector<int> TypeDeVariable(NombreDeVariables);

    std::vector<int> ComplementDeLaBase(NombreDeContraintes);
    std::vector<int> IndicesDebutDeLigne(NombreDeContraintes);
    std::vector<int> NombreDeTermesDesLignes(NombreDeContraintes);
    std::vector<char> Sens(NombreDeContraintes);
    std::vector<double> SecondMembre(NombreDeContraintes);

    int NbTermesMatrice = 0;
    NbTermesMatrice += 4 * NombreDePasDeTempsProblemeHebdo;
    NbTermesMatrice += 2 * NombreDePasDeTempsProblemeHebdo;
    NbTermesMatrice += 1 * NombreDePasDeTempsProblemeHebdo;
    NbTermesMatrice += NombreDePasDeTempsProblemeHebdo
                       * (1 + (2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique));
    NbTermesMatrice += NombreDePasDeTempsProblemeHebdo
                       * (1 + DureeMinimaleDArretDUnGroupeDuPalierThermique);

    std::vector<int> IndicesColonnes(NbTermesMatrice);
    std::vector<double> CoefficientsDeLaMatriceDesContraintes(NbTermesMatrice);

    NombreDeVariables = 0;
    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
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
    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
    {
        int NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;

        CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
        IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[pdt];
        NombreDeTermes++;
        NbTermesMatrice++;

        int t1moins1 = pdt - 1;
        if (t1moins1 < 0)
        {
            t1moins1 = NombreDePasDeTempsProblemeHebdo + t1moins1;
        }
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
        {
            t1moins1 = NombreDePasDeTempsProblemeHebdo + t1moins1;
        }
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
            {
                tmp = NombreDePasDeTempsProblemeHebdo + tmp;
            }

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
            {
                tmp = NombreDePasDeTempsProblemeHebdo + tmp;
            }
            CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
            IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[tmp];
            NombreDeTermes++;
            NbTermesMatrice++;
        }

        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
        Sens[NombreDeContraintes] = '<';
        t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;
        if (t1 < 0)
        {
            t1 = NombreDePasDeTempsProblemeHebdo + t1;
        }
        SecondMembre[NombreDeContraintes] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            t1 = k;

            if (t1 < 0)
            {
                t1 = NombreDePasDeTempsProblemeHebdo + t1;
            }
            t1moins1 = t1 - 1;
            if (t1moins1 < 0)
            {
                t1moins1 = NombreDePasDeTempsProblemeHebdo + t1moins1;
            }
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                  - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                > 0)
            {
                SecondMembre[NombreDeContraintes] += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                                                     - NombreMaxDeGroupesEnMarcheDuPalierThermique
                                                       [t1moins1];
            }
        }
        NombreDeContraintes++;
    }

#ifdef TRACES
    printf(
      "LinearProblem auxiliaire: NombreDeVariables %d NombreDeContraintes %d NbTermesMatrice %d\n",
      NombreDeVariables,
      NombreDeContraintes,
      NbTermesMatrice);
#endif

    PROBLEME_SIMPLEXE Probleme;

    Probleme.Contexte = SIMPLEXE_SEUL;
    Probleme.BaseDeDepartFournie = NON_SPX;

    Probleme.NombreMaxDIterations = -1;
    Probleme.DureeMaxDuCalcul = -1.;

    Probleme.CoutLineaire = CoutLineaire.data();
    Probleme.X = Xsolution.data();
    Probleme.Xmin = Xmin.data();
    Probleme.Xmax = Xmax.data();
    Probleme.NombreDeVariables = NombreDeVariables;
    Probleme.TypeDeVariable = TypeDeVariable.data();

    Probleme.NombreDeContraintes = NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = IndicesDebutDeLigne.data();
    Probleme.NombreDeTermesDesLignes = NombreDeTermesDesLignes.data();
    Probleme.IndicesColonnes = IndicesColonnes.data();
    Probleme.CoefficientsDeLaMatriceDesContraintes = CoefficientsDeLaMatriceDesContraintes.data();
    Probleme.Sens = Sens.data();
    Probleme.SecondMembre = SecondMembre.data();

    Probleme.ChoixDeLAlgorithme = SPX_DUAL;

    Probleme.TypeDePricing = PRICING_STEEPEST_EDGE;
    Probleme.FaireDuScaling = OUI_SPX;
    Probleme.StrategieAntiDegenerescence = AGRESSIF;

    Probleme.PositionDeLaVariable = PositionDeLaVariable.data();
    Probleme.NbVarDeBaseComplementaires = 0;
    Probleme.ComplementDeLaBase = ComplementDeLaBase.data();

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
        for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
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
}
