#include "constraint_builder_utils.h"

ConstraintBuilderData GetConstraintBuilderDataFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre)
{
    return {
      .Pi = ProblemeAResoudre.Pi,
      .Colonne = ProblemeAResoudre.Colonne,
      .nombreDeContraintes = ProblemeAResoudre.NombreDeContraintes,
      .nombreDeTermesDansLaMatriceDeContrainte
      = ProblemeAResoudre.NombreDeTermesDansLaMatriceDesContraintes,
      .IndicesDebutDeLigne = ProblemeAResoudre.IndicesDebutDeLigne,
      .CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre.CoefficientsDeLaMatriceDesContraintes,
      .IndicesColonnes = ProblemeAResoudre.IndicesColonnes,
      .NombreDeTermesAllouesDansLaMatriceDesContraintes
      = ProblemeAResoudre.NombreDeTermesAllouesDansLaMatriceDesContraintes,
      .NombreDeTermesDesLignes = ProblemeAResoudre.NombreDeTermesDesLignes,
      .Sens = ProblemeAResoudre.Sens,
      .IncrementDAllocationMatriceDesContraintes
      = ProblemeAResoudre.IncrementDAllocationMatriceDesContraintes,
      .CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim,
      .NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeVariableStockFinal = problemeHebdo->NumeroDeVariableStockFinal,
      .NumeroDeVariableDeTrancheDeStock = problemeHebdo->NumeroDeVariableDeTrancheDeStock,
      .NomDesContraintes = ProblemeAResoudre.NomDesContraintes,
      .NamedProblems = problemeHebdo->NamedProblems,
      .NomsDesPays = problemeHebdo->NomsDesPays,
      .weekInTheYear = problemeHebdo->weekInTheYear,
      .NombreDePasDeTemps = problemeHebdo->NombreDePasDeTemps,
      .NbTermesContraintesPourLesCoutsDeDemarrage = nullptr,
      .NombreDePays = problemeHebdo->NombreDePays,
      .NombreDeContraintesCouplantes = problemHebdo->NombreDeContraintesCouplantes,
      .CaracteristiquesHydrauliques = problemHebdo->CaracteristiquesHydrauliques};
}