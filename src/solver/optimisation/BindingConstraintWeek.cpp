#include "BindingConstraintWeek.h"

void BindingConstraintWeek::add(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
        return;

    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        builder.updateHourWithinWeek(pdt);
        for (int index = 0; index < nbInterco; index++)
        {
            int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
            double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

            builder.include(Variable::NTCDirect(interco), poids, offset, true);
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
            builder.include(Variable::DispatchableProduction(palier), poids, offset, true);
        }
    }
    char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
    builder.operatorRHS(op);

    // Name
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.BindingConstraintWeek(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                    MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}