#include "ShortTermStorageLevel.h"

void ShortTermStorageLevel::add(int pdt, int pays, ShortTermStorageLevelData& data)
{
    // TODO improve this
    ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    builder.updateHourWithinWeek(pdt);
    for (const auto& storage : data.ShortTermStorage[pays])
    {
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        namer.ShortTermStorageLevel(builder.data.nombreDeContraintes, storage.name);
        const auto index = storage.clusterGlobalIndex;
        data.ShortTermStorageLevelConstraint[index] = builder.data.nombreDeContraintes;

        builder.include(Variable::ShortTermStorageLevel(index), 1.0)
          .include(Variable::ShortTermStorageLevel(index),
                   -1.0,
                   -1,
                   true,
                   builder.data.NombreDePasDeTempsPourUneOptimisation)
          .include(Variable::ShortTermStorageInjection(index), -1.0 * storage.efficiency)
          .include(Variable::ShortTermStorageWithdrawal(index), 1.0)
          .equalTo()
          .build();
    }
}
