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

#include <antares/antares/fatal-error.h>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/logs/logs.h>
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data;

using Antares::Solver::Optimization::OptimizationOptions;

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions&,
                                      PROBLEME_HEBDO*,
                                      Solver::IResultWriter&,
                                      Solver::Simulation::ISimulationObserver&);
bool OPT_PilotageOptimisationQuadratique(PROBLEME_HEBDO*);
void OPT_LiberationProblemesSimplexe(const PROBLEME_HEBDO*);

void OPT_OptimisationHebdomadaire(const OptimizationOptions& options,
                                  PROBLEME_HEBDO* pProblemeHebdo,
                                  Solver::IResultWriter& writer,
                                  Solver::Simulation::ISimulationObserver& simulationObserver)
{
    if (pProblemeHebdo->TypeDOptimisation == OPTIMISATION_LINEAIRE)
    {
        if (!OPT_PilotageOptimisationLineaire(options, pProblemeHebdo, writer, simulationObserver))
        {
            logs.error() << "Linear optimization failed";
            throw UnfeasibleProblemError("Linear optimization failed");
        }
    }
    else if (pProblemeHebdo->TypeDOptimisation == OPTIMISATION_QUADRATIQUE)
    {
        OPT_LiberationProblemesSimplexe(pProblemeHebdo);
        if (!OPT_PilotageOptimisationQuadratique(pProblemeHebdo))
        {
            logs.error() << "Quadratic optimization failed";
            throw UnfeasibleProblemError("Quadratic optimization failed");
        }
    }
    else
    {
        throw FatalError(
          "Bug: TypeDOptimisation, OPTIMISATION_LINEAIRE ou OPTIMISATION_QUADRATIQUE "
          "non initialise");
    }
}
