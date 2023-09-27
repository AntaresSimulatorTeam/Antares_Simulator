#include "AreaBalance.h"

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    std::shared_ptr<NewConstraintBuilder> constraintBuilder)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        constraintBuilder->ShortTermStorageInjection(index, 1.0)
          .ShortTermStorageWithdrawal(index, -1.0);
    }
}

void AreaBalance::add(int pdt, int pays, std::shared_ptr<AreaBalanceData> data)
{
    data->NumeroDeContrainteDesBilansPays[pays] = builder->data->nombreDeContraintes;

    ConstraintNamer namer(builder->data->NomDesContraintes);
    namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);
    namer.AreaBalance(builder->data->nombreDeContraintes);

    builder->updateHourWithinWeek(pdt);

    int interco = data->IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder->NTCDirect(interco, 1.0);
        interco = data->IndexSuivantIntercoOrigine[interco];
    }

    interco = data->IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder->NTCDirect(interco, -1.0);
        interco = data->IndexSuivantIntercoExtremite[interco];
    }

    new_exportPaliers(data->PaliersThermiquesDuPays, builder);
    builder->HydProd(pays, -1.0)
      .Pumping(pays, 1.0)
      .PositiveUnsuppliedEnergy(pays, -1.0)
      .NegativeUnsuppliedEnergy(pays, 1.0);

    shortTermStorageBalance(data->ShortTermStorage[pays], builder);

    builder->equalTo();
    builder->build();
}
