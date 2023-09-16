#include "FlowDissociation.h"
#include "opt_rename_problem.h"

void FlowDissociation::add(int pdt, int interco, std::shared_ptr<FlowDissociationData> data)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = data->CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        // TODO improve this
        data->NumeroDeContrainteDeDissociationDeFlux[interco] = builder->data->nombreDeContraintes;
        {
            const auto origin
              = builder->data->NomsDesPays[data->PaysOrigineDeLInterconnexion[interco]];
            const auto destination
              = builder->data->NomsDesPays[data->PaysExtremiteDeLInterconnexion[interco]];
            ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);
            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
            namer.FlowDissociation(builder->data->nombreDeContraintes, origin, destination);
        }

        builder->updateHourWithinWeek(pdt);
        builder->include(NewVariable::NTCDirect(interco), 1.0)
          .include(NewVariable::IntercoDirectCost(interco), -1.0)
          .include(NewVariable::IntercoIndirectCost(interco), 1.0);

        builder->equalTo();

        builder->build();
    }
}
