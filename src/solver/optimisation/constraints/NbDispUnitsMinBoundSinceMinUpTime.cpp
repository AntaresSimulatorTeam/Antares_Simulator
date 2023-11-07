#include "NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(
  int pays,
  std::shared_ptr<NbDispUnitsMinBoundSinceMinUpTimeData> data)
{
    // const PALIERS_THERMIQUES& PaliersThermiquesDuPays
    //   = problemeHebdo->PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = data->PaliersThermiquesDuPays
          .DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[data->clusterIndex];

    // CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
    //   = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    data->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[data->cluster] = -1;
    if (!data->Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder->data->NombreDePasDeTempsPourUneOptimisation;

        builder->updateHourWithinWeek(data->pdt).NumberOfDispatchableUnits(data->cluster, 1.0);

        for (int k = data->pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1;
             k <= data->pdt;
             k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder->updateHourWithinWeek(t1)
              .NumberStartingDispatchableUnits(data->cluster, -1.0)
              .NumberBreakingDownDispatchableUnits(data->cluster, 1.0);
        }

        builder->greaterThan();
        if (builder->NumberOfVariables() > 1)
        {
            data->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[data->cluster]
              = builder->data->nombreDeContraintes;

            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);

            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              builder->data->nombreDeContraintes,
              data->PaliersThermiquesDuPays.NomsDesPaliersThermiques[data->clusterIndex]);
            builder->build();
        }
    }
    else
    {
        *builder->data->NbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + 2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
        builder->data->nombreDeContraintes++;
    }
}
