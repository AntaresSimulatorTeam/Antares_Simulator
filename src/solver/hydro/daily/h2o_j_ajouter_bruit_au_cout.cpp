// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/antares/constants.h>
#include <antares/mersenne-twister/mersenne-twister.h>
#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_AjouterBruitAuCout(DONNEES_MENSUELLES& donnesMensuelles)
{
    auto& ProblemeHydraulique = donnesMensuelles.ProblemeHydraulique;
    auto& ProblemeLineairePartieFixe = ProblemeHydraulique.ProblemeLineairePartieFixe;
    auto& CorrespondanceDesVariables = ProblemeHydraulique.CorrespondanceDesVariables;
    auto NombreDeProblemes = ProblemeHydraulique.NombreDeProblemes;
    Antares::MersenneTwister noiseGenerator;
    constexpr unsigned int costNoiseSeed = 0x79686a64; // "hydj" in hexa

    noiseGenerator.reset(costNoiseSeed); // Arbitrary seed, hard-coded since we don't really want
    // the user to change it

    for (int i = 0; i < NombreDeProblemes; i++)
    {
        for (int j = 0; j < ProblemeLineairePartieFixe[i].NombreDeVariables; j++)
        {
            ProblemeLineairePartieFixe[i].CoutLineaire[j] += noiseGenerator()
                                                             * Antares::Constants::noiseAmplitude;
        }

        for (int pdt = 0; pdt < donnesMensuelles.ProblemeHydraulique.NbJoursDUnProbleme[i]; pdt++)
        {
            ProblemeLineairePartieFixe[i]
              .CoutLineaire[CorrespondanceDesVariables[i].NumeroDeLaVariableXi[pdt]]
              += noiseGenerator() * Antares::Constants::noiseAmplitude;
        }
        ProblemeLineairePartieFixe[i]
          .CoutLineaire[CorrespondanceDesVariables[i].NumeroDeLaVariableXiPlus]
          += noiseGenerator() * Antares::Constants::noiseAmplitude;
        ProblemeLineairePartieFixe[i]
          .CoutLineaire[CorrespondanceDesVariables[i].NumeroDeLaVariableXiMoins]
          += noiseGenerator() * Antares::Constants::noiseAmplitude;
    }
}
} // namespace DoneesOptimisationJournaliere
