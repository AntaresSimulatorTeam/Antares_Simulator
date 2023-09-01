#include "AreaBalance.h"

static void shortTermStorageBalance(
  const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
  ConstraintBuilder& constraintBuilder,
  const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        if (const int varInjection
            = CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage.InjectionVariable[index];
            varInjection >= 0)
        {
            constraintBuilder.include(Variable::ShortTermStorageInjection(varInjection), 1.0)
        }
        if (const int varWithdrawal
            = CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage.WithdrawalVariable[index];
            varWithdrawal >= 0)
        {
            constraintBuilder.include(Variable::ShortTermStorageWithdrawal(varWithdrawal), -1.0);
        }
    }
}

// Constraint definitions
void AreaBalance::add(int pdt, int pays)
{
    /** can be done without this --- keep it for now**/
    CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesBilansPays[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

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
      problemeHebdo->ShortTermStorage[pays], builder, CorrespondanceVarNativesVarOptim);

    // {
    //     const CONSOMMATIONS_ABATTUES& ConsommationsAbattues
    //       = problemeHebdo->ConsommationsAbattues[pdtHebdo];
    //     double rhs = -ConsommationsAbattues.ConsommationAbattueDuPays[pays];
    //     bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1);
    //     bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);
    //     if (reserveJm1 && opt1)
    //     {
    //         rhs -= problemeHebdo->ReserveJMoins1[pays].ReserveHoraireJMoins1[pdtHebdo];
    //     }
    //     /* check !*/
    //     double* adresseDuResultat
    //       = &(problemeHebdo->ResultatsHoraires[pays].CoutsMarginauxHoraires[pdtHebdo]);

    //     std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
    //       = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    //     int cnt = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    //     AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = adresseDuResultat;

    // }
    /*check! */
    builder.equalTo();
    builder.build();
}
