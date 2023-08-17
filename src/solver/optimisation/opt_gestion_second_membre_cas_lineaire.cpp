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

void exportPaliers(const PROBLEME_HEBDO& problemeHebdo,
                   ConstraintBuilder& constraintBuilder,
                   int pays)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo.PaliersThermiquesDuPays[pays];

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        constraintBuilder.updateIndex(palier);
        constraintBuilder.include(Variable::DispatchableProduction, -1.0);
    }
}

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder)
{
    for (const auto& storage : shortTermStorageInput)
    {
        const int clusterGlobalIndex = storage.clusterGlobalIndex;
        constraintBuilder.updateIndex(clusterGlobalIndex);
        constraintBuilder
          .include(Variable::ShortTermStorageInjection, 1.0)
          .include(Variable::ShortTermStorageWithdrawal, -1.0);
    }
}

struct AreaBalance : public IConstraint
{
  using IConstraint::IConstraint;
              
  void add(int pdt, int pdtHebdo, int pays, int optimizationNumber) override
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
        builder.updateIndex(interco);
        builder.include(Variable::NTCDirect, 1.0);
        interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
      }

    interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
      {
        builder.updateIndex(interco);
        builder.include(Variable::NTCDirect, -1.0);
        interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
      }

    exportPaliers(*problemeHebdo, builder, pays);
    builder.updateIndex(pays);
    builder
      .include(Variable::HydProd, -1.0)
      .include(Variable::Pumping, 1.0)
      .include(Variable::PositiveUnsuppliedEnergy, -1.0)
      .include(Variable::NegativeUnsuppliedEnergy, 1.0);

    shortTermStorageBalance(problemeHebdo->ShortTermStorage[pays],
                            builder);

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
      builder.equal(rhs);
    }
    builder.build();
  }
};

struct FictiveLoad : public IConstraint
{
  using IConstraint::IConstraint;
  void add(int pdt, int pdtHebdo, int pays, int optimizationNumber) override
  {
    // TODO improve this
    {
      ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes, problemeHebdo->NamedProblems);
    
      namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
      namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
      namer.FictiveLoads(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
    }

    builder.updateHourWithinWeek(pdt);
    exportPaliers(*problemeHebdo, builder, pays);
    builder.updateIndex(pays);
    builder
      .include(Variable::HydProd, -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays])
      .include(Variable::NegativeUnsuppliedEnergy, 1.0);

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
        builder.less(rhs);
          }
      builder.build();
  }
};

struct ShortTermStorageLevel : public IConstraint
{
  using IConstraint::IConstraint;
  void add(int pdt, int pdtHebdo, int pays, int optimizationNumber) override
  {
    // TODO improve this
    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes, problemeHebdo->NamedProblems);
    const int hourInTheYear = problemeHebdo->weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

    builder.updateHourWithinWeek(pdt);
    for (const auto& storage : problemeHebdo->ShortTermStorage[pays])
    {
      // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
      namer.ShortTermStorageLevel(problemeHebdo->ProblemeAResoudre->NombreDeContraintes, storage.name);
      builder.updateIndex(storage.clusterGlobalIndex);
      builder
        .include(Variable::ShortTermStorageLevel, 1.0)
        .include(Variable::ShortTermStorageLevel, -1.0, -1, true)
        .include(Variable::ShortTermStorageInjection, -1.0 * storage.efficiency)
        .include(Variable::ShortTermStorageWithdrawal, 1.0)
        .equal(storage.series->inflows[hourInTheYear])
        .build();
    }
  }
};

struct FlowDissociation : public IConstraint
{
  using IConstraint::IConstraint;
  void add(int pdt, int pdtHebdo, int interco, int optimizationNumber) override
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
          ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes, problemeHebdo->NamedProblems);
          namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
          namer.FlowDissociation(problemeHebdo->ProblemeAResoudre->NombreDeContraintes, origin, destination);
        }

        builder.updateHourWithinWeek(pdt);
        builder.updateIndex(interco);
        builder.include(Variable::NTCDirect, 1.0)
          .include(Variable::IntercoDirectCost, -1.0)
          .include(Variable::IntercoIndirectCost, 1.0);

        if (CoutDeTransport.IntercoGereeAvecLoopFlow)
          builder.equal(problemeHebdo->ValeursDeNTC[pdtHebdo]
                        .ValeurDeLoopFlowOrigineVersExtremite[interco]);
        else
          builder.equal(0.);

        builder.build();
      }
  }
};

struct BindingConstraintHour: public IConstraint
{
  using IConstraint::IConstraint;
  void add(int pdt, int pdtHebdo, int cntCouplante, int optimizationNumber) override
  {
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
        return;

    // Links
    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
    {
      const int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
      builder.updateIndex(interco);
      const double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
      const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
      builder.include(Variable::NTCDirect, poids, offset, true);
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
      builder.updateIndex(palier);
      builder.include(Variable::DispatchableProduction, poids, offset, true);
    }

    double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[pdtHebdo];
    switch(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante) {
    case '=':
      builder.equal(rhs);
      break;
    case '<':
      builder.less(rhs);
      break;
    case '>':
      builder.greater(rhs);
      break;
      //TODO default case ?
    }

    builder.build();
  }
};


struct BindingConstraintDaily : public IConstraint
{
  using IConstraint::IConstraint;
  void add(int pdtDebut, int, int cntCouplante, int optimizationNumber) override
  {
    const int nombreDePasDeTempsDUneJournee = 24; // TODO pass to constructor

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
      return;

    const int nbInterco
      = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters
      = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
      {
        int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

        builder.updateIndex(interco);
        for (int pdt = pdtDebut; pdt < pdtDebut + nombreDePasDeTempsDUneJournee; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::NTCDirect, poids, offset, true);
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

        builder.updateIndex(palier);
        for (int pdt = pdtDebut; pdt < pdtDebut + nombreDePasDeTempsDUneJournee; pdt++)
          {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::DispatchableProduction, poids, offset, true);
          }
      }
    const int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
    double rhs = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[jour];
    switch(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante) {
    case '=':
      builder.equal(rhs);
      break;
    case '<':
      builder.less(rhs);
      break;
    case '>':
      builder.greater(rhs);
      break;
      //TODO default case ?
    }
    builder.build();
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
    FictiveLoad fictiveLoad(problemeHebdo);
    ShortTermStorageLevel shortTermStorageLevels(problemeHebdo);
    FlowDissociation flowDissociation(problemeHebdo);
    BindingConstraintHour bindingConstraintHour(problemeHebdo);
    BindingConstraintDaily bindingConstraintDaily(problemeHebdo);

    for (int pdt = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdt++)
    {
      for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
      {
        areaBalance.add(pdt, pdtHebdo, pays, optimizationNumber);
        fictiveLoad.add(pdt, pdtHebdo, pays, optimizationNumber);
        shortTermStorageLevels.add(pdt, pdtHebdo, pays, optimizationNumber);
      }

      for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
      {
        flowDissociation.add(pdt, pdtHebdo, interco, optimizationNumber);
      }

      for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
      {
        bindingConstraintHour.add(pdt, pdtHebdo, cntCouplante, optimizationNumber);
      }
    }
    
    const int nombreDePasDeTempsPourUneOptimisation = 168; // TODO
    const int NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
      {
        for (int pdtDebut = 0; pdtDebut < nombreDePasDeTempsPourUneOptimisation; pdtDebut += NombreDePasDeTempsDUneJournee)
        {
            bindingConstraintDaily.add(pdtDebut, 0, cntCouplante, optimizationNumber);
        }
      }



    if (problemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

            if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante
                != CONTRAINTE_HEBDOMADAIRE)
                continue;

            int cnt = 2;
            if (cnt >= 0)
            {
                SecondMembre[cnt]
                  = MatriceDesContraintesCouplantes.SecondMembreDeLaContrainteCouplante[0];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt]
                  = problemeHebdo->ResultatsContraintesCouplantes[cntCouplante].variablesDuales.data();
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        int cnt = 2;
        if (cnt >= 0)
        {
            SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                  .CntEnergieH2OParIntervalleOptimise[NumeroDeLIntervalle];
            AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        if (presenceHydro && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
            int cnt =  2;//NumeroDeContrainteMinEnergieHydraulique[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        if (presenceHydro && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
          int cnt = 2; //NumeroDeContrainteMaxEnergieHydraulique[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
          int cnt = 2;//NumeroDeContrainteMaxPompage[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pdt = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
                continue;

            int cnt = 2;
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .ApportNaturelHoraire[pdtHebdo];
                if (pdtHebdo == 0)
                {
                    SecondMembre[cnt]
                      += problemeHebdo->CaracteristiquesHydrauliques[pays].NiveauInitialReservoir;
                }
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess)
        {
          int cnt = 2;//problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
          int cnt = 2; //problemeHebdo->NumeroDeContrainteExpressionStockFinal[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
