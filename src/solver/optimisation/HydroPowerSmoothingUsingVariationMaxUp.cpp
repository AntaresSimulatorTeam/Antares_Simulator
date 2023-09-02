#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int pays, int pdt)
{
    if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    {
        return;
    }

    const int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    const auto& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    int nombreDeTermes = 0;

    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);
    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
    namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxUp(
      problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .include(Variable::HydProd(pays), 1.0)
      .updateHourWithinWeek(0)
      .include(Variable::HydProdUp(pays), -1.0)
      .greaterThan()
      .build();
}