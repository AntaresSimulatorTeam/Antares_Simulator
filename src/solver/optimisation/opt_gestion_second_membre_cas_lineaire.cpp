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
#include "opt_rename_problem.h"
#include "constraint_builder.h"
#include <antares/study.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

// Helper functions
void exportPaliers(const PROBLEME_HEBDO& problemeHebdo,
                   ConstraintBuilder& constraintBuilder,
                   int pays)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo.PaliersThermiquesDuPays[pays];

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        constraintBuilder.include(Variable::DispatchableProduction(palier), -1.0);
    }
}

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder)
{
    for (const auto& storage : shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        constraintBuilder.include(Variable::ShortTermStorageInjection(index), 1.0)
          .include(Variable::ShortTermStorageWithdrawal(index), -1.0);
    }
}

// Constraint definitions
struct AreaBalance : public Constraint
{
    using Constraint::Constraint;

    void add(int pdt, int pdtHebdo, int pays, int optimizationNumber)
    {
        // TODO improve this
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.AreaBalance(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        }
        builder.updateHourWithinWeek(pdt);

        int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
        while (interco >= 0)
        {
            builder.include(Variable::NTCDirect(interco), 1.0);
            interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
        }

        interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
        while (interco >= 0)
        {
            builder.include(Variable::NTCDirect(interco), -1.0);
            interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
        }

        exportPaliers(*problemeHebdo, builder, pays);
        builder.include(Variable::HydProd(pays), -1.0)
          .include(Variable::Pumping(pays), 1.0)
          .include(Variable::PositiveUnsuppliedEnergy(pays), -1.0)
          .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

        shortTermStorageBalance(problemeHebdo->ShortTermStorage[pays], builder);

        {
            const CONSOMMATIONS_ABATTUES& ConsommationsAbattues
              = problemeHebdo->ConsommationsAbattues[pdtHebdo];
            double rhs = -ConsommationsAbattues.ConsommationAbattueDuPays[pays];
            bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1);
            bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);
            if (reserveJm1 && opt1)
            {
                rhs -= problemeHebdo->ReserveJMoins1[pays].ReserveHoraireJMoins1[pdtHebdo];
            }
            builder.equalTo(rhs);
            builder.build();
        }
    }
};

struct FictitiousLoad : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pdtHebdo, int pays)
    {
        // TODO improve this
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.FictiveLoads(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        }

        builder.updateHourWithinWeek(pdt);
        exportPaliers(*problemeHebdo, builder, pays);
        builder
          .include(Variable::HydProd(pays), -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays])
          .include(Variable::NegativeUnsuppliedEnergy(pays), 1.0);

        {
            double rhs = 0;
            // Private members ?
            const ALL_MUST_RUN_GENERATION& AllMustRunGeneration
              = problemeHebdo->AllMustRunGeneration[pdtHebdo];
            const CONSOMMATIONS_ABATTUES& ConsommationsAbattues
              = problemeHebdo->ConsommationsAbattues[pdtHebdo];
            const std::vector<bool>& DefaillanceNegativeUtiliserConsoAbattue
              = problemeHebdo->DefaillanceNegativeUtiliserConsoAbattue;
            const std::vector<bool>& DefaillanceNegativeUtiliserPMinThermique
              = problemeHebdo->DefaillanceNegativeUtiliserPMinThermique;

            double MaxAllMustRunGeneration = 0.0;

            if (AllMustRunGeneration.AllMustRunGenerationOfArea[pays] > 0.0)
                MaxAllMustRunGeneration = AllMustRunGeneration.AllMustRunGenerationOfArea[pays];

            double MaxMoinsConsommationBrute = 0.0;
            if (-(ConsommationsAbattues.ConsommationAbattueDuPays[pays]
                  + AllMustRunGeneration.AllMustRunGenerationOfArea[pays])
                > 0.0)
                MaxMoinsConsommationBrute
                  = -(ConsommationsAbattues.ConsommationAbattueDuPays[pays]
                      + AllMustRunGeneration.AllMustRunGenerationOfArea[pays]);

            rhs = DefaillanceNegativeUtiliserConsoAbattue[pays]
                  * (MaxAllMustRunGeneration + MaxMoinsConsommationBrute);

            if (DefaillanceNegativeUtiliserPMinThermique[pays] == 0)
            {
                rhs -= OPT_SommeDesPminThermiques(problemeHebdo, pays, pdtHebdo);
            }
            builder.lessThan(rhs);
        }
        builder.build();
    }
};

struct ShortTermStorageLevel : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pdtHebdo, int pays)
    {
        // TODO improve this
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        const int hourInTheYear = problemeHebdo->weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

        builder.updateHourWithinWeek(pdt);
        for (const auto& storage : problemeHebdo->ShortTermStorage[pays])
        {
            // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
            namer.ShortTermStorageLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                        storage.name);
            const auto index = storage.clusterGlobalIndex;
            builder.include(Variable::ShortTermStorageLevel(index), 1.0)
              .include(Variable::ShortTermStorageLevel(index), -1.0, -1, true)
              .include(Variable::ShortTermStorageInjection(index), -1.0 * storage.efficiency)
              .include(Variable::ShortTermStorageWithdrawal(index), 1.0)
              .equalTo(storage.series->inflows[hourInTheYear])
              .build();
        }
    }
};

struct FlowDissociation : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pdtHebdo, int interco)
    {
        if (const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
            CoutDeTransport.IntercoGereeAvecDesCouts)
        {
            // TODO improve this

            {
                const auto origin
                  = problemeHebdo
                      ->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
                const auto destination
                  = problemeHebdo
                      ->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
                ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                      problemeHebdo->NamedProblems);
                namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                namer.FlowDissociation(
                  problemeHebdo->ProblemeAResoudre->NombreDeContraintes, origin, destination);
            }

            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::NTCDirect(interco), 1.0)
              .include(Variable::IntercoDirectCost(interco), -1.0)
              .include(Variable::IntercoIndirectCost(interco), 1.0);

            if (CoutDeTransport.IntercoGereeAvecLoopFlow)
                builder.equalTo(problemeHebdo->ValeursDeNTC[pdtHebdo]
                                  .ValeurDeLoopFlowOrigineVersExtremite[interco]);
            else
                builder.equalTo(0.);

            builder.build();
        }
    }
};

struct BindingConstraintHour : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pdtHebdo, int cntCouplante)
    {
        const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
        if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
            return;

        builder.updateHourWithinWeek(pdt);
        // Links
        const int nbInterco
          = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
        for (int index = 0; index < nbInterco; index++)
        {
            const int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
            const double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
            const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
            builder.include(Variable::NTCDirect(interco), poids, offset, true);
        }

        // Thermal clusters
        const int nbClusters
          = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
        for (int index = 0; index < nbClusters; index++)
        {
            const int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                  [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
            const double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
            const int offset
              = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
            builder.include(Variable::DispatchableProduction(palier), poids, offset, true);
        }

        double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[pdtHebdo];
        char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
        builder.operatorRHS(op, rhs);
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.BindingConstraintHour(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                        MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
        }
        builder.build();
    }
};

struct BindingConstraintDay : public Constraint
{
    using Constraint::Constraint;
    void add(int cntCouplante)
    {
        const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
        if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
            return;

        const int nbInterco
          = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
        const int nbClusters
          = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation; // TODO
        const int NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
        for (int pdtDebut = 0; pdtDebut < NombreDePasDeTempsPourUneOptimisation;
             pdtDebut += NombreDePasDeTempsDUneJournee)
        {
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
                int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

                for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
                {
                    builder.updateHourWithinWeek(pdt);
                    builder.include(Variable::NTCDirect(interco), poids, offset, true);
                }
            }

            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
                int offset
                  = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

                for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
                {
                    builder.updateHourWithinWeek(pdt);
                    builder.include(Variable::DispatchableProduction(palier), poids, offset, true);
                }
            }
            // TODO probably wrong from the 2nd week, check
            const int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
            double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[jour];
            char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
            builder.operatorRHS(op, rhs);
            {
                ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                      problemeHebdo->NamedProblems);
                namer.UpdateTimeStep(jour);
                namer.BindingConstraintDay(
                  problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                  MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
            }
            builder.build();
        }
    }
};

struct BindingConstraintWeek : public Constraint
{
    using Constraint::Constraint;
    void add(int cntCouplante)
    {
        const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
        if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
            return;

        const int nbInterco
          = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
        const int nbClusters
          = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
                int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

                builder.include(Variable::NTCDirect(interco), poids, offset, true);
            }

            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
                int offset
                  = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
                builder.include(Variable::DispatchableProduction(palier), poids, offset, true);
            }
        }
        // RHS
        {
            double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[0];
            char op = MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
            builder.operatorRHS(op, rhs);
        }
        // Name
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            namer.BindingConstraintWeek(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                        MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
        }
        builder.build();
    }
};

struct HydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        if (!presenceHydro || TurbEntreBornes)
            return;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        if (bool presencePompage
            = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            const double pumpingRatio
              = problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0)
                  .include(Variable::Pumping(pays), -pumpingRatio);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0);
            }
        }
        {
            double rhs = problemeHebdo->CaracteristiquesHydrauliques[pays]
                           .CntEnergieH2OParIntervalleOptimise[NumeroDeLIntervalle];
            builder.equalTo(rhs);
        }
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            namer.HydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        }
        builder.build();
    }
};

struct HydroPowerSmoothingUsingVariationSum : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle)
    {
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            return;
        }

        const int nombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            const auto& CorrespondanceVarNativesVarOptim
              = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
            int nombreDeTermes = 0;

            int pdt1 = pdt + 1;
            if (pdt1 >= nombreDePasDeTempsPourUneOptimisation)
                pdt1 = 0;
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.HydroPowerSmoothingUsingVariationSum(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

            builder.updateHourWithinWeek(pdt)
              .include(Variable::HydProd(pays), 1.0)
              .updateHourWithinWeek(pdt1) /* /!\ Re-check*/
              .include(Variable::HydProd(pays), -1.0)
              .updateHourWithinWeek(pdt) /* /!\ Re-check*/
              .include(Variable::HydProdDown(pays), -1.0)
              .include(Variable::HydProdUp(pays), 1.0)
              .equalTo(0)
              .build();
        }
    }
};
struct HydroPowerSmoothingUsingVariationMaxDown : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle, int pdt)
    {
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            return;
        }

        const int nombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            const auto& CorrespondanceVarNativesVarOptim
              = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
            int nombreDeTermes = 0;

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.HydroPowerSmoothingUsingVariationMaxDown(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

            builder.updateHourWithinWeek(pdt)
              .include(Variable::HydProd(pays), 1.0)
              .updateHourWithinWeek(0)
              .include(Variable::HydProdDown(pays), -1.0)
              .lessThan(0)
              .build();
    }
};
struct HydroPowerSmoothingUsingVariationMaxUp : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle, int pdt)
    {
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            return;
        }

        const int nombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            const auto& CorrespondanceVarNativesVarOptim
              = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
            int nombreDeTermes = 0;

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.HydroPowerSmoothingUsingVariationMaxUp(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

            builder.updateHourWithinWeek(pdt)
              .include(Variable::HydProd(pays), 1.0)
              .updateHourWithinWeek(0)
              .include(Variable::HydProdUp(pays), -1.0)
              .greaterThan(0)
              .build();
    }
};
struct MinHydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        if (presenceHydro
            && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
            const int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            namer.MinHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0);
            }
            const double rhs = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                 .MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
            builder.greaterThan(rhs).build();
        }
    }
};

struct MaxHydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        if (presenceHydro
            && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
            const int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::HydProd(pays), 1.0);
            }

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            namer.MaxHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

            const double rhs = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                 .MaxEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
            builder.lessThan(rhs).build();
        }
    }
};
struct MaxPumping : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int NumeroDeLIntervalle)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            const int NombreDePasDeTempsPourUneOptimisation
              = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.include(Variable::Pumping(pays), 1.0);
            }
            const double rhs = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                 .MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];

            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                                  problemeHebdo->NamedProblems);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            namer.MaxPumping(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
            builder.lessThan(rhs).build();
        }
    }
};

struct AreaHydroLevel : public Constraint
{
    using Constraint::Constraint;
    void add(int pays, int pdt, int pdtHebdo)
    {
        double rhs
          = problemeHebdo->CaracteristiquesHydrauliques[pays].ApportNaturelHoraire[pdtHebdo];
        if (pdtHebdo == 0)
        {
            rhs += problemeHebdo->CaracteristiquesHydrauliques[pays].NiveauInitialReservoir;
        }
        builder.updateHourWithinWeek(pdt).include(Variable::HydroLevel(pays), 1.0);
        if (pdt > 0)
        {
            builder.updateHourWithinWeek(pdt - 1).include(Variable::HydroLevel(pays), -1.0);
        }
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .include(Variable::HydProd(pays), 1.0)
          .include(Variable::Pumping(pays),
                   -problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio)
          .include(Variable::Overflow(pays), 1.)
          .equalTo(rhs)
          .build();
    }
};

struct FinalStockEquivalent : public Constraint
{
    using Constraint::Constraint;
    void add(int pays)
    {
        const auto& pdt = problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockEquivalent(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .include(Variable::FinalStorage(pays), 1.0)
          .include(Variable::HydroLevel(pays), -1.0)
          .equalTo(0)
          .build();
    }
};

struct FinalStockExpression : public Constraint
{
    using Constraint::Constraint;
    void add(int pays)
    {
        const auto pdt = problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1;
        builder.updateHourWithinWeek(pdt).include(Variable::FinalStorage(pays), -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder.include(Variable::LayerStorage(pays, layerindex), 1.0);
        }
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.equalTo(0).build();
    }
};

void OPT_BuildConstraints(PROBLEME_HEBDO* problemeHebdo,
                          int PremierPdtDeLIntervalle,
                          int DernierPdtDeLIntervalle,
                          int NumeroDeLIntervalle,
                          const int optimizationNumber)
{
    int weekFirstHour = problemeHebdo->weekInTheYear * 168;

    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    const std::vector<int>& NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;

    for (int i = 0; i < ProblemeAResoudre->NombreDeContraintes; i++)
    {
        AdresseOuPlacerLaValeurDesCoutsMarginaux[i] = nullptr;
        SecondMembre[i] = 0.0;
    }

    // TODO reset selectively
    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;

    AreaBalance areaBalance(problemeHebdo);
    FictitiousLoad fictitiousLoad(problemeHebdo);
    ShortTermStorageLevel shortTermStorageLevels(problemeHebdo);
    FlowDissociation flowDissociation(problemeHebdo);
    BindingConstraintHour bindingConstraintHour(problemeHebdo);
    BindingConstraintDay bindingConstraintDay(problemeHebdo);
    BindingConstraintWeek bindingConstraintWeek(problemeHebdo);
    HydroPower hydroPower(problemeHebdo);
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(problemeHebdo);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(
      problemeHebdo);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(problemeHebdo);

    MinHydroPower minHydroPower(problemeHebdo);
    MaxHydroPower maxHydroPower(problemeHebdo);
    MaxPumping maxPumping(problemeHebdo);
    AreaHydroLevel areaHydroLevel(problemeHebdo);
    FinalStockEquivalent finalStockEquivalent(problemeHebdo);
    FinalStockExpression finalStockExpression(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle; pdtHebdo++)
    {
        for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
            {
                areaBalance.add(pdt, pdtHebdo, pays, optimizationNumber);
                fictitiousLoad.add(pdt, pdtHebdo, pays);
                shortTermStorageLevels.add(pdt, pdtHebdo, pays);
            }

            for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
            {
                flowDissociation.add(pdt, pdtHebdo, interco);
            }

            for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
                 cntCouplante++)
            {
                bindingConstraintHour.add(pdt, pdtHebdo, cntCouplante);
            }
        }
    }

    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }

    // Weekly binding constraints are only added if the opt period is a week
    // ignored otherwise
    if (problemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        hydroPower.add(pays, NumeroDeLIntervalle);
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            hydroPowerSmoothingUsingVariationSum.add(pays, NumeroDeLIntervalle);
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, NumeroDeLIntervalle, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, NumeroDeLIntervalle, pdt);
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        minHydroPower.add(pays, NumeroDeLIntervalle);
        maxHydroPower.add(pays, NumeroDeLIntervalle);
    }

    // TODO after this
    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        // if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        // {
        //     int cnt = 2; // NumeroDeContrainteMaxPompage[pays];
        //     if (cnt >= 0)
        //     {
        //         SecondMembre[cnt]
        //           = problemeHebdo->CaracteristiquesHydrauliques[pays]
        //               .MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];
        //         AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
        //     }
        // }
        maxPumping.add(pays, NumeroDeLIntervalle);
    }

    for (int pdt = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
                continue;

            areaHydroLevel.add(pays, pdt, pdtHebdo);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess)
        {
            finalStockEquivalent.add(pays);
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            finalStockExpression.add(pays);
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle, false);
    }
}
