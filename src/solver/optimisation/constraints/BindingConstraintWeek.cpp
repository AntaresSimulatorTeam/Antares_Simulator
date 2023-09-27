#include "BindingConstraintWeek.h"

void BindingConstraintWeek::add(int cntCouplante, std::shared_ptr<BindingConstraintWeekData> data)
{
    int semaine = builder->data->weekInTheYear;
    if (data->TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
        return;

    const int nbInterco = data->NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = data->NombreDePaliersDispatchDansLaContrainteCouplante;

    for (int index = 0; index < nbInterco; index++)
    {
        int interco = data->NumeroDeLInterconnexion[index];
        double poids = data->PoidsDeLInterconnexion[index];
        int offset = data->OffsetTemporelSurLInterco[index];
        for (int pdt = 0; pdt < builder->data->NombreDePasDeTempsPourUneOptimisation; pdt++)
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
          = PaliersThermiquesDuPays
              .NumeroDuPalierDansLEnsembleDesPaliersThermiques[data->NumeroDuPalierDispatch[index]];
        double poids = data->PoidsDuPalierDispatch[index];
        int offset = data->OffsetTemporelSurLePalierDispatch[index];
        for (int pdt = 0; pdt < builder->data->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder->updateHourWithinWeek(pdt).DispatchableProduction(
              palier, poids, offset, builder->data->NombreDePasDeTemps);
        }
    }

    char op = data->SensDeLaContrainteCouplante;
    builder->SetOperator(op);

    data->NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = builder->data->nombreDeContraintes;

    ConstraintNamer namer(builder->data->NomDesContraintes);
    namer.UpdateTimeStep(semaine);
    namer.BindingConstraintWeek(builder->data->nombreDeContraintes,
                                data->NomDeLaContrainteCouplante);
        builder->build();
}