#include "FlowDissociation.h"

void FlowDissociation::add(int pdt,
                           int interco,
                           std::vector<int>& NumeroDeContrainteDeDissociationDeFlux)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        // TODO improve this
        NumeroDeContrainteDeDissociationDeFlux[interco]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
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

        builder.equalTo();

        builder.build();
    }
}
