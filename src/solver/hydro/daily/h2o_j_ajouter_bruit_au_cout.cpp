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
#include <antares/mersenne-twister/mersenne-twister.h>
#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace Constants
{
constexpr double noiseAmplitude = 1e-3;
constexpr unsigned int seed = 0x79686a64; // "hydj" in hexa
} // namespace Constants

void H2O_J_AjouterBruitAuCout(DONNEES_MENSUELLES& donnesMensuelles)
{
    auto& ProblemeHydraulique = donnesMensuelles.ProblemeHydraulique;
    auto& ProblemeLineairePartieFixe = ProblemeHydraulique.ProblemeLineairePartieFixe;
    auto& CorrespondanceDesVariables = ProblemeHydraulique.CorrespondanceDesVariables;
    auto NombreDeProblemes = ProblemeHydraulique.NombreDeProblemes;
    Antares::MersenneTwister noiseGenerator;
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
                                           // the user to change it

    for (int i = 0; i < NombreDeProblemes; i++)
    {
        for (int j = 0; j < ProblemeLineairePartieFixe[i].NombreDeVariables; j++)
        {
            ProblemeLineairePartieFixe[i].CoutLineaire[j] += noiseGenerator()
                                                             * Constants::noiseAmplitude;
        }

        ProblemeLineairePartieFixe[i]
          .CoutLineaire[CorrespondanceDesVariables[i].NumeroDeLaVariableMu]
          += noiseGenerator() * Constants::noiseAmplitude;
        ProblemeLineairePartieFixe[i]
          .CoutLineaire[CorrespondanceDesVariables[i].NumeroDeLaVariableXi]
          += noiseGenerator() * Constants::noiseAmplitude;
    }
}
