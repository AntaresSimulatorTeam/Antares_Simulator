#include "ShortTermStorageLevel.h"

void ShortTermStorageLevel::add(int pdt, int pays)
{
    // TODO improve this
    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
    /** can be done without this --- keep it for now**/
    CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

    /******/

    const int hourInTheYear = problemeHebdo->weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

    builder.updateHourWithinWeek(pdt);
    for (const auto& storage : problemeHebdo->ShortTermStorage[pays])
    {
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        namer.ShortTermStorageLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                    storage.name);
        const auto index = storage.clusterGlobalIndex;
        CorrespondanceCntNativesCntOptim.ShortTermStorageLevelConstraint[index]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        builder.ShortTermStorageLevel(index, 1.0)
          .ShortTermStorageLevel(
            index, -1.0, -1, problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
          .ShortTermStorageInjection(index, -1.0 * storage.efficiency)
          .ShortTermStorageWithdrawal(index, 1.0)
          .equalTo()
          .build();
    }
}
