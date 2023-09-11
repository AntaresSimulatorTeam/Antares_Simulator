#include "HydroPower.h"

void HydroPower::add(int pays, HydroPowerData& data)
{
    bool presenceHydro = data.presenceHydro;
    bool TurbEntreBornes = data.TurbEntreBornes;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    if (presenceHydro && !TurbEntreBornes)
    {
        if (data.presencePompage)
        {
            data.NumeroDeContrainteEnergieHydraulique[pays]
              = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

            const double pumpingRatio = data.pumpingRatio;
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
        data.NumeroDeContrainteEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

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
    else
        data.NumeroDeContrainteEnergieHydraulique[pays] = -1;
}
