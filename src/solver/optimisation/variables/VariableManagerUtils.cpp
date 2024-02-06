#include "VariableManagerUtils.h"

VariableManagement::VariableManager VariableManagerFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    return VariableManagement::VariableManagement(
      problemeHebdo->CorrespondanceVarNativesVarOptim,
      problemeHebdo->NumeroDeVariableStockFinal,
      problemeHebdo->NumeroDeVariableDeTrancheDeStock,
      problemeHebdo->NombreDePasDeTempsPourUneOptimisation);
}