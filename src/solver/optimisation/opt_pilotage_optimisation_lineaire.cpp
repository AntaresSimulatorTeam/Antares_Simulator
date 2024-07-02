/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

using Antares::Solver::Optimization::OptimizationOptions;

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions& options,
                                      PROBLEME_HEBDO* problemeHebdo,
                                      const AdqPatchParams& adqPatchParams,
                                      Solver::IResultWriter& writer,
                                      Solver::Simulation::ISimulationObserver& simulationObserver)
{
    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
        if (problemeHebdo->TypeDOptimisation == OPTIMISATION_LINEAIRE)
        {
            for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
            {
                problemeHebdo->CoutDeDefaillanceEnReserve[pays] = 1.e+6;
            }

            problemeHebdo->NombreDeJours = (int)(problemeHebdo->NombreDePasDeTemps
                                                 / problemeHebdo->NombreDePasDeTempsDUneJournee);

            if (!problemeHebdo->OptimisationAuPasHebdomadaire)
            {
                problemeHebdo->NombreDePasDeTempsPourUneOptimisation
                  = problemeHebdo->NombreDePasDeTempsDUneJournee;
            }
            else
            {
                problemeHebdo->NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                                         ->NombreDePasDeTemps;
            }

            OPT_AllocDuProblemeAOptimiser(problemeHebdo);

            OPT_ChainagesDesIntercoPartantDUnNoeud(problemeHebdo);
        }

        problemeHebdo->LeProblemeADejaEteInstancie = true;
    }

    OPT_VerifierPresenceReserveJmoins1(problemeHebdo);

    OPT_InitialiserLesPminHebdo(problemeHebdo);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(problemeHebdo);

    OPT_MaxDesPmaxHydrauliques(problemeHebdo);

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(problemeHebdo);
    }

    return OPT_OptimisationLineaire(options,
                                    problemeHebdo,
                                    adqPatchParams,
                                    writer,
                                    simulationObserver);
}
