#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt)
{
    const auto& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesNiveauxPays[pays]
      = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
    if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
    {
        builder.updateHourWithinWeek(pdt).HydroLevel(pays, 1.0);
        if (pdt > 0)
        {
            builder.updateHourWithinWeek(pdt - 1).HydroLevel(pays, -1.0);
        }
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesNiveauxPays[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        builder.updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .Pumping(pays, -problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio)
          .Overflow(pays, 1.)
          .equalTo()
          .build();
    }
    else
        CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesNiveauxPays[pays] = -1;
}