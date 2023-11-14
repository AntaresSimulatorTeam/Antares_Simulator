#include "HydroPower.h"

void HydroPower::add(int pays)
{
    const int NombreDePasDeTempsPourUneOptimisation
      = builder.data.NombreDePasDeTempsPourUneOptimisation;
    auto caracteristiquesHydrauliques = data.CaracteristiquesHydrauliques[pays];
    if (caracteristiquesHydrauliques.PresenceDHydrauliqueModulable
        && !caracteristiquesHydrauliques.TurbinageEntreBornes)
    {
        if (caracteristiquesHydrauliques.PresenceDePompageModulable)
        {
            data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

            const double pumpingRatio = caracteristiquesHydrauliques.PumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt).HydProd(pays, 1.0).Pumping(pays, -pumpingRatio);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt).HydProd(pays, 1.0);
            }
        }
        data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        builder.equalTo();
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.HydroPower(builder.data.nombreDeContraintes);
        builder.build();
    }
    else
        data.NumeroDeContrainteEnergieHydraulique[pays] = -1;
}
