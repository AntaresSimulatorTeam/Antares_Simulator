#include "HydroPower.h"

void HydroPower::add(int pays, HydroPowerData& data)
{
    bool presenceHydro = data.presenceHydro;
    bool TurbEntreBornes = data.TurbEntreBornes;

    const int NombreDePasDeTempsPourUneOptimisation
      = builder.data.NombreDePasDeTempsPourUneOptimisation;
    if (presenceHydro && !TurbEntreBornes)
    {
        if (data.presencePompage)
        {
            data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

            const double pumpingRatio = data.pumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0)
                  .include(Variable::Pumping(pays), -pumpingRatio);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0);
            }
        }
        data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        builder.equalTo();
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.UpdateTimeStep(builder.data.weekInTheYear);
            namer.HydroPower(builder.data.nombreDeContraintes);
        }
        builder.build();
    }
    else
        data.NumeroDeContrainteEnergieHydraulique[pays] = -1;
}
