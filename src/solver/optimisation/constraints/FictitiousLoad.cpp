
#include "FictitiousLoad.h"

void FictitiousLoad::add(int pdt, int pays)
{
    /** can be done without this --- keep it for now**/
    CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContraintePourEviterLesChargesFictives[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    /******/

    // TODO improve this
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.FictiveLoads(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);
    exportPaliers(*problemeHebdo, builder, pays);
    auto coeff = problemeHebdo->DefaillanceNegativeUtiliserHydro[pays] ? -1 : 0;
    builder.HydProd(pays, coeff).NegativeUnsuppliedEnergy(pays, 1.0);

    builder.lessThan();
    builder.build();
}
