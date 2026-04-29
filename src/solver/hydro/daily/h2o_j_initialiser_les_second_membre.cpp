// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>

#include <antares/logs/logs.h>
#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES* DonneesMensuelles,
                                     int NumeroDeProbleme,
                                     const std::string& areaName)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    CORRESPONDANCE_DES_CONTRAINTES& CorrespondanceDesContraintes = ProblemeHydraulique
                                                                     .CorrespondanceDesContraintes
                                                                       [NumeroDeProbleme];

    int NumeroDeContrainteDEnergieMensuelle = CorrespondanceDesContraintes
                                                .NumeroDeContrainteDEnergieMensuelle;

    std::vector<double>& SecondMembre = ProblemeHydraulique
                                          .ProblemeLineairePartieVariable[NumeroDeProbleme]
                                          .SecondMembre;

    double turbineMin = 0;
    double turbineMax = 0;
    const int NbPdt = ProblemeHydraulique.NbJoursDUnProbleme[NumeroDeProbleme];
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        int Cnt = CorrespondanceDesContraintes.NumeroDeContrainteSurXi[Pdt];
        SecondMembre[Cnt] = DonneesMensuelles->TurbineCible[Pdt];
        Cnt = CorrespondanceDesContraintes.NumeroDeContrainteSurXiSym[Pdt];
        SecondMembre[Cnt] = -DonneesMensuelles->TurbineCible[Pdt];
        Cnt = CorrespondanceDesContraintes.NumeroDeContrainteSurXiPlus[Pdt];
        SecondMembre[Cnt] = DonneesMensuelles->TurbineCible[Pdt];
        Cnt = CorrespondanceDesContraintes.NumeroDeContrainteSurXiMoins[Pdt];
        SecondMembre[Cnt] = -DonneesMensuelles->TurbineCible[Pdt];

        turbineMin += DonneesMensuelles->TurbineMin[Pdt];
        turbineMax += DonneesMensuelles->TurbineMax[Pdt];
    }

    const double TurbineDuMois = DonneesMensuelles->TurbineDuMois;
    if (TurbineDuMois < turbineMin || TurbineDuMois > turbineMax)
    {
        Antares::logs.warning() << "hydro daily heuristic for area " << areaName
                                << " : target turbine (" << DonneesMensuelles->TurbineDuMois
                                << ") outside of bounds ([" << turbineMin << " , " << turbineMax
                                << "], value was clamped)";
    }

    SecondMembre[NumeroDeContrainteDEnergieMensuelle] = std::clamp(TurbineDuMois,
                                                                   turbineMin,
                                                                   turbineMax);
}
} // namespace DoneesOptimisationJournaliere
