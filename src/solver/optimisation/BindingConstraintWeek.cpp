#include "BindingConstraintWeek.h"

void BindingConstraintWeek::add(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
    int semaine = problemeHebdo->weekInTheYear;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
      = problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires;
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
        return;

    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    for (int index = 0; index < nbInterco; index++)
    {
        int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
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
            int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
            double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
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

    char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
    builder.operatorRHS(op);

    CorrespondanceCntNativesCntOptimHebdomadaires
      .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
    // Name
    {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(semaine);
            namer.BindingConstraintWeek(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                        MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}