#include "NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(int pays,
                                            int cluster,
                                            int clusterIndex,
                                            int pdt,
                                            bool Simulation)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo->PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[clusterIndex];

    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
      = -1;
    if (!Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique;

        builder.updateHourWithinWeek(pdt).NODU(cluster, 1.0);

        for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1)
              .NumberStartingDispatchableUnits(cluster, -1.0)
              .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0);
        }

        builder.greaterThan();
        if (builder.NumberOfVariables() > 1)
        {
            CorrespondanceCntNativesCntOptim
              .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
              = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + 2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
