#include "CsrFlowDissociation.h"

void CsrFlowDissociation::add()
{
    builder.updateHourWithinWeek(data.hour);

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateTimeStep(data.hour);
    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes
    // of type 2.
    for (uint32_t interco = 0; interco < data.NombreDInterconnexions; interco++)
    {
        if (data.originAreaMode[interco] == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && data.extremityAreaMode[interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            builder.NTCDirect(interco, 1.0)
              .IntercoDirectCost(interco, -1.0)
              .IntercoIndirectCost(interco, 1.0);

            data.numberOfConstraintCsrFlowDissociation[interco]
              = builder.data.nombreDeContraintes;

            const auto& origin
              = builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[interco]];
            const auto& destination
              = builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[interco]];
            namer.CsrFlowDissociation(builder.data.nombreDeContraintes, origin, destination);
            builder.equalTo();
            builder.build();
        }
    }
}