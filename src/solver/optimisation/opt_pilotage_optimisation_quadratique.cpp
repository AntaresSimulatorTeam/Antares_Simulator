// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/QuadraticProblemMatrix.h"
#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"
#include "antares/solver/optimisation/opt_appel_solveur_quadratique.h"

bool OPT_PilotageOptimisationQuadratique(const SingleOptimOptions& options,
                                         PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->LeProblemeADejaEteInstancie)
    {
        OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(problemeHebdo);
        auto builder_data = NewGetConstraintBuilderFromProblemHebdo(problemeHebdo);
        ConstraintBuilder builder(builder_data);
        QuadraticProblemMatrix(problemeHebdo, builder).Run();

        problemeHebdo->LeProblemeADejaEteInstancie = true;
    }

    bool result = true;
    if (problemeHebdo->NombreDInterconnexions > 0)
    {
        for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
        {
            OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(problemeHebdo, pdtHebdo);

            OPT_InitialiserLeSecondMembreDuProblemeQuadratique(problemeHebdo, pdtHebdo);

            OPT_InitialiserLesCoutsQuadratiques(problemeHebdo, pdtHebdo);

            result = OPT_AppelDuSolveurQuadratique(options, problemeHebdo->ProblemeAResoudre.get())
                     && result;

            if (!result)
            {
                logs.warning() << "Quadratic Optimisation: No solution, hour " << pdtHebdo;
            }
        }
    }

    return result;
}
