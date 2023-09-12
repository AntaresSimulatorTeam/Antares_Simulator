#include "AreaBalance.h"

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder,
                                    std::vector<int>& InjectionVariable,
                                    std::vector<int>& WithdrawalVariable)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        if (const int varInjection = InjectionVariable[index]; varInjection >= 0)
        {
            constraintBuilder.include(Variable::ShortTermStorageInjection(index), 1.0);
        }
        if (const int varWithdrawal = WithdrawalVariable[index]; varWithdrawal >= 0)
        {
            constraintBuilder.include(Variable::ShortTermStorageWithdrawal(index), -1.0);
        }
    }
}

// Constraint definitions
void AreaBalance::add(int pdt, int pays, AreaBalanceData& data)
{
    /** can be done without this --- keep it for now**/
    data.NumeroDeContrainteDesBilansPays[pays] = builder.data.nombreDeContraintes;

    /******/
    // TODO improve this
    {
        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.AreaBalance(builder.data.nombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);

    int interco = data.IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder.include(Variable::NTCDirect(interco), 1.0);
        interco = data.IndexSuivantIntercoOrigine[interco];
    }

    interco = data.IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.include(Variable::NTCDirect(interco), -1.0);
        interco = data.IndexSuivantIntercoExtremite[interco];
    }

    exportPaliers(data.PaliersThermiquesDuPays, builder);
    builder.include(Variable::HydProd(pays), -1.0)
      .include(Variable::Pumping(pays), 1.0)
      .include(Variable::PositiveUnsuppliedEnergy(pays), -1.0)
      .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

    shortTermStorageBalance(
      data.ShortTermStorage[pays], builder, data.InjectionVariable, data.WithdrawalVariable);

    builder.equalTo();
    builder.build();
}
