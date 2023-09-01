#include "HydroPower.h"

void HydroPower::add(int pays)
{
    bool presenceHydro
      = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
    bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
    if (!presenceHydro || TurbEntreBornes)
        return;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    if (bool presencePompage
        = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
    {
        problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays]
          = ProblemeAResoudre->NombreDeContraintes;

        const double pumpingRatio = problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::HydProd(pays), 1.0)
              .include(Variable::Pumping(pays), -pumpingRatio);
        }
    }
    else
    {
        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::HydProd(pays), 1.0);
        }
    }
    builder.equalTo();
    {
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.HydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }
    builder.build();
}
