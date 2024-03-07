#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"

#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "constraints/RampingDecreaseRate.h"
#include "constraints/RampingIncreaseRate.h"
#include "constraints/PowerOutputVariation.h"

#include <antares/study/study.h>

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireRampesThermiques(PROBLEME_HEBDO* problemeHebdo, bool Simulation)
{
    int nombreDePasDeTempsPourUneOptimisation  = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes);

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] != -1)
            {
                RampingIncreaseRate rampingIncreaseRate(problemeHebdo);
                RampingDecreaseRate rampingDecreaseRate(problemeHebdo);
                PowerOutputVariation powerOutputVariation(problemeHebdo);

                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    rampingIncreaseRate.add(pays, palier, index, pdt, Simulation);
                    rampingDecreaseRate.add(pays, palier, index, pdt, Simulation);
                    powerOutputVariation.add(pays, palier, index, pdt, Simulation);
                }
            }
        }
    }
}