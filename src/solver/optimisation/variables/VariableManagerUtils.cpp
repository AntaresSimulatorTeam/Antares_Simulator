// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

VariableManagement::VariableManager VariableManagerFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    return VariableManagement::VariableManager(
      problemeHebdo->CorrespondanceVarNativesVarOptim,
      problemeHebdo->NumeroDeVariableStockFinal,
      problemeHebdo->NumeroDeVariableDeTrancheDeStock,
      problemeHebdo->NombreDePasDeTempsPourUneOptimisation);
}
