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
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "constraint_builder.h"

#include <antares/study.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

struct PMaxDispatchableGeneration : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
    {
        if (!Simulation)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            double pmaxDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.PmaxDUnGroupeDuPalierThermique[clusterIndex];
            const int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];
            int t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;

            int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            double rhs = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]; // /!\ TODO check

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.PMaxDispatchableGeneration(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.updateHourWithinWeek(pdt)
              .include(Variable::DispatchableProduction(cluster), 1.0)
              .include(Variable::NODU(cluster), -pmaxDUnGroupeDuPalierThermique)
              .lessThan(rhs)
              .build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage += 2;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};

struct PMinDispatchableGeneration : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
    {
        if (!Simulation)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            double pminDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];
            const int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];
            int t1 = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique;

            int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            double rhs = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]; // /!\ TODO check

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.PMinDispatchableGeneration(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.updateHourWithinWeek(pdt)
              .include(Variable::DispatchableProduction(cluster), 1.0)
              .include(Variable::NODU(cluster), -pminDUnGroupeDuPalierThermique)
              .greaterThan(rhs)
              .build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage += 2;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};

struct ConsistenceNODU : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
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
            double rhs = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]; // /!\ TODO check

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                  PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.updateHourWithinWeek(pdt)
              .include(Variable::NODU(cluster), 1.0)
              .updateHourWithinWeek(Pdtmoins1)
              .include(Variable::NODU(cluster), -1)
              .updateHourWithinWeek(pdt)
              .include(Variable::NumberStartingDispatchableUnits(cluster), -1)
              .include(Variable::NumberStoppingDispatchableUnits(cluster), 1)
              .equalTo(rhs)
              .build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage += 4;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};

struct NbUnitsOutageLessThanNbUnitsStop : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
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

            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            double rhs = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]; // /!\ TODO check

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.NbUnitsOutageLessThanNbUnitsStop(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.updateHourWithinWeek(pdt)
              .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0)
              .include(Variable::NumberStoppingDispatchableUnits(cluster), -1.0)
              .lessThan(rhs)
              .build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage += 4;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};

struct NbDispUnitsMinBoundSinceMinUpTime : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[clusterIndex];
        if (!Simulation)
        {
            double pminDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];

            int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            double rhs = 0; // /!\ TODO check

            builder.updateHourWithinWeek(pdt).include(Variable::NODU(cluster), 1.0);

            for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
            {
                int t1 = k;
                if (t1 < 0)
                    t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                int t1moins1 = t1 - 1;

                if (t1moins1 < 0)
                    t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;

                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                    > 0)
                {
                    rhs
                      += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                         - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]; // /!\ TODO check
                }
                builder.updateHourWithinWeek(t1)
                  .include(Variable::NumberStartingDispatchableUnits(cluster), -1.0)
                  .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0);
            }
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.greaterThan(rhs).build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage
              += 1 + 2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};
struct MinDownTime : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        const int DureeMinimaleDArretDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];
        if (!Simulation)
        {
            double pminDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];

            int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            double rhs = 0; // /!\ TODO check

            builder.updateHourWithinWeek(pdt).include(Variable::NODU(cluster), 1.0);

            for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
            {
                int t1 = k;
                if (t1 < 0)
                    t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                int t1moins1 = t1 - 1;

                if (t1moins1 < 0)
                    t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;

                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                    > 0)
                {
                    rhs
                      += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                         - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]; // /!\ TODO check
                }
                builder.updateHourWithinWeek(t1).include(
                  Variable::NumberStoppingDispatchableUnits(cluster), 1.0);
            }
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.MinDownTime(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.lessThan(rhs).build();
        }
        else
        {
            nbTermesContraintesPourLesCoutsDeDemarrage
              += 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique;
            problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};

void OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo,
                                                                     int PremierPdtDeLIntervalle,
                                                                     int DernierPdtDeLIntervalle,
                                                                     bool Simulation)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();
    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage = 0;
    PMaxDispatchableGeneration pMaxDispatchableGeneration(problemeHebdo);
    PMinDispatchableGeneration pMinDispatchableGeneration(problemeHebdo);
    ConsistenceNODU consistenceNODU(problemeHebdo);
    NbUnitsOutageLessThanNbUnitsStop nbUnitsOutageLessThanNbUnitsStop(problemeHebdo);
    NbDispUnitsMinBoundSinceMinUpTime nbDispUnitsMinBoundSinceMinUpTime(problemeHebdo);
    MinDownTime minDownTime(problemeHebdo);

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            const int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdtJour = 0, pdtHebdo = PremierPdtDeLIntervalle;
                 pdtHebdo < DernierPdtDeLIntervalle;
                 pdtHebdo++, pdtJour++)
            {
                int cnt = 2; // TODO
                // if (cnt >= 0)
                // {
                pMaxDispatchableGeneration.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += pMaxDispatchableGeneration.nbTermesContraintesPourLesCoutsDeDemarrage;
                pMinDispatchableGeneration.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += pMinDispatchableGeneration.nbTermesContraintesPourLesCoutsDeDemarrage;
                consistenceNODU.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += consistenceNODU.nbTermesContraintesPourLesCoutsDeDemarrage;
                nbUnitsOutageLessThanNbUnitsStop.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += nbUnitsOutageLessThanNbUnitsStop.nbTermesContraintesPourLesCoutsDeDemarrage;
                nbDispUnitsMinBoundSinceMinUpTime.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += nbDispUnitsMinBoundSinceMinUpTime.nbTermesContraintesPourLesCoutsDeDemarrage;
                minDownTime.add(pays, palier, index, pdtHebdo, Simulation);
                problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
                  += minDownTime.nbTermesContraintesPourLesCoutsDeDemarrage;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
                // }
            }
        }
    }

    return;
}
