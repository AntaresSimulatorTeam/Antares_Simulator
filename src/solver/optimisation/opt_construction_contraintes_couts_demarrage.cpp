/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"

#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "constraints/PMaxDispatchableGeneration.h"
#include "constraints/PMinDispatchableGeneration.h"
#include "constraints/ConsistenceNODU.h"
#include "constraints/NbUnitsOutageLessThanNbUnitsStop.h"
#include "constraints/NbDispUnitsMinBoundSinceMinUpTime.h"
#include "constraints/MinDownTime.h"

using namespace Antares::Data;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double>& Pi = ProblemeAResoudre->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre->Colonne;
    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            PMaxDispatchableGeneration pMaxDispatchableGeneration(problemeHebdo);
            PMinDispatchableGeneration pMinDispatchableGeneration(problemeHebdo);
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                pMaxDispatchableGeneration.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += pMaxDispatchableGeneration.nbTermesContraintesPourLesCoutsDeDemarrage;

                pMinDispatchableGeneration.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += pMinDispatchableGeneration.nbTermesContraintesPourLesCoutsDeDemarrage;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                ConsistenceNODU consistenceNODU(problemeHebdo);
                consistenceNODU.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += consistenceNODU.nbTermesContraintesPourLesCoutsDeDemarrage;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        NbUnitsOutageLessThanNbUnitsStop nbUnitsOutageLessThanNbUnitsStop(problemeHebdo);

        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbUnitsOutageLessThanNbUnitsStop.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += nbUnitsOutageLessThanNbUnitsStop.nbTermesContraintesPourLesCoutsDeDemarrage;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        NbDispUnitsMinBoundSinceMinUpTime nbDispUnitsMinBoundSinceMinUpTime(problemeHebdo);
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];
            if (DureeMinimaleDeMarcheDUnGroupeDuPalierThermique <= 0)
                continue;
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbDispUnitsMinBoundSinceMinUpTime.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += nbDispUnitsMinBoundSinceMinUpTime.nbTermesContraintesPourLesCoutsDeDemarrage;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        MinDownTime minDownTime(problemeHebdo);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                minDownTime.add(pays, palier, index, pdt, Simulation);
                nbTermesContraintesPourLesCoutsDeDemarrage
                  += minDownTime.nbTermesContraintesPourLesCoutsDeDemarrage;
            }
        }
    }

    if (Simulation)
        problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
          = nbTermesContraintesPourLesCoutsDeDemarrage;

    return;
}
