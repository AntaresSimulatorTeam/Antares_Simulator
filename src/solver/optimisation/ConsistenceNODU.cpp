#include "ConsistenceNODU.h"

void ConsistenceNODU::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        double pminDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];
        const int DureeMinimaleDArretDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];

        int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        int t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;
        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptimTmoins1
          = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdtmoins1];

        if (t1 < 0)
            t1 = NombreDePasDeTempsPourUneOptimisation + t1;

        const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique;
        double rhs = 0; // /!\ TODO check

        builder.updateHourWithinWeek(pdt)
          .include(Variable::NODU(cluster), 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .include(Variable::NODU(cluster), -1)
          .updateHourWithinWeek(pdt)
          .include(Variable::NumberStartingDispatchableUnits(cluster), -1)
          .include(Variable::NumberStoppingDispatchableUnits(cluster), 1)
          .equalTo();
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
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
