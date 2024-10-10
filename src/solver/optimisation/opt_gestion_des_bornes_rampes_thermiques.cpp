/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "variables/VariableManagement.h"
#include "variables/VariableManagerUtils.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_fonctions.h"


void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireRampesThermiques(PROBLEME_HEBDO* problemeHebdo, const int PremierPdtDeLIntervalle, const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
            int maxThermalPlant = PaliersThermiquesDuPays.NombreDePaliersThermiques;

            for (int index = 0; index < maxThermalPlant; index++)
            {
                if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0)
                {
                    const int palier = PaliersThermiquesDuPays
                                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                    int var = CorrespondanceVarNativesVarOptim.powerRampingDecreaseIndex[palier];
                    Xmin[var] = 0;
                    Xmax[var] = LINFINI_ANTARES;


                    var = CorrespondanceVarNativesVarOptim.powerRampingIncreaseIndex[palier];
                    Xmin[var] = 0;
                    Xmax[var] = LINFINI_ANTARES;

                }
            }
        }
    }
}