#include "MinHydroPower.h"

void MinHydroPower::add(int pays)
{
    bool presenceHydro
      = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
    bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
    if (presenceHydro
        && (TurbEntreBornes
            || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
    {
        problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MinHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::HydProd(pays), 1.0);
        }
        // const double rhs = problemeHebdo->CaracteristiquesHydrauliques[pays]
        //                      .MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
        builder.greaterThan().build();
    }
}
