#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt)
{
    const auto& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesNiveauxPays[pays]
      = ProblemeAResoudre->NombreDeContraintes;

    builder.updateHourWithinWeek(pdt).include(Variable::HydroLevel(pays), 1.0);
    if (pdt > 0)
    {
        builder.updateHourWithinWeek(pdt - 1).include(Variable::HydroLevel(pays), -1.0);
    }
    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);

    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
    namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
    namer.AreaHydroLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .include(Variable::HydProd(pays), 1.0)
      .include(Variable::Pumping(pays),
               -problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio)
      .include(Variable::Overflow(pays), 1.)
      .equalTo()
      .build();
}