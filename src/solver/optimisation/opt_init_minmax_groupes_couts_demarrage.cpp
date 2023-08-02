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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

void OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->OptimisationAvecCoutsDeDemarrage)
        return;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const std::vector<double>& PuissanceDisponibleDuPalierThermiqueRef
              = PuissanceDisponibleEtCout[index].PuissanceDisponibleDuPalierThermiqueRef;
            const std::vector<double>& PuissanceMinDuPalierThermiqueRef
              = PuissanceDisponibleEtCout[index].PuissanceMinDuPalierThermiqueRef;
            std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PuissanceDisponibleEtCout[index].NombreMaxDeGroupesEnMarcheDuPalierThermique;
            std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
              = PuissanceDisponibleEtCout[index].NombreMinDeGroupesEnMarcheDuPalierThermique;

            double TailleUnitaireDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.TailleUnitaireDUnGroupeDuPalierThermique[index];

            for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
            {
                if (TailleUnitaireDUnGroupeDuPalierThermique != 0)
                {
                    NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                      = (int)ceil(PuissanceDisponibleDuPalierThermiqueRef[pdtHebdo]
                                  / TailleUnitaireDUnGroupeDuPalierThermique);

                    NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo]
                      = (int)ceil(PuissanceMinDuPalierThermiqueRef[pdtHebdo]
                                  / TailleUnitaireDUnGroupeDuPalierThermique);
                }
                else
                {
                    NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = 0;
                    NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = 0;
                }
            }
        }
    }

    return;
}
