#include "constraint_builder_utils.h"

ConstraintBuilderData NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre)
{
    return {ProblemeAResoudre.Pi,
            ProblemeAResoudre.Colonne,
            ProblemeAResoudre.NombreDeContraintes,
            ProblemeAResoudre.NombreDeTermesDansLaMatriceDesContraintes,
            ProblemeAResoudre.IndicesDebutDeLigne,
            ProblemeAResoudre.CoefficientsDeLaMatriceDesContraintes,
            ProblemeAResoudre.IndicesColonnes,
            ProblemeAResoudre.NombreDeTermesAllouesDansLaMatriceDesContraintes,
            ProblemeAResoudre.NombreDeTermesDesLignes,
            ProblemeAResoudre.Sens,
            ProblemeAResoudre.IncrementDAllocationMatriceDesContraintes,
            problemeHebdo->CorrespondanceVarNativesVarOptim,
            problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
            problemeHebdo->NumeroDeVariableStockFinal,
            problemeHebdo->NumeroDeVariableDeTrancheDeStock,
            ProblemeAResoudre.NomDesContraintes,
            problemeHebdo->NamedProblems,
            problemeHebdo->NomsDesPays,
            problemeHebdo->weekInTheYear,
            problemeHebdo->NombreDePasDeTemps,
            nullptr};
}