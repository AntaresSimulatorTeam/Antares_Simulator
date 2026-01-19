// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"

ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre)
{
    return {ProblemeAResoudre.Pi,
            ProblemeAResoudre.Colonne,
            ProblemeAResoudre.NombreDeContraintes,
            ProblemeAResoudre.NombreDeTermesDansLaMatriceDesContraintes,
            ProblemeAResoudre.IndicesDebutDeLigne,
            ProblemeAResoudre.CoefficientsDeLaMatriceDesContraintes,
            ProblemeAResoudre.IndicesColonnes,
            ProblemeAResoudre.NombreDeTermesDesLignes,
            ProblemeAResoudre.Sens,
            ProblemeAResoudre.IncrementDAllocationMatriceDesContraintes,
            problemeHebdo->CorrespondanceVarNativesVarOptim,
            problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
            problemeHebdo->NumeroDeVariableStockFinal,
            problemeHebdo->NumeroDeVariableDeTrancheDeStock,
            ProblemeAResoudre.NomDesContraintes,
            problemeHebdo->NomsDesPays,
            problemeHebdo->weekInTheYear,
            problemeHebdo->NombreDePasDeTemps};
}
