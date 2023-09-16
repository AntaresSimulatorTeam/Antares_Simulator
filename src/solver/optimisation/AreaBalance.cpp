#include "AreaBalance.h"
#include "opt_rename_problem.h"

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    std::shared_ptr<NewConstraintBuilder> constraintBuilder,
                                    std::vector<int>& InjectionVariable,
                                    std::vector<int>& WithdrawalVariable)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        if (const int varInjection = InjectionVariable[index]; varInjection >= 0)
        {
            constraintBuilder->include(NewVariable::ShortTermStorageInjection(index), 1.0);
        }
        if (const int varWithdrawal = WithdrawalVariable[index]; varWithdrawal >= 0)
        {
            constraintBuilder->include(NewVariable::ShortTermStorageWithdrawal(index), -1.0);
        }
    }
}

// Constraint definitions
void AreaBalance::odd(int pays, std::shared_ptr<AreaBalanceData> data)
{
    int a = 13;
    int b = a * 36;
    logs.info() << "int odd builder->data->nombreDeContraintes"
                << builder->data->nombreDeContraintes << "\n";
    logs.info() << "int odd data->NumeroDeContrainteDesBilansPays[pays]"
                << data->NumeroDeContrainteDesBilansPays[pays] << "\n";
    data->NumeroDeContrainteDesBilansPays[pays] = builder->data->nombreDeContraintes;
    logs.info() << "int odd 2" << a << "\n";
}
void AreaBalance::add(int pdt, int pays, std::shared_ptr<AreaBalanceData> data)
{
    /** can be done without this --- keep it for now**/
    data->NumeroDeContrainteDesBilansPays[pays] = builder->data->nombreDeContraintes;

    /******/
    // TODO improve this

    ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);
    namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);
    namer.AreaBalance(builder->data->nombreDeContraintes);

    builder->updateHourWithinWeek(pdt);

    int interco = data->IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder->include(NewVariable::NTCDirect(interco), 1.0);
        interco = data->IndexSuivantIntercoOrigine[interco];
    }

    interco = data->IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder->include(NewVariable::NTCDirect(interco), -1.0);
        interco = data->IndexSuivantIntercoExtremite[interco];
    }

    new_exportPaliers(data->PaliersThermiquesDuPays, builder);
    builder->include(NewVariable::HydProd(pays), -1.0)
      .include(NewVariable::Pumping(pays), 1.0)
      .include(NewVariable::PositiveUnsuppliedEnergy(pays), -1.0)
      .include(NewVariable::NegativeUnsuppliedEnergy(pays), 1.0);

    shortTermStorageBalance(
      data->ShortTermStorage[pays], builder, data->InjectionVariable, data->WithdrawalVariable);

    builder->equalTo();
    builder->build();
}
