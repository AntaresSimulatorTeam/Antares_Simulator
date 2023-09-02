#include "FlowDissociation.h"

void FlowDissociation::add(int pdt, int interco)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        /** can be done without this --- keep it for now**/
        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = *problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
        CorrespondanceCntNativesCntOptim.NumeroDeContrainteDeDissociationDeFlux[interco]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        /******/

        // TODO improve this

        {
            const auto origin
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
            const auto destination
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.FlowDissociation(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes, origin, destination);
        }

        builder.updateHourWithinWeek(pdt);
        builder.include(Variable::NTCDirect(interco), 1.0)
          .include(Variable::IntercoDirectCost(interco), -1.0)
          .include(Variable::IntercoIndirectCost(interco), 1.0);

        // if (CoutDeTransport.IntercoGereeAvecLoopFlow)
        //     builder.equalTo(
        //       problemeHebdo->ValeursDeNTC[pdtHebdo].ValeurDeLoopFlowOrigineVersExtremite[interco]);
        // else
        //     builder.equalTo(0.);
        builder.equalTo();

        builder.build();
    }
}
