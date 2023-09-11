#include "BindingConstraintDay.h"

void BindingConstraintDay::add(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
        return;

    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation; // TODO
    const int NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int pdtDebut = 0;
    while (pdtDebut < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
        CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES& CorrespondanceCntNativesCntOptimJournalieres
          = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[jour];

        for (int index = 0; index < nbInterco; index++)
        {
            int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
            double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

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
            int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
            double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

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

        char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
        builder.operatorRHS(op);
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(jour);
            namer.BindingConstraintDay(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                       MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
        }
        builder.build();
        pdtDebut += problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
}
