#include "FlowDissociation.h"

void FlowDissociation::add(int pdt, int interco, FlowDissociationData& data)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = data.CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        // TODO improve this
        data.NumeroDeContrainteDeDissociationDeFlux[interco] = builder.data.nombreDeContraintes;
        {
            const auto origin
              = builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[interco]];
            const auto destination
              = builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[interco]];
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.FlowDissociation(builder.data.nombreDeContraintes, origin, destination);
        }

        builder.updateHourWithinWeek(pdt);
        builder.include(Variable::NTCDirect(interco), 1.0)
          .include(Variable::IntercoDirectCost(interco), -1.0)
          .include(Variable::IntercoIndirectCost(interco), 1.0);

        builder.equalTo();

        builder.build();
    }
}
