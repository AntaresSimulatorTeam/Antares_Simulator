
#include "FictitiousLoad.h"

void FictitiousLoad::add(int pdt, int pays)
{
    /** can be done without this --- keep it for now**/
    CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContraintePourEviterLesChargesFictives[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    /******/

    // TODO improve this
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.FictiveLoads(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);
    exportPaliers(*problemeHebdo, builder, pays);
    builder.include(Variable::HydProd(pays), -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays])
      .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

    {
        // double rhs = 0;
        // // Private members ?
        // const ALL_MUST_RUN_GENERATION& AllMustRunGeneration
        //   = problemeHebdo->AllMustRunGeneration[pdtHebdo];
        // const CONSOMMATIONS_ABATTUES& ConsommationsAbattues
        //   = problemeHebdo->ConsommationsAbattues[pdtHebdo];
        // const std::vector<bool>& DefaillanceNegativeUtiliserConsoAbattue
        //   = problemeHebdo->DefaillanceNegativeUtiliserConsoAbattue;
        // const std::vector<bool>& DefaillanceNegativeUtiliserPMinThermique
        //   = problemeHebdo->DefaillanceNegativeUtiliserPMinThermique;

        // double MaxAllMustRunGeneration = 0.0;

        // if (AllMustRunGeneration.AllMustRunGenerationOfArea[pays] > 0.0)
        //     MaxAllMustRunGeneration = AllMustRunGeneration.AllMustRunGenerationOfArea[pays];

        // double MaxMoinsConsommationBrute = 0.0;
        // if (-(ConsommationsAbattues.ConsommationAbattueDuPays[pays]
        //       + AllMustRunGeneration.AllMustRunGenerationOfArea[pays])
        //     > 0.0)
        //     MaxMoinsConsommationBrute = -(ConsommationsAbattues.ConsommationAbattueDuPays[pays]
        //                                   +
        //                                   AllMustRunGeneration.AllMustRunGenerationOfArea[pays]);

        // rhs = DefaillanceNegativeUtiliserConsoAbattue[pays]
        //       * (MaxAllMustRunGeneration + MaxMoinsConsommationBrute);

        // if (DefaillanceNegativeUtiliserPMinThermique[pays] == 0)
        // {
        //     rhs -= OPT_SommeDesPminThermiques(problemeHebdo, pays, pdtHebdo);
        // }
        builder.lessThan();
    }
    builder.build();
}
