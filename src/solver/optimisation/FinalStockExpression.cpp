#include "FinalStockExpression.h"

void FinalStockExpression::add(int pays)
{
    problemeHebdo->NumeroDeContrainteExpressionStockFinal[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    const auto pdt = problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1;
    builder.updateHourWithinWeek(pdt).include(Variable::FinalStorage(pays), -1.0);
    for (int layerindex = 0; layerindex < 100; layerindex++)
    {
        builder.include(Variable::LayerStorage(pays, layerindex), 1.0);
    }
    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);

    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
    namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
    namer.FinalStockExpression(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    builder.equalTo().build();
}
