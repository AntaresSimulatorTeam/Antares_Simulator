#include "AreaBalance.h"

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        constraintBuilder.ShortTermStorageInjection(index, 1.0)
        .ShortTermStorageWithdrawal(index, -1.0);
    }
}

void AreaBalance::add(int pdt, int pays)
{
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesBilansPays[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.AreaBalance(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.updateHourWithinWeek(pdt);

        int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
        while (interco >= 0)
        {
        builder.NTCDirect(interco, 1.0);
        interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
        }

    interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.NTCDirect(interco, -1.0);
        interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
    }

    exportPaliers(*problemeHebdo, builder, pays);
    builder.HydProd(pays, -1.0)
      .Pumping(pays, 1.0)
      .PositiveUnsuppliedEnergy(pays, -1.0)
      .NegativeUnsuppliedEnergy(pays, 1.0);

    shortTermStorageBalance(problemeHebdo->ShortTermStorage[pays], builder);

    builder.equalTo();
    builder.build();
}
