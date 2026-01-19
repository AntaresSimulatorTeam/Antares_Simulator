// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"

void H2O2_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles,
                                             int NumeroDeProbleme)
{
    const auto& TurbineMax = DonneesMensuelles.TurbineMax;
    const auto& TurbineMin = DonneesMensuelles.TurbineMin;

    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    int NbPdt = ProblemeHydrauliqueEtendu.NbJoursDUnProbleme[NumeroDeProbleme];

    auto& CorrespondanceDesVariables = ProblemeHydrauliqueEtendu
                                         .CorrespondanceDesVariables[NumeroDeProbleme];
    auto& ProblemeLineaireEtenduPartieVariable = ProblemeHydrauliqueEtendu
                                                   .ProblemeLineaireEtenduPartieVariable
                                                     [NumeroDeProbleme];

    auto& NumeroVar_Turbine = CorrespondanceDesVariables.NumeroVar_Turbine;

    auto& Xmax = ProblemeLineaireEtenduPartieVariable.Xmax;
    auto& Xmin = ProblemeLineaireEtenduPartieVariable.Xmin;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        int Var = NumeroVar_Turbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];
        Xmin[Var] = TurbineMin[Pdt];
    }

    return;
}
