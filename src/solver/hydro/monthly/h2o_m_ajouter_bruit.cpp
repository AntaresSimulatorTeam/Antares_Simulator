/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <vector>

#include <antares/mersenne-twister/mersenne-twister.h>
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace Constants
{
constexpr double noiseAmplitude = 1e-3;
constexpr unsigned int seed = 0x79686d64; // "hydm" in hexa
} // namespace Constants

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
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
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
                                      + noiseGenerator() * Constants::noiseAmplitude;
        }
    }
    int Var = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaireBruite[Var] = CoutLineaire[Var] + noiseGenerator() * Constants::noiseAmplitude;

    Var = CorrespondanceDesVariables.NumeroDeLaVariableXi;
    CoutLineaireBruite[Var] = CoutLineaire[Var] + noiseGenerator() * Constants::noiseAmplitude;
}
