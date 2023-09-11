#include "BindingConstraintHour.h"

void BindingConstraintHour::add(int pdt, int cntCouplante, BindingConstraintHourData& data)
{
    data.NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    if (data.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
        return;

    builder.updateHourWithinWeek(pdt);
    // Links
    const int nbInterco = data.NombreDInterconnexionsDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
    {
        const int interco = data.NumeroDeLInterconnexion[index];
        const double poids = data.PoidsDeLInterconnexion[index];
        const int offset = data.OffsetTemporelSurLInterco[index];
        int pdt1;
        if (offset >= 0)
        {
            pdt1 = (pdt + offset) % problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        }
        else
        {
            pdt1 = (pdt + offset + problemeHebdo->NombreDePasDeTemps)
                   % problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        }

        builder.updateHourWithinWeek(pdt1).include(
          Variable::NTCDirect(interco), poids, 0, false, problemeHebdo->NombreDePasDeTemps);
    }

    // Thermal clusters
    const int nbClusters = data.NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbClusters; index++)
    {
        const int pays = data.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int palier
          = PaliersThermiquesDuPays
              .NumeroDuPalierDansLEnsembleDesPaliersThermiques[data.NumeroDuPalierDispatch[index]];
        const double poids = data.PoidsDuPalierDispatch[index];
        const int offset = data.OffsetTemporelSurLePalierDispatch[index];
        int pdt1;

        if (offset >= 0)
        {
            pdt1 = (pdt + offset) % problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        }
        else
        {
            pdt1 = (pdt + offset + problemeHebdo->NombreDePasDeTemps)
                   % problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        }

        builder.updateHourWithinWeek(pdt1).include(Variable::DispatchableProduction(palier),
                                                   poids,
                                                   0,
                                                   false,
                                                   problemeHebdo->NombreDePasDeTemps);
    }

    char op = data.SensDeLaContrainteCouplante;
    builder.operatorRHS(op);
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.BindingConstraintHour(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                    data.NomDeLaContrainteCouplante);
    }
    builder.build();
}
