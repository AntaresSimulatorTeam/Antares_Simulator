#include "BindingConstraintDay.h"

void BindingConstraintDay::add(int cntCouplante, std::shared_ptr<BindingConstraintDayData> data)
{
    if (data->TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
        return;

    const int nbInterco = data->NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = data->NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = builder->data->NombreDePasDeTempsPourUneOptimisation;
    const int NombreDePasDeTempsDUneJournee = data->NombreDePasDeTempsDUneJournee;
    int pdtDebut = 0;
    while (pdtDebut < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = data->NumeroDeJourDuPasDeTemps[pdtDebut];

        for (int index = 0; index < nbInterco; index++)
        {
            int interco = data->NumeroDeLInterconnexion[index];
            double poids = data->PoidsDeLInterconnexion[index];
            int offset = data->OffsetTemporelSurLInterco[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                builder->updateHourWithinWeek(pdt).NTCDirect(
                  interco, poids, offset, builder->data->NombreDePasDeTemps);
            }
        }

        for (int index = 0; index < nbClusters; index++)
        {
            int pays = data->PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data->PaliersThermiquesDuPays[pays];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [data->NumeroDuPalierDispatch[index]];
            double poids = data->PoidsDuPalierDispatch[index];
            int offset = data->OffsetTemporelSurLePalierDispatch[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                builder->updateHourWithinWeek(pdt).DispatchableProduction(
                  palier, poids, offset, builder->data->NombreDePasDeTemps);
            }
        }

        data->CorrespondanceCntNativesCntOptimJournalieres[jour]
          .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
          = builder->data->nombreDeContraintes;

        char op = data->SensDeLaContrainteCouplante;
        builder->SetOperator(op);
        {
            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateTimeStep(jour);
            namer.BindingConstraintDay(builder->data->nombreDeContraintes,
                                       data->NomDeLaContrainteCouplante);
        }
        builder->build();
        pdtDebut += data->NombreDePasDeTempsDUneJournee;
    }
}
