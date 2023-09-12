#include "MaxHydroPower.h"

void MaxHydroPower::add(int pays, MaxHydroPowerData& data)
{
    bool presenceHydro = data.presenceHydro;
    bool TurbEntreBornes = data.TurbEntreBornes;
    if (presenceHydro && (TurbEntreBornes || data.PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::HydProd(pays), 1.0);
        }
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.MaxHydroPower(builder.data.nombreDeContraintes);

        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
}