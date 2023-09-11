
#include "FictitiousLoad.h"

void FictitiousLoad::add(int pdt,
                         int pays,
                         std::vector<int>& NumeroDeContraintePourEviterLesChargesFictives)
{
    /** can be done without this --- keep it for now**/
    NumeroDeContraintePourEviterLesChargesFictives[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    /******/

    // TODO improve this
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.FictiveLoads(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);
    exportPaliers(*problemeHebdo, builder, pays);
    auto coeff = problemeHebdo->DefaillanceNegativeUtiliserHydro[pays] ? -1 : 0;
    builder.include(Variable::HydProd(pays), coeff)
      .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

    builder.lessThan();
    builder.build();
}
