#include "BindingConstraintHour.h"

void BindingConstraintHour::add(int pdt, int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
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
        builder.include(
          Variable::NTCDirect(interco), poids, offset, true, problemeHebdo->NombreDePasDeTemps);
    }

    // Thermal clusters
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbClusters; index++)
    {
        const int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        const double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
        builder.include(Variable::DispatchableProduction(palier),
                        poids,
                        offset,
                        true,
                        problemeHebdo->NombreDePasDeTemps);
    }

    // std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
    //   = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;
    // int cnt = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

    // double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[pdtHebdo];
    // AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt]
    //   = problemeHebdo->ResultatsContraintesCouplantes[cntCouplante].variablesDuales.data()
    //     + pdtHebdo;
    char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
    builder.operatorRHS(op);
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.BindingConstraintHour(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                    MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}
