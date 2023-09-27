#include "ShortTermStorageLevel.h"
#include "opt_rename_problem.h"

void ShortTermStorageLevel::add(int pdt, int pays, std::shared_ptr<ShortTermStorageLevelData> data)
{
    ConstraintNamer namer(builder->data->NomDesContraintes);
    const int hourInTheYear = builder->data->weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);

    builder->updateHourWithinWeek(pdt);
    for (const auto& storage : data->ShortTermStorage[pays])
    {
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        namer.ShortTermStorageLevel(builder->data->nombreDeContraintes, storage.name);
        const auto index = storage.clusterGlobalIndex;
        data->ShortTermStorageLevelConstraint[index] = builder->data->nombreDeContraintes;

        builder->ShortTermStorageLevel(index, 1.0)
          .ShortTermStorageLevel(
            index, -1.0, -1, builder->data->NombreDePasDeTempsPourUneOptimisation)
          .ShortTermStorageInjection(index, -1.0 * storage.efficiency)
          .ShortTermStorageWithdrawal(index, 1.0)
          .equalTo()
          .build();
    }
}
