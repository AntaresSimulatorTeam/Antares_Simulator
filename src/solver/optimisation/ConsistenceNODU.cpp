#include "ConsistenceNODU.h"

void ConsistenceNODU::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];

        int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptimTmoins1
          = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdtmoins1];

        builder.updateHourWithinWeek(pdt)
          .NODU(cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .NODU(cluster, -1)
          .updateHourWithinWeek(pdt)
          .include(Variable::NumberStartingDispatchableUnits(cluster), -1)
          .NumberStoppingDispatchableUnits(cluster, 1)
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
