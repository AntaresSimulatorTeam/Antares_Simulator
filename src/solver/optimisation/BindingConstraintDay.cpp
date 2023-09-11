#include "BindingConstraintDay.h"

void BindingConstraintDay::add(int cntCouplante, BindingConstraintDayData& data)
{
    if (data.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
        return;

    const int nbInterco = data.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = data.NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation; // TODO
    const int NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int pdtDebut = 0;
    while (pdtDebut < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
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
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [data.NumeroDuPalierDispatch[index]];
            double poids = data.PoidsDuPalierDispatch[index];
            int offset = data.OffsetTemporelSurLePalierDispatch[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
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
        }

        CorrespondanceCntNativesCntOptimJournalieres
          .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
          = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

        char op = data.SensDeLaContrainteCouplante;
        builder.operatorRHS(op);
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(jour);
            namer.BindingConstraintDay(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                       data.NomDeLaContrainteCouplante);
        }
        builder.build();
        pdtDebut += problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
}
