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
void AreaBalance::add(int pdt,
                      int pays,
                      std::vector<int>& NumeroDeContrainteDesBilansPays,
                      std::vector<int>& InjectionVariable,
                      std::vector<int>& WithdrawalVariable)
{
    /** can be done without this --- keep it for now**/
    NumeroDeContrainteDesBilansPays[pays] = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    /******/
    // TODO improve this
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.AreaBalance(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);

    int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder.include(Variable::NTCDirect(interco), 1.0);
        interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
    }

    interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.include(Variable::NTCDirect(interco), -1.0);
        interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
    }

    exportPaliers(*problemeHebdo, builder, pays);
    builder.include(Variable::HydProd(pays), -1.0)
      .include(Variable::Pumping(pays), 1.0)
      .include(Variable::PositiveUnsuppliedEnergy(pays), -1.0)
      .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

    shortTermStorageBalance(
      problemeHebdo->ShortTermStorage[pays], builder, InjectionVariable, WithdrawalVariable);

    builder.equalTo();
    builder.build();
}
