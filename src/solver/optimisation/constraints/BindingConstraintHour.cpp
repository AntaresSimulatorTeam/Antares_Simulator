#include "BindingConstraintHour.h"

void BindingConstraintHour::add(int pdt, int cntCouplante)
{
    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = builder.data.nombreDeContraintes;

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[cntCouplante];

    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
        return;

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
