#include "MaxHydroPower.h"

void MaxHydroPower::add(int pays, MaxHydroPowerData& data)
{
    bool presenceHydro = data.presenceHydro;
    bool TurbEntreBornes = data.TurbEntreBornes;
    if (presenceHydro && (TurbEntreBornes || data.PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::HydProd(pays), 1.0);
        }
        data.NumeroDeContrainteMaxEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MaxHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
}