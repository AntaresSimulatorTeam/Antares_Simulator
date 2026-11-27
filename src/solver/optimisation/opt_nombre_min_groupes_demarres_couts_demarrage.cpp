// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cstddef>
#include <spx_fonctions.h>

#include "antares/solver/optimisation/thermalHeuristicsCore.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  int index,
  int NombreDePasDeTempsProblemeHebdo,
  RESULTATS_HORAIRES& ResultatsHoraires,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays);

namespace
{
void adjustBoundsForExpansion(std::size_t nbTimeSteps,
                              PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout,
                              const std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermique,
                              std::size_t palier)
{
    const double eps_prodTherm = 1.0;
    const double eps_nbGroupes = 1.0;

    std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
      = PuissanceDisponibleEtCout.NombreMinDeGroupesEnMarcheDuPalierThermique;
    std::vector<double>& PuissanceDisponibleDuPalierThermique
      = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique;
    std::vector<double>& PuissanceMinDuPalierThermique = PuissanceDisponibleEtCout
                                                           .PuissanceMinDuPalierThermique;

    for (std::size_t pdtHebdo = 0; pdtHebdo < nbTimeSteps; ++pdtHebdo)
    {
        const double ProductionThermiqueDuPalier = ProductionThermique[pdtHebdo]
                                                     .ProductionThermiqueDuPalier[palier];

        if (ProductionThermiqueDuPalier - eps_prodTherm > PuissanceMinDuPalierThermique[pdtHebdo])
        {
            PuissanceMinDuPalierThermique[pdtHebdo] = ProductionThermiqueDuPalier - eps_prodTherm;
        }

        if (ProductionThermiqueDuPalier + eps_prodTherm
            < PuissanceDisponibleDuPalierThermique[pdtHebdo])
        {
            PuissanceDisponibleDuPalierThermique[pdtHebdo] = ProductionThermiqueDuPalier
                                                             + eps_prodTherm;
        }

        const double NombreDeGroupesEnMarcheDuPalier = ProductionThermique[pdtHebdo]
                                                         .NombreDeGroupesEnMarcheDuPalier[palier];

        if (NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes
            > NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo])
        {
            NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = static_cast<int>(
              ceil(NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes));
        }
    }
}

/// Warns if the solution of the 1st optimisation does not respect the bounds of the cluster
void warnIfInconsistentWithBounds(double X,
                                  double P,
                                  int NombreMinDeGroupes,
                                  int NombreMaxDeGroupes,
                                  double pminDUnGroupe,
                                  double PmaxDUnGroupe,
                                  int pays,
                                  int index)
{
    const double Eps = 1.e-3;
    if (X > NombreMaxDeGroupes + Eps)
    {
        printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
        printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e max %d\n",
               pays,
               index,
               X,
               NombreMaxDeGroupes);
    }
    if (X < NombreMinDeGroupes - Eps)
    {
        printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
        printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e min %d\n",
               pays,
               index,
               X,
               NombreMinDeGroupes);
    }
    if (P < X * pminDUnGroupe - Eps)
    {
        printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
        printf("Pays %d palier dans le pays %d P %e < NbGroupe (%e) * PminGroupe (%e)\n",
               pays,
               index,
               P,
               X,
               pminDUnGroupe);
    }
    if (P > X * PmaxDUnGroupe + Eps)
    {
        printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
        printf("Pays %d palier dans le pays %d P %e > NbGroupe (%e) * PmaxGroupe (%e)\n",
               pays,
               index,
               P,
               X,
               PmaxDUnGroupe);
    }
}
} // namespace

void OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  int NombreDePasDeTempsProblemeHebdo,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays,
  RESULTATS_HORAIRES& ResultatsHoraires,
  bool expansion,
  int index,
  bool OptimisationAvecVariablesEntieres,
  int pays)
{
    const auto palier = static_cast<std::size_t>(index);
    const auto nbTimeSteps = static_cast<std::size_t>(NombreDePasDeTempsProblemeHebdo);
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout = PaliersThermiquesDuPays
                                                                      .PuissanceDisponibleEtCout
                                                                        [palier];
    const std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermique = ResultatsHoraires
                                                                              .ProductionThermique;
    if (expansion)
    {
        adjustBoundsForExpansion(nbTimeSteps,
                                 PuissanceDisponibleEtCout,
                                 ProductionThermique,
                                 palier);
        return;
    }

    std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
      = PuissanceDisponibleEtCout.NombreMinDeGroupesEnMarcheDuPalierThermique;
    std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
      = PuissanceDisponibleEtCout.NombreMaxDeGroupesEnMarcheDuPalierThermique;
    std::vector<double>& PuissanceDisponibleDuPalierThermique
      = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique;
    const double pminDUnGroupeDuPalierThermique = PaliersThermiquesDuPays
                                                    .pminDUnGroupeDuPalierThermique[palier];
    const double PmaxDUnGroupeDuPalierThermique = PaliersThermiquesDuPays
                                                    .PmaxDUnGroupeDuPalierThermique[palier];

    for (std::size_t pdtHebdo = 0; pdtHebdo < nbTimeSteps; ++pdtHebdo)
    {
        const double X = ProductionThermique[pdtHebdo].NombreDeGroupesEnMarcheDuPalier[palier];
        const double P = ProductionThermique[pdtHebdo].ProductionThermiqueDuPalier[palier];
        warnIfInconsistentWithBounds(X,
                                     P,
                                     NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo],
                                     NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo],
                                     pminDUnGroupeDuPalierThermique,
                                     PmaxDUnGroupeDuPalierThermique,
                                     pays,
                                     index);

        NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = static_cast<int>(
          Antares::Utils::ceil(X));
    }

    if (OptimisationAvecVariablesEntieres)
    {
        for (std::size_t pdtHebdo = 0; pdtHebdo < nbTimeSteps; ++pdtHebdo)
        {
            NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
              = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
        }
        return;
    }

    OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
      NombreMinDeGroupesEnMarcheDuPalierThermique,
      index,
      NombreDePasDeTempsProblemeHebdo,
      ResultatsHoraires,
      PaliersThermiquesDuPays);

    for (std::size_t pdtHebdo = 0; pdtHebdo < nbTimeSteps; ++pdtHebdo)
    {
        // When using the ramping model, we must ensure that the NODU don't change
        // during the 2nd optimization. Without this, the solver may bypass the
        // ramping constraints by partially starting/stopping units.
        if (NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
              < NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
            || PaliersThermiquesDuPays.rampingEnabled[palier])
        {
            NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
              = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
        }

        if (pminDUnGroupeDuPalierThermique * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
            > PuissanceDisponibleDuPalierThermique[pdtHebdo])
        {
            PuissanceDisponibleDuPalierThermique[pdtHebdo]
              = pminDUnGroupeDuPalierThermique
                * NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
        }
    }
}

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  int index,
  int NombreDePasDeTempsProblemeHebdo,
  RESULTATS_HORAIRES& ResultatsHoraires,
  PALIERS_THERMIQUES& PaliersThermiquesDuPays)
{
    const auto palier = static_cast<std::size_t>(index);
    const auto nbTimeSteps = static_cast<std::size_t>(NombreDePasDeTempsProblemeHebdo);
    const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[palier]
          .NombreMaxDeGroupesEnMarcheDuPalierThermique;
    const auto DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[palier];
    const auto DureeMinimaleDArretDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[palier];

    std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermique = ResultatsHoraires
                                                                        .ProductionThermique;

    for (std::size_t t1 = 0; t1 < nbTimeSteps; ++t1)
    {
        // The week is cyclic: the time step before the first one is the last one
        const std::size_t t1moins1 = (t1 == 0 ? nbTimeSteps : t1) - 1;
        auto& production = ProductionThermique[t1];

        if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] < 0)
        {
            production.NombreDeGroupesQuiDemarrentDuPalier[palier] = 0;
            production.NombreDeGroupesQuiSArretentDuPalier[palier]
              = NbMinOptDeGroupesEnMarche[t1moins1] - NbMinOptDeGroupesEnMarche[t1];
            production.NombreDeGroupesQuiTombentEnPanneDuPalier[palier] = 0;
            if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                < NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1])
            {
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                    < production.NombreDeGroupesQuiSArretentDuPalier[palier])
                {
                    production.NombreDeGroupesQuiTombentEnPanneDuPalier[palier]
                      = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                        - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                }
                else
                {
                    production.NombreDeGroupesQuiTombentEnPanneDuPalier[palier]
                      = production.NombreDeGroupesQuiSArretentDuPalier[palier];
                }
            }
        }
        else if (NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] > 0)
        {
            production.NombreDeGroupesQuiDemarrentDuPalier[palier] = NbMinOptDeGroupesEnMarche[t1]
                                                                     - NbMinOptDeGroupesEnMarche
                                                                       [t1moins1];
            production.NombreDeGroupesQuiSArretentDuPalier[palier] = 0;
            production.NombreDeGroupesQuiTombentEnPanneDuPalier[palier] = 0;
        }
        else
        {
            production.NombreDeGroupesQuiDemarrentDuPalier[palier] = 0;
            production.NombreDeGroupesQuiSArretentDuPalier[palier] = 0;
            production.NombreDeGroupesQuiTombentEnPanneDuPalier[palier] = 0;
        }
    }

    OPT_ResoudreLePbLineaireDuNombreMinDeGroupesEnMarche(
      NbMinOptDeGroupesEnMarche,
      NombreMaxDeGroupesEnMarcheDuPalierThermique,
      DureeMinimaleDeMarcheDUnGroupeDuPalierThermique,
      DureeMinimaleDArretDUnGroupeDuPalierThermique,
      NombreDePasDeTempsProblemeHebdo);
}

void OPT_ResoudreLePbLineaireDuNombreMinDeGroupesEnMarche(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique,
  int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique,
  int DureeMinimaleDArretDUnGroupeDuPalierThermique,
  int NombreDePasDeTempsProblemeHebdo)
{
    int NombreDeVariables = 0;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;
    NombreDeVariables += NombreDePasDeTempsProblemeHebdo;

    const auto nbTimeSteps = static_cast<std::size_t>(NombreDePasDeTempsProblemeHebdo);
    std::vector<int> NumeroDeVariableDeM(nbTimeSteps);
    std::vector<int> NumeroDeVariableDeMMoinsMoins(nbTimeSteps);
    std::vector<int> NumeroDeVariableDeMPlus(nbTimeSteps);
    std::vector<int> NumeroDeVariableDeMMoins(nbTimeSteps);

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
    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; ++pdt)
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
    for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; ++pdt)
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
        for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; ++pdt)
        {
            const auto t = static_cast<std::size_t>(pdt);
            const auto variable = static_cast<std::size_t>(NumeroDeVariableDeM[t]);
            NbMinOptDeGroupesEnMarche[t] = static_cast<int>(ceil(Xsolution[variable]));
        }
    }
    else
    {
#ifdef TRACES
        printf("Pas de solution au probleme auxiliaire\n");
#endif
    }
}
