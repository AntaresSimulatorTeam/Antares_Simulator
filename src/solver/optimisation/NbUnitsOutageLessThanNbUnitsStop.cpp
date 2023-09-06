#include "NbUnitsOutageLessThanNbUnitsStop.h"

void NbUnitsOutageLessThanNbUnitsStop::add(int pays,
                                           int cluster,
                                           int clusterIndex,
                                           int pdt,
                                           bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int DureeMinimaleDArretDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];

        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
        CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
          = -1;

        builder.updateHourWithinWeek(pdt)
          .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0)
          .include(Variable::NumberStoppingDispatchableUnits(cluster), -1.0)
          .lessThan();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.NbUnitsOutageLessThanNbUnitsStop(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);

            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage += 4;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
