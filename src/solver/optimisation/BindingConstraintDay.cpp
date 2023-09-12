#include "BindingConstraintDay.h"

void BindingConstraintDay::add(int cntCouplante, BindingConstraintDayData& data)
{
    if (data.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
        return;

    const int nbInterco = data.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = data.NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = builder.data.NombreDePasDeTempsPourUneOptimisation; // TODO
    const int NombreDePasDeTempsDUneJournee = data.NombreDePasDeTempsDUneJournee;
    int pdtDebut = 0;
    while (pdtDebut < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = data.NumeroDeJourDuPasDeTemps[pdtDebut];
        auto& CorrespondanceCntNativesCntOptimJournalieres
          = data.CorrespondanceCntNativesCntOptimJournalieres[jour];

        for (int index = 0; index < nbInterco; index++)
        {
            int interco = data.NumeroDeLInterconnexion[index];
            double poids = data.PoidsDeLInterconnexion[index];
            int offset = data.OffsetTemporelSurLInterco[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                int pdt1;
                if (offset >= 0)
                {
                    pdt1 = (pdt + offset) % builder.data.NombreDePasDeTempsPourUneOptimisation;
                }
                else
                {
                    pdt1 = (pdt + offset + builder.data.NombreDePasDeTemps)
                           % builder.data.NombreDePasDeTempsPourUneOptimisation;
                }
                builder.updateHourWithinWeek(pdt1).include(
                  Variable::NTCDirect(interco), poids, 0, false, builder.data.NombreDePasDeTemps);
            }
        }

        for (int index = 0; index < nbClusters; index++)
        {
            int pays = data.PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [data.NumeroDuPalierDispatch[index]];
            double poids = data.PoidsDuPalierDispatch[index];
            int offset = data.OffsetTemporelSurLePalierDispatch[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                int pdt1;
                if (offset >= 0)
                {
                    pdt1 = (pdt + offset) % builder.data.NombreDePasDeTempsPourUneOptimisation;
                }
                else
                {
                    pdt1 = (pdt + offset + builder.data.NombreDePasDeTemps)
                           % builder.data.NombreDePasDeTempsPourUneOptimisation;
                }

                builder.updateHourWithinWeek(pdt1).include(Variable::DispatchableProduction(palier),
                                                           poids,
                                                           0,
                                                           false,
                                                           builder.data.NombreDePasDeTemps);
            }
        }

        CorrespondanceCntNativesCntOptimJournalieres
          .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
          = builder.data.nombreDeContraintes;

        char op = data.SensDeLaContrainteCouplante;
        builder.operatorRHS(op);
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateTimeStep(jour);
            namer.BindingConstraintDay(builder.data.nombreDeContraintes,
                                       data.NomDeLaContrainteCouplante);
        }
        builder.build();
        pdtDebut += data.NombreDePasDeTempsDUneJournee;
    }
}
