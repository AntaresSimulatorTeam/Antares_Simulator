#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

VariableManagement::VariableManager VariableManagerFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    return VariableManagement::VariableManager(
      problemeHebdo->CorrespondanceVarNativesVarOptim,
      problemeHebdo->NumeroDeVariableStockFinal,
      problemeHebdo->NumeroDeVariableDeTrancheDeStock,
      problemeHebdo->NombreDePasDeTempsPourUneOptimisation);
}
