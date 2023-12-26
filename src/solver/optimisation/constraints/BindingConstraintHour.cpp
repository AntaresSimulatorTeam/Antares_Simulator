#include "BindingConstraintHour.h"

void BindingConstraintHour::add(int pdt, int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[cntCouplante];
    // Are we dealing with an hourly binding constraint ?
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
        return;

    // If so, is it enabled at this hour ?
    if (!MatriceDesContraintesCouplantes.enabledAtHour[pdt])
    {
        // By convention, any value that is < 0 represents a non-existing constraint
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesCouplantes[cntCouplante] = -1;
        return;
    }

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = builder.data.nombreDeContraintes;

    builder.updateHourWithinWeek(pdt);
    // Links
    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
    {
        const int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        const double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
        builder.updateHourWithinWeek(pdt).NTCDirect(
          interco, poids, offset, builder.data.NombreDePasDeTemps);
    }

    // Thermal clusters
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbClusters; index++)
    {
        const int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        const double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

        builder.updateHourWithinWeek(pdt).DispatchableProduction(
          palier, poids, offset, builder.data.NombreDePasDeTemps);
    }

    builder.SetOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
    {
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.BindingConstraintHour(builder.data.nombreDeContraintes,
                                    MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}
