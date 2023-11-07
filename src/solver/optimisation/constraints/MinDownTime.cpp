#include "MinDownTime.h"

void MinDownTime::add(int pays, std::shared_ptr<MinDownTimeData> data)
{
    // const PALIERS_THERMIQUES& PaliersThermiquesDuPays
    //   = problemeHebdo->PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = data->PaliersThermiquesDuPays
          .DureeMinimaleDArretDUnGroupeDuPalierThermique[data->clusterIndex];

    // CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
    //   = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    data->NumeroDeContrainteDesContraintesDeDureeMinDArret[data->cluster] = -1;
    if (!data->Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder->data->NombreDePasDeTempsPourUneOptimisation;

        builder->updateHourWithinWeek(data->pdt).NumberOfDispatchableUnits(data->cluster, 1.0);

        for (int k = data->pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= data->pdt;
             k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder->updateHourWithinWeek(t1).NumberStoppingDispatchableUnits(data->cluster, 1.0);
        }
        builder->lessThan();
        if (builder->NumberOfVariables() > 1)
        {
            data->NumeroDeContrainteDesContraintesDeDureeMinDArret[data->cluster]
              = builder->data->nombreDeContraintes;
            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);

            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.MinDownTime(
              builder->data->nombreDeContraintes,
              data->PaliersThermiquesDuPays.NomsDesPaliersThermiques[data->clusterIndex]);

            builder->build();
        }
    }
    else
    {
        *builder->data->NbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique;
        builder->data->nombreDeContraintes++;
    }
}
