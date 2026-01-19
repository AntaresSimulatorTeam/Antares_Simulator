// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_ConstruireLesContraintes(int NbPdt,
                                    std::vector<int>& NumeroDeVariableTurbine,
                                    std::vector<int>& NumeroDeLaVariableXi,
                                    int NumeroDeLaVariableXiPlus,
                                    int NumeroDeLaVariableXiMoins,
                                    std::vector<int>& IndicesDebutDeLigne,
                                    std::vector<char>& Sens,
                                    std::vector<int>& NombreDeTermesDesLignes,
                                    std::vector<double>& CoefficientsDeLaMatriceDesContraintes,
                                    std::vector<int>& IndicesColonnes,
                                    CORRESPONDANCE_DES_CONTRAINTES& CorrespondanceDesContraintes)
{
    int NombreDeContraintes = 0;
    int NombreDeTermes = 0;
    int il = 0;

    IndicesDebutDeLigne[NombreDeContraintes] = il;
    // --- Contrainte somme des turbines = somme des cibles ---
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;
    }

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = NbPdt;

    CorrespondanceDesContraintes.NumeroDeContrainteDEnergieMensuelle = NombreDeContraintes;
    NombreDeContraintes++;

    // --- turbine[t] + Xi >= cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeLaVariableXi[Pdt];
        il++;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;

        CorrespondanceDesContraintes.NumeroDeContrainteSurXi[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- cible[t] + Xi >= turbine[t]  => -turbine[t] + Xi >= -cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXi[Pdt];

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiSym[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- turbine[t] + xi_plus >= cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXiPlus;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiPlus[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- cible[t] + xi_moins >= turbine[t] => -turbine[t] + xi_moins >= -cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXiMoins;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiMoins[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }
}
} // namespace DoneesOptimisationJournaliere
