#include "MinHydroPower.h"

void MinHydroPower::add(int pays, MinHydroPowerData& data)
{
    bool presenceHydro = data.presenceHydro;
    bool TurbEntreBornes = data.TurbEntreBornes;
    if (presenceHydro && (TurbEntreBornes || data.PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMinEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = data.NombreDePasDeTempsPourUneOptimisation;

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

        data.NumeroDeContrainteMinEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        builder.greaterThan().build();
    }
    else
        data.NumeroDeContrainteMinEnergieHydraulique[pays] = -1;
}
