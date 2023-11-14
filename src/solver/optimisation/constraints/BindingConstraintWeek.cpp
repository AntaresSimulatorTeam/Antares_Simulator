#include "BindingConstraintWeek.h"

void BindingConstraintWeek::add(int cntCouplante)
{
    int semaine = builder.data.weekInTheYear;

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
        return;

    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

    for (int index = 0; index < nbInterco; index++)
    {
        int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
        for (int pdt = 0; pdt < builder.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt).NTCDirect(
              interco, poids, offset, builder.data.NombreDePasDeTemps);
        }
    }

    for (int index = 0; index < nbClusters; index++)
    {
        int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
        for (int pdt = 0; pdt < builder.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt).DispatchableProduction(
              palier, poids, offset, builder.data.NombreDePasDeTemps);
        }
    }

    builder.SetOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);

    data.NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateTimeStep(semaine);
    namer.BindingConstraintWeek(builder.data.nombreDeContraintes,
                                MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    builder.build();
}