// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::Solver::Optimization::OptimizationOptions;

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions& options,
                                      PROBLEME_HEBDO* problemeHebdo,
                                      Solver::IResultWriter& writer,
                                      Solver::Simulation::ISimulationObserver& simulationObserver,
                                      OptimisationsSimulationTable* simulationTables)
{
    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
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
                                    writer,
                                    simulationObserver,
                                    simulationTables);
}
