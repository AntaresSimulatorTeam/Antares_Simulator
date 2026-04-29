// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void OPT_InitialiserLeSecondMembreDuProblemeLineaireRampesThermiques(PROBLEME_HEBDO* problemeHebdo,
                                                                     int PremierPdtDeLIntervalle,
                                                                     int DernierPdtDeLIntervalle)
{
    logs.info() << "PremierPdtDeLIntervalle= " << PremierPdtDeLIntervalle;

    if (PremierPdtDeLIntervalle > 0)
    {
        const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
        std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

        int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                      ->NombreDePasDeTempsPourUneOptimisation;

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];

            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier = PaliersThermiquesDuPays
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                const CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[0];
                int cnt = CorrespondanceCntNativesCntOptim.ConstraintIndexRampingIncrease[palier];
                if (cnt >= 0)
                {
                    SecondMembre[cnt] = problemeHebdo->ResultatsHoraires[pays]
                                          .ProductionThermique[PremierPdtDeLIntervalle - 1]
                                          .ProductionThermiqueDuPalier[index];
                    logs.info() << "second membre = " << SecondMembre[cnt];
                }

                cnt = CorrespondanceCntNativesCntOptim.ConstraintIndexRampingIncrease[palier];
                if (cnt >= 0)
                {
                    SecondMembre[cnt] = problemeHebdo->ResultatsHoraires[pays]
                                          .ProductionThermique[PremierPdtDeLIntervalle - 1]
                                          .ProductionThermiqueDuPalier[index];
                }

                cnt = CorrespondanceCntNativesCntOptim.ConstraintIndexRampingIncrease[palier];
                if (cnt >= 0)
                {
                    SecondMembre[cnt] = problemeHebdo->ResultatsHoraires[pays]
                                          .ProductionThermique[PremierPdtDeLIntervalle - 1]
                                          .ProductionThermiqueDuPalier[index];
                }
            }
        }
    }
}
