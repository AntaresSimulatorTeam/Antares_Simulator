// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <vector>

#include <antares/antares/constants.h>
#include <antares/mersenne-twister/mersenne-twister.h>
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
void H2O_M_AjouterBruitAuCout(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                 .CorrespondanceDesVariables;
    auto& CoutLineaireBruite = ProblemeLineairePartieFixe.CoutLineaireBruite;
    const auto& CoutLineaire = ProblemeLineairePartieFixe.CoutLineaire;

    Antares::MersenneTwister noiseGenerator;
    constexpr unsigned int noiseSeed = 0x79686d64; // "hydm" in hexa
    noiseGenerator.reset(noiseSeed); // Arbitrary seed, hard-coded since we don't really want
    // the user to change it
    const std::vector<const std::vector<int>*> monthlyVariables = {
      &CorrespondanceDesVariables.NumeroDeVariableVolume,
      &CorrespondanceDesVariables.NumeroDeVariableTurbine,
      &CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax,
      &CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin,
      &CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible,
      &CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible};

    for (const auto& variable: monthlyVariables)
    {
        for (int Var: *variable)
        {
            CoutLineaireBruite[Var] = CoutLineaire[Var]
                                      + noiseGenerator() * Antares::Constants::noiseAmplitude;
        }
    }
    int Var = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaireBruite[Var] = CoutLineaire[Var]
                              + noiseGenerator() * Antares::Constants::noiseAmplitude;

    Var = CorrespondanceDesVariables.NumeroDeLaVariableXi;
    CoutLineaireBruite[Var] = CoutLineaire[Var]
                              + noiseGenerator() * Antares::Constants::noiseAmplitude;

    for (int Var: CorrespondanceDesVariables.NumeroDeVariableOverflow)
    {
        CoutLineaireBruite[Var] = CoutLineaire[Var]
                                  + noiseGenerator() * Antares::Constants::noiseAmplitude;
    }
}
} // namespace DonneesOptimisationMensuelle
