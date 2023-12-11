#include "FinalStockGroup.h"

FinalStockEquivalentData FinalStockGroup::GetFinalStockEquivalentData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteEquivalenceStockFinal
            = problemeHebdo_->NumeroDeContrainteEquivalenceStockFinal};
}

FinalStockExpressionData FinalStockGroup::GetFinalStockExpressionData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteExpressionStockFinal
            = problemeHebdo_->NumeroDeContrainteExpressionStockFinal};
}

void FinalStockGroup::BuildConstraints()
{
    auto finalStockEquivalentData = GetFinalStockEquivalentData();
    FinalStockEquivalent finalStockEquivalent(builder_, finalStockEquivalentData);
    auto finalStockExpressionData = GetFinalStockExpressionData();
    FinalStockExpression finalStockExpression(builder_, finalStockExpressionData);

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        finalStockEquivalent.add(pays);

        finalStockExpression.add(pays);
    }
}