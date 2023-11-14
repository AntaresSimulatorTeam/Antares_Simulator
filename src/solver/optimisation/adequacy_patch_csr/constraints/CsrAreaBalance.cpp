#include "CsrAreaBalance.h"

void CsrAreaBalance::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            continue;

        // + ENS
        builder.updateHourWithinWeek(data.hour).PositiveUnsuppliedEnergy(Area, 1.0);

        // - export flows
        int Interco = data.IndexDebutIntercoOrigine[Area];
        while (Interco >= 0)
        {
            if (data.extremityAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoOrigine[Interco];
                continue;
            }

            // flow (A->2)
            builder.NTCDirect(Interco, -1.0);

            logs.debug() << "S-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoOrigine[Interco];
        }

        // or + import flows
        Interco = data.IndexDebutIntercoExtremite[Area];
        while (Interco >= 0)
        {
            if (data.originAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoExtremite[Interco];
                continue;
            }
            // flow (2 -> A)
            builder.NTCDirect(Interco, 1.0);

            logs.debug() << "E-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoExtremite[Interco];
        }

        // - Spilled Energy
        builder.NegativeUnsuppliedEnergy(Area, -1.0);

        data.numberOfConstraintCsrAreaBalance[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrAreaBalance(builder.data.nombreDeContraintes);
        builder.equalTo();
        builder.build();
    }
}