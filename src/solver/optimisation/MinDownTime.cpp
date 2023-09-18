#include "MinDownTime.h"

void MinDownTime::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo->PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];

    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster] = -1;
    if (!Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique;

        builder.updateHourWithinWeek(pdt).NODU(cluster, 1.0);

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1).include(
              Variable::NumberStoppingDispatchableUnits(cluster), 1.0);
        }
        builder.lessThan();
        if (builder.NumberOfVariables() > 1)
        {
            CorrespondanceCntNativesCntOptim
              .NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
              = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.MinDownTime(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);

            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
