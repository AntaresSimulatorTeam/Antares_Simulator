// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

constexpr int maxProblemSize = 31;

namespace DoneesOptimisationJournaliere
{
DONNEES_MENSUELLES H2O_J_Instanciation()
{
    DONNEES_MENSUELLES DonneesMensuelles{};

    DonneesMensuelles.TurbineMax.assign(maxProblemSize, 0.);
    DonneesMensuelles.TurbineMin.assign(maxProblemSize, 0.);
    DonneesMensuelles.TurbineCible.assign(maxProblemSize, 0.);
    DonneesMensuelles.Turbine.assign(maxProblemSize, 0.);

    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles.ProblemeHydraulique;

    // TODO Ok this is stupid
    ProblemeHydraulique.NombreDeProblemes = 4;
    int NombreDeProblemes = ProblemeHydraulique.NombreDeProblemes;

    ProblemeHydraulique.ProblemeLineairePartieFixe.resize(NombreDeProblemes);
    ProblemeHydraulique.ProblemeLineairePartieVariable.resize(NombreDeProblemes);

    return DonneesMensuelles;
}
} // namespace DoneesOptimisationJournaliere
