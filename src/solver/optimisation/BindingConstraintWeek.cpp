#include "BindingConstraintWeek.h"

void BindingConstraintWeek::add(int cntCouplante, BindingConstraintWeekData& data)
{
    int semaine = problemeHebdo->weekInTheYear;
    auto& NumeroDeContrainteDesContraintesCouplantes
      = data.NumeroDeContrainteDesContraintesCouplantes;
    if (data.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
        return;

    const int nbInterco = data.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = data.NombreDePaliersDispatchDansLaContrainteCouplante;

    for (int index = 0; index < nbInterco; index++)
    {
        int interco = data.NumeroDeLInterconnexion[index];
        double poids = data.PoidsDeLInterconnexion[index];
        int offset = data.OffsetTemporelSurLInterco[index];
        for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
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
    }

        for (int index = 0; index < nbClusters; index++)
        {
        int pays = data.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int palier
          = PaliersThermiquesDuPays
              .NumeroDuPalierDansLEnsembleDesPaliersThermiques[data.NumeroDuPalierDispatch[index]];
        double poids = data.PoidsDuPalierDispatch[index];
        int offset = data.OffsetTemporelSurLePalierDispatch[index];
        for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            int pdt1;

            builder.updateHourWithinWeek(pdt); // useless?
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
                                                       offset,
                                                       false,
                                                       problemeHebdo->NombreDePasDeTemps);
        }
        }

        char op = data.SensDeLaContrainteCouplante;
        builder.operatorRHS(op);

        NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        // Name
        {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateTimeStep(semaine);
        namer.BindingConstraintWeek(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                    data.NomDeLaContrainteCouplante);
        }
    builder.build();
}