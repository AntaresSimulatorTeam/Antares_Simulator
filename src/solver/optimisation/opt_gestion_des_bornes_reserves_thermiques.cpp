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

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "variables/VariableManagement.h"
#include "variables/VariableManagerUtils.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_fonctions.h"

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireReservesThermiques(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            auto areaReserves = problemeHebdo->allReserves.thermalAreaReserves[pays];

            int index = 0;
            for (const auto& areaReserveUp : areaReserves.areaCapacityReservationsUp)
            {
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        int var = CorrespondanceVarNativesVarOptim
                                    .clusterReserveUpParticipationIndex[index];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                    }
                    index++;
                }
            }
            index = 0;
            for (const auto& areaReserveDown : areaReserves.areaCapacityReservationsUp)
            {
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        int var = CorrespondanceVarNativesVarOptim
                                    .clusterReserveUpParticipationIndex[index];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                    }
                    index++;
                }
            }
        }
    }
}