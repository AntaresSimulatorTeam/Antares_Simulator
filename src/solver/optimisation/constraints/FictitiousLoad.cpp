
#include "FictitiousLoad.h"

void FictitiousLoad::add(int pdt, int pays)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContraintePourEviterLesChargesFictives[pays]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);

    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.FictiveLoads(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt);
    ExportPaliers(data.PaliersThermiquesDuPays[pays], builder);
    auto coeff = data.DefaillanceNegativeUtiliserHydro[pays] ? -1 : 0;
    builder.HydProd(pays, coeff).NegativeUnsuppliedEnergy(pays, 1.0);

    builder.lessThan();
    builder.build();
}
