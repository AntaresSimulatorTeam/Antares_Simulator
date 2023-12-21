#include "CsrBindingConstraintHour.h"

void CsrBindingConstraintHour::add(int CntCouplante)
{
    if (data.MatriceDesContraintesCouplantes[CntCouplante].TypeDeContrainteCouplante
        != CONTRAINTE_HORAIRE)
        return;

    int NbInterco = data.MatriceDesContraintesCouplantes[CntCouplante]
                      .NombreDInterconnexionsDansLaContrainteCouplante;
    builder.updateHourWithinWeek(data.hour);

    for (int Index = 0; Index < NbInterco; Index++)
    {
        int Interco
          = data.MatriceDesContraintesCouplantes[CntCouplante].NumeroDeLInterconnexion[Index];
        double Poids
          = data.MatriceDesContraintesCouplantes[CntCouplante].PoidsDeLInterconnexion[Index];

        if (data.originAreaMode[Interco] == Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && data.extremityAreaMode[Interco] == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            builder.NTCDirect(Interco, Poids);
        }
    }

    if (builder.NumberOfVariables()
        > 0) // current binding constraint contains an interco type 2<->2
    {
        data.numberOfConstraintCsrHourlyBinding[CntCouplante] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.CsrBindingConstraintHour(
          builder.data.nombreDeContraintes,
          data.MatriceDesContraintesCouplantes[CntCouplante].NomDeLaContrainteCouplante);
        builder.SetOperator(
          data.MatriceDesContraintesCouplantes[CntCouplante].SensDeLaContrainteCouplante);
        builder.build();
    }
}