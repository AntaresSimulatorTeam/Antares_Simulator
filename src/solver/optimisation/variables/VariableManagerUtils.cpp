#include "VariableManagerUtils.h"

VariableManagement::VariableManagerFactory VariableManagerFactoryFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo)
{
    return VariableManagement::VariableManagerFactory(
      problemeHebdo->CorrespondanceVarNativesVarOptim,
      problemeHebdo->NumeroDeVariableStockFinal,
      problemeHebdo->NumeroDeVariableDeTrancheDeStock,
      problemeHebdo->NombreDePasDeTempsPourUneOptimisation);
}