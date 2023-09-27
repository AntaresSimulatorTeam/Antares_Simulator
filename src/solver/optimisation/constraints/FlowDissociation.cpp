#include "FlowDissociation.h"

void FlowDissociation::add(int pdt, int interco, std::shared_ptr<FlowDissociationData> data)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = data->CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        data->NumeroDeContrainteDeDissociationDeFlux[interco] = builder->data->nombreDeContraintes;
        const auto origin = builder->data->NomsDesPays[data->PaysOrigineDeLInterconnexion[interco]];
        const auto destination
          = builder->data->NomsDesPays[data->PaysExtremiteDeLInterconnexion[interco]];
        ConstraintNamer namer(builder->data->NomDesContraintes);
        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.FlowDissociation(builder->data->nombreDeContraintes, origin, destination);

        builder->updateHourWithinWeek(pdt);
        builder->NTCDirect(interco, 1.0)
          .IntercoDirectCost(interco, -1.0)
          .IntercoIndirectCost(interco, 1.0);

        builder->equalTo();

        builder->build();
    }
}
