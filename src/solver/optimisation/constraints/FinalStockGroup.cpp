#include "FinalStockGroup.h"

std::shared_ptr<FinalStockEquivalentData> FinalStockGroup::GetFinalStockEquivalentData(
  uint32_t pays)
{
    FinalStockEquivalentData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].AccurateWaterValue,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].DirectLevelAccess,
         problemeHebdo_->NumeroDeContrainteEquivalenceStockFinal};
    return std::make_shared<FinalStockEquivalentData>(data);
}

std::shared_ptr<FinalStockExpressionData> FinalStockGroup::GetFinalStockExpressionData(
  uint32_t pays)
{
    FinalStockExpressionData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].AccurateWaterValue,
         problemeHebdo_->NumeroDeContrainteExpressionStockFinal};
    return std::make_shared<FinalStockExpressionData>(data);
}

void FinalStockGroup::Build()
{
    FinalStockEquivalent finalStockEquivalent(builder_);
    FinalStockExpression finalStockExpression(builder_);

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        finalStockEquivalent.add(pays);

        finalStockExpression.add(pays);
    }
}