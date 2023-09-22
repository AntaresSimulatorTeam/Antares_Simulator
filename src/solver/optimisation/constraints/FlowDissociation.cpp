#include "FlowDissociation.h"

void FlowDissociation::add(int pdt, int interco)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
        CorrespondanceCntNativesCntOptim.NumeroDeContrainteDeDissociationDeFlux[interco]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const auto origin
          = problemeHebdo->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
        const auto destination
          = problemeHebdo->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FlowDissociation(
          problemeHebdo->ProblemeAResoudre->NombreDeContraintes, origin, destination);

        builder.updateHourWithinWeek(pdt);
        builder.NTCDirect(interco, 1.0)
          .IntercoDirectCost(interco, -1.0)
          .IntercoIndirectCost(interco, 1.0);

        builder.equalTo();

        builder.build();
    }
}
