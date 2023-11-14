#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays]
      = builder.data.nombreDeContraintes;
    if (data.CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
    {
        builder.updateHourWithinWeek(pdt).HydroLevel(pays, 1.0);
        if (pdt > 0)
        {
            builder.updateHourWithinWeek(pdt - 1).HydroLevel(pays, -1.0);
        }
        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(builder.data.nombreDeContraintes);
        data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays]
          = builder.data.nombreDeContraintes;
        builder.updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .Pumping(pays, -data.CaracteristiquesHydrauliques[pays].PumpingRatio)
          .Overflow(pays, 1.)
          .equalTo()
          .build();
    }
    else
        data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays] = -1;
}