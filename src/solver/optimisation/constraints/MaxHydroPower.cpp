#include "MaxHydroPower.h"

void MaxHydroPower::add(int pays)
{
    bool presenceHydro
      = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
    bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
    if (presenceHydro
        && (TurbEntreBornes
            || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
    {
        problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.HydProd(pays, 1.0);
        }
        problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MaxHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.lessThan().build();
    }
    else
        problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
}