/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/simulation.h"

using namespace Antares;

void SIM_AllocationProblemeHebdo(const Data::Study& study,
                                 PROBLEME_HEBDO& problem,
                                 unsigned NombreDePasDeTemps)
{
    try
    {
        SIM_AllocationProblemeDonneesGenerales(problem, study, NombreDePasDeTemps);
        SIM_AllocationProblemePasDeTemps(problem, study, NombreDePasDeTemps);
        SIM_AllocationLinks(problem, study.runtime.interconnectionsCount(), NombreDePasDeTemps);
        SIM_AllocationConstraints(problem, study, NombreDePasDeTemps);
        SIM_AllocateAreas(problem, study, NombreDePasDeTemps);
    }
    catch (const std::bad_alloc& e)
    {
        logs.error() << "Memory allocation failed, aborting (" << e.what() << ")";
    }
}

void SIM_AllocationProblemeDonneesGenerales(PROBLEME_HEBDO& problem,
                                            const Antares::Data::Study& study,
                                            unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime.interconnectionsCount();

    problem.DefaillanceNegativeUtiliserPMinThermique.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserHydro.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserConsoAbattue.assign(nbPays, false);

    problem.CoefficientEcretementPMaxHydraulique.assign(nbPays, 0.);

    problem.BruitSurCoutHydraulique.assign(nbPays, std::vector<double>(8784));

    problem.NomsDesPays.resize(nbPays);

    problem.PaysExtremiteDeLInterconnexion.assign(linkCount, 0);
    problem.PaysOrigineDeLInterconnexion.assign(linkCount, 0);

    problem.CoutDeTransport.resize(linkCount);

    // was previously set to -1 with a loop, now use assign
    problem.IndexDebutIntercoOrigine.assign(nbPays, -1);
    problem.IndexDebutIntercoExtremite.assign(nbPays, -1);

    problem.IndexSuivantIntercoOrigine.assign(linkCount, 0);
    problem.IndexSuivantIntercoExtremite.assign(linkCount, 0);

    problem.NumeroDeJourDuPasDeTemps.assign(NombreDePasDeTemps, 0);
    problem.NumeroDIntervalleOptimiseDuPasDeTemps.assign(NombreDePasDeTemps, 0);
    problem.NbGrpCourbeGuide.assign(NombreDePasDeTemps, 0);
    problem.NbGrpOpt.assign(NombreDePasDeTemps, 0);

    problem.CoutDeDefaillancePositive.assign(nbPays, 0);
    problem.CoutDeDefaillanceNegative.assign(nbPays, 0);

    problem.NumeroDeContrainteEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMinEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMaxEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMaxPompage.assign(nbPays, 0);
    problem.NumeroDeContrainteDeSoldeDEchange.assign(nbPays, 0);

    problem.NumeroDeContrainteEquivalenceStockFinal.assign(nbPays, 0);
    problem.NumeroDeContrainteExpressionStockFinal.assign(nbPays, 0);

    problem.NumeroDeVariableStockFinal.assign(nbPays, 0);
    problem.NumeroDeVariableDeTrancheDeStock.assign(nbPays, std::vector<int>(100));

    problem.ValeursDeNTC.resize(NombreDePasDeTemps);

    problem.ConsommationsAbattues.resize(NombreDePasDeTemps);

    problem.AllMustRunGeneration.resize(NombreDePasDeTemps);
    problem.SoldeMoyenHoraire.resize(NombreDePasDeTemps);
    problem.CorrespondanceVarNativesVarOptim.resize(NombreDePasDeTemps);
    problem.CorrespondanceCntNativesCntOptim.resize(NombreDePasDeTemps);
    problem.VariablesDualesDesContraintesDeNTC.resize(NombreDePasDeTemps);

    auto activeConstraints = study.bindingConstraints.activeConstraints();
    problem.NombreDeContraintesCouplantes = activeConstraints.size();
    problem.MatriceDesContraintesCouplantes.resize(activeConstraints.size());
    problem.PaliersThermiquesDuPays.resize(nbPays);
    problem.CaracteristiquesHydrauliques.resize(nbPays);
    problem.previousSimulationFinalLevel.assign(nbPays, 0.);

    problem.ShortTermStorage.resize(nbPays);

    problem.ReserveJMoins1.resize(nbPays);
    problem.ResultatsHoraires.resize(nbPays);

    problem.coutOptimalSolution1.assign(7, 0.);
    problem.coutOptimalSolution2.assign(7, 0.);
}

void SIM_AllocationProblemePasDeTemps(PROBLEME_HEBDO& problem,
                                      const Antares::Data::Study& study,
                                      unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime.interconnectionsCount();
    const uint shortTermStorageCount = study.runtime.shortTermStorageCount;

    auto activeConstraints = study.bindingConstraints.activeConstraints();

    for (uint k = 0; k < NombreDePasDeTemps; k++)
    {
        problem.ValeursDeNTC[k].ValeurDeNTCOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeNTCExtremiteVersOrigine.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeLoopFlowOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDuFlux.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ResistanceApparente.assign(linkCount, 0.);

        problem.ConsommationsAbattues[k].ConsommationAbattueDuPays.assign(nbPays, 0.);

        problem.AllMustRunGeneration[k].AllMustRunGenerationOfArea.assign(nbPays, 0.);

        problem.SoldeMoyenHoraire[k].SoldeMoyenDuPays.assign(nbPays, 0.);

        auto& variablesMapping = problem.CorrespondanceVarNativesVarOptim[k];
        variablesMapping.NumeroDeVariableDeLInterconnexion.assign(linkCount, 0);
        variablesMapping.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion.assign(linkCount,
                                                                                          0);
        variablesMapping.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion.assign(linkCount,
                                                                                          0);

        variablesMapping.NumeroDeVariableDuPalierThermique
          .assign(study.runtime.thermalPlantTotalCount, 0);
        variablesMapping.NumeroDeVariablesDeLaProdHyd.assign(nbPays, 0);
        variablesMapping.NumeroDeVariablesDePompage.assign(nbPays, 0);
        variablesMapping.NumeroDeVariablesDeNiveau.assign(nbPays, 0);
        variablesMapping.NumeroDeVariablesDeDebordement.assign(nbPays, 0);
        variablesMapping.NumeroDeVariableDefaillancePositive.assign(nbPays, 0);
        variablesMapping.NumeroDeVariableDefaillanceNegative.assign(nbPays, 0);

        variablesMapping.NumeroDeVariablesVariationHydALaBaisse.assign(nbPays, 0);

        variablesMapping.NumeroDeVariablesVariationHydALaHausse.assign(nbPays, 0);

        variablesMapping.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique
          .assign(study.runtime.thermalPlantTotalCount, 0);
        variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
          .assign(study.runtime.thermalPlantTotalCount, 0);
        variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
          .assign(study.runtime.thermalPlantTotalCount, 0);
        variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
          .assign(study.runtime.thermalPlantTotalCount, 0);

        variablesMapping.SIM_ShortTermStorage.InjectionVariable.assign(shortTermStorageCount, 0);
        variablesMapping.SIM_ShortTermStorage.WithdrawalVariable.assign(shortTermStorageCount, 0);
        variablesMapping.SIM_ShortTermStorage.LevelVariable.assign(shortTermStorageCount, 0);

        variablesMapping.SIM_ShortTermStorage.CostVariationInjection.assign(shortTermStorageCount,
                                                                            0);
        variablesMapping.SIM_ShortTermStorage.CostVariationWithdrawal.assign(shortTermStorageCount,
                                                                             0);

        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesBilansPays.assign(nbPays,
                                                                                           0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContraintePourEviterLesChargesFictives.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesNiveauxPays.assign(nbPays,
                                                                                            0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageLevelConstraint.assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionBackward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalBackward.assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroPremiereContrainteDeReserveParZone.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeuxiemeContrainteDeReserveParZone.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDeDissociationDeFlux.assign(linkCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesCouplantes.assign(activeConstraints.size(), 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDeMarche
          .assign(study.runtime.thermalPlantTotalCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDArret
          .assign(study.runtime.thermalPlantTotalCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
          .assign(study.runtime.thermalPlantTotalCount, 0);

        problem.VariablesDualesDesContraintesDeNTC[k]
          .VariableDualeParInterconnexion.assign(linkCount, 0.);
    }
}

void SIM_AllocationLinks(PROBLEME_HEBDO& problem, const uint linkCount, unsigned NombreDePasDeTemps)
{
    for (unsigned k = 0; k < linkCount; ++k)
    {
        problem.CoutDeTransport[k].IntercoGereeAvecDesCouts = false;
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremite.assign(NombreDePasDeTemps,
                                                                              0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigine.assign(NombreDePasDeTemps,
                                                                              0.);
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremiteRef.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigineRef.assign(NombreDePasDeTemps,
                                                                                 0.);
    }
}

void SIM_AllocationConstraints(PROBLEME_HEBDO& problem,
                               const Antares::Data::Study& study,
                               unsigned NombreDePasDeTemps)
{
    auto activeConstraints = study.bindingConstraints.activeConstraints();

    problem.CorrespondanceCntNativesCntOptimJournalieres.resize(7);
    for (uint k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          .NumeroDeContrainteDesContraintesCouplantes.assign(activeConstraints.size(), 0);
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires.NumeroDeContrainteDesContraintesCouplantes
      .assign(activeConstraints.size(), 0);

    const auto& bindingConstraintCount = activeConstraints.size();

    for (unsigned constraintIndex = 0; constraintIndex != bindingConstraintCount; ++constraintIndex)
    {
        assert(constraintIndex < bindingConstraintCount);

        auto bc = activeConstraints[constraintIndex];

        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .SecondMembreDeLaContrainteCouplante.assign(NombreDePasDeTemps, 0.);

        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDeLInterconnexion.assign(bc->linkCount(), 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDeLInterconnexion.assign(bc->linkCount(), 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLInterco.assign(bc->linkCount(), 0);

        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDuPalierDispatch.assign(bc->clusterCount(), 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDuPalierDispatch.assign(bc->clusterCount(), 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLePalierDispatch.assign(bc->clusterCount(), 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PaysDuPalierDispatch.assign(bc->clusterCount(), 0);

        // TODO : create a numberOfTimeSteps method in class of runtime.bindingConstraint
        unsigned int nbTimeSteps;
        switch (bc->type())
        {
            using namespace Antares::Data;
        case BindingConstraint::typeHourly:
            nbTimeSteps = 168;
            break;
        case BindingConstraint::typeDaily:
            nbTimeSteps = 7;
            break;
        case BindingConstraint::typeWeekly:
            nbTimeSteps = 1;
            break;
        default:
            nbTimeSteps = 0;
            break;
        }
        if (nbTimeSteps > 0)
        {
            problem.ResultatsContraintesCouplantes.emplace(std::piecewise_construct,
                                                           std::forward_as_tuple(bc),
                                                           std::forward_as_tuple(nbTimeSteps, 0.));
        }
    }
}

void SIM_AllocateAreas(PROBLEME_HEBDO& problem,
                       const Antares::Data::Study& study,
                       unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    for (unsigned k = 0; k < nbPays; k++)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.enabledAndNotMustRunCount();

        problem.PaliersThermiquesDuPays[k].minUpDownTime.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUneHeure.assign(nbPaliers,
                                                                                       0.);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUnJour.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .TailleUnitaireDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques.assign(nbPaliers, 0);

        problem.PaliersThermiquesDuPays[k]
          .CoutDeDemarrageDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].CoutDArretDUnGroupeDuPalierThermique.assign(nbPaliers,
                                                                                       0.);
        problem.PaliersThermiquesDuPays[k]
          .CoutFixeDeMarcheDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].pminDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].PmaxDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .DureeMinimaleDeMarcheDUnGroupeDuPalierThermique.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k]
          .DureeMinimaleDArretDUnGroupeDuPalierThermique.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].NomsDesPaliersThermiques.resize(nbPaliers);

        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParIntervalleOptimise.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParJour.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxHydrauliqueHoraire.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxHydrauliqueHoraireRef.assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergieHydrauParIntervalleOptimise.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].MinEnergieHydrauParIntervalleOptimise.assign(7, 0.);

        problem.CaracteristiquesHydrauliques[k].NiveauHoraireSup.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].NiveauHoraireInf.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].ApportNaturelHoraire.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].MingenHoraire.assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].WaterLayerValues.assign(100, 0.);
        problem.CaracteristiquesHydrauliques[k].InflowForTimeInterval.assign(100, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergiePompageParIntervalleOptimise.assign(7,
                                                                                              0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxPompageHoraire.assign(NombreDePasDeTemps, 0.);

        problem.ReserveJMoins1[k].ReserveHoraireJMoins1.assign(NombreDePasDeTemps, 0.);

        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.ResultatsHoraires[k]
          .ValeursHorairesDeDefaillancePositiveCSR.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDENS.assign(NombreDePasDeTemps,
                                                                0.); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesLmrViolations.assign(NombreDePasDeTemps,
                                                                         0); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr.assign(NombreDePasDeTemps,
                                                                     0.); // adq patch

        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.ResultatsHoraires[k].TurbinageHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].PompageHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].CoutsMarginauxHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].niveauxHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].valeurH2oHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].debordementsHoraires.assign(NombreDePasDeTemps, 0.);

        problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout.resize(nbPaliers);
        problem.ResultatsHoraires[k].ProductionThermique.resize(NombreDePasDeTemps);

        for (unsigned j = 0; j < nbPaliers; ++j)
        {
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .CoutHoraireDeProductionDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermiqueRef.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermiqueRef.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique.assign(NombreDePasDeTemps, 0);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .NombreMinDeGroupesEnMarcheDuPalierThermique.assign(NombreDePasDeTemps, 0);
        }
        for (unsigned j = 0; j < NombreDePasDeTemps; j++)
        {
            problem.ResultatsHoraires[k].ProductionThermique[j].ProductionThermiqueDuPalier.assign(
              nbPaliers,
              0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesEnMarcheDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiDemarrentDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiSArretentDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiTombentEnPanneDuPalier.assign(nbPaliers, 0.);
        }
        // Short term storage results
        const unsigned long nbShortTermStorage = study.areas.byIndex[k]->shortTermStorage.count();
        problem.ResultatsHoraires[k].ShortTermStorage.resize(NombreDePasDeTemps);
        for (uint pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].injection.resize(nbShortTermStorage);
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].withdrawal.resize(
              nbShortTermStorage);
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].level.resize(nbShortTermStorage);
        }
    }
}
