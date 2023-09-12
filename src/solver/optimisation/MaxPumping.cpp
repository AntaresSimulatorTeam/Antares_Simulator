#include "MaxPumping.h"

void MaxPumping::add(int pays, MaxPumpingData& data)
{
    if (data.PresenceDePompageModulable)
    {
        data.NumeroDeContrainteMaxPompage[pays] = builder.data.nombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::Pumping(pays), 1.0);
        }
        data.NumeroDeContrainteMaxPompage[pays] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.MaxPumping(builder.data.nombreDeContraintes);
        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxPompage[pays] = -1;
}