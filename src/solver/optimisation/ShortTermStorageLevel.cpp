#include "ShortTermStorageLevel.h"

void ShortTermStorageLevel::add(int pdt,
                                int pays,
                                std::vector<int>& ShortTermStorageLevelConstraint)
{
    // TODO improve this
    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);
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
        ShortTermStorageLevelConstraint[index]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        builder.include(Variable::ShortTermStorageLevel(index), 1.0)
          .include(Variable::ShortTermStorageLevel(index),
                   -1.0,
                   -1,
                   true,
                   problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
          .include(Variable::ShortTermStorageInjection(index), -1.0 * storage.efficiency)
          .include(Variable::ShortTermStorageWithdrawal(index), 1.0)
          .equalTo()
          .build();
    }
}
