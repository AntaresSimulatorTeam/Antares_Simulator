// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"

#include <cassert>

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/simulation.h"

using namespace Antares;

// ---------------------------------------------------------------------------
// cloneProblemHebdoForWeek
// ---------------------------------------------------------------------------
// Deep-copy rationale for PROBLEME_HEBDO:
//
//  All std::vector<T> members are value-copyable and need no special handling.
//  The unique_ptr<PROBLEME_ANTARES_A_RESOUDRE> ProblemeAResoudre is the only
//  non-trivially-copyable member that requires explicit treatment.
//  Its internal pointer-of-pointers vectors (AdresseOuPlacerLaValeurDes*)
//  point into the PROBLEME_HEBDO data structures; however, these pointers are
//  unconditionally rebuilt at the start of every weekly LP solve inside
//  OPT_InitialiserLesBornesDesVariables*(). Therefore it is safe to start with
//  a fresh PROBLEME_ANTARES_A_RESOUDRE that has all scalar / vector members
//  copied but empty pointer-of-pointer vectors -- the solve will repopulate
//  them against the clone's own data.
//
//  The shared_ptr<AdequacyPatchRuntimeData> adequacyPatchRuntimeData is shared
//  across weeks for read-only access (areaMode, linkMode vectors are read-only
//  during the solve).  The mutable csrTriggeredHoursPerArea_ per-area set is
//  reset at the start of each week's post-process; it is only written from the
//  aggregation thread (sequential), so sharing is safe.
//
//  The const char* NomDeLaContrainteCouplante in CONTRAINTES_COUPLANTES points
//  to string data owned by the study's binding-constraint objects, which live
//  for the duration of the simulation. It is read-only during the solve.
//
//  Solver::ModelerData* modelerData is nullptr in all normal simulations.
// ---------------------------------------------------------------------------

PROBLEME_HEBDO cloneProblemHebdoForWeek(
    const PROBLEME_HEBDO& src,
    uint weekIndex,
    const std::vector<double>& precomputedInitialLevels)
{
    // Step 1: memberwise copy of all value-type / std::vector / shared_ptr members.
    // Because PROBLEME_HEBDO has a deleted copy-constructor (via deleted sub-structs
    // CORRESPONDANCES_DES_VARIABLES / CORRESPONDANCES_DES_CONTRAINTES), we build
    // the copy member-by-member for those fields.

    PROBLEME_HEBDO dst;

    // Scalar / flag fields
    dst.weekInTheYear                   = weekIndex;
    dst.year                            = src.year;
    dst.OptimisationAuPasHebdomadaire   = src.OptimisationAuPasHebdomadaire;
    dst.TypeDeLissageHydraulique        = src.TypeDeLissageHydraulique;
    dst.OptimisationAvecCoutsDeDemarrage= src.OptimisationAvecCoutsDeDemarrage;
    dst.OptimisationAvecVariablesEntieres = src.OptimisationAvecVariablesEntieres;
    dst.NombreDePays                    = src.NombreDePays;
    dst.NombreDePaliersThermiques       = src.NombreDePaliersThermiques;
    dst.NombreDInterconnexions          = src.NombreDInterconnexions;
    dst.NombreDePasDeTemps              = src.NombreDePasDeTemps;
    dst.NombreDePasDeTempsPourUneOptimisation = src.NombreDePasDeTempsPourUneOptimisation;
    dst.NombreDeJours                   = src.NombreDeJours;
    dst.NombreDePasDeTempsDUneJournee   = src.NombreDePasDeTempsDUneJournee;
    dst.NombreDeContraintesCouplantes   = src.NombreDeContraintesCouplantes;
    dst.NumberOfShortTermStorages       = src.NumberOfShortTermStorages;
    dst.YaDeLaReserveJmoins1            = src.YaDeLaReserveJmoins1;
    dst.Expansion                       = src.Expansion;
    dst.HeureDansLAnnee                 = src.HeureDansLAnnee;
    dst.LeProblemeADejaEteInstancie     = src.LeProblemeADejaEteInstancie;
    dst.ExportMPS                       = src.ExportMPS;
    dst.exportMPSOnError                = src.exportMPSOnError;
    dst.NamedProblems                   = src.NamedProblems;
    dst.exportSolutions                 = src.exportSolutions;

    // std::vector members (all copyable by value)
    dst.NomsDesPays                     = src.NomsDesPays;
    dst.PaysOrigineDeLInterconnexion    = src.PaysOrigineDeLInterconnexion;
    dst.PaysExtremiteDeLInterconnexion  = src.PaysExtremiteDeLInterconnexion;
    dst.CoutDeTransport                 = src.CoutDeTransport;
    dst.ValeursDeNTC                    = src.ValeursDeNTC;
    dst.NumeroDeJourDuPasDeTemps        = src.NumeroDeJourDuPasDeTemps;
    dst.NumeroDIntervalleOptimiseDuPasDeTemps = src.NumeroDIntervalleOptimiseDuPasDeTemps;
    dst.ConsommationsAbattues           = src.ConsommationsAbattues;
    dst.CoutDeDefaillancePositive       = src.CoutDeDefaillancePositive;
    dst.CoutDeDefaillanceNegative       = src.CoutDeDefaillanceNegative;
    dst.CoutDeDebordement               = src.CoutDeDebordement;
    dst.PaliersThermiquesDuPays         = src.PaliersThermiquesDuPays;
    dst.CaracteristiquesHydrauliques    = src.CaracteristiquesHydrauliques;
    dst.ShortTermStorage                = src.ShortTermStorage;
    dst.DefaillanceNegativeUtiliserPMinThermique = src.DefaillanceNegativeUtiliserPMinThermique;
    dst.DefaillanceNegativeUtiliserHydro = src.DefaillanceNegativeUtiliserHydro;
    dst.DefaillanceNegativeUtiliserConsoAbattue = src.DefaillanceNegativeUtiliserConsoAbattue;
    dst.BruitSurCoutHydraulique         = src.BruitSurCoutHydraulique;
    dst.MatriceDesContraintesCouplantes = src.MatriceDesContraintesCouplantes;
    dst.ResultatsContraintesCouplantes  = src.ResultatsContraintesCouplantes;
    dst.SoldeMoyenHoraire               = src.SoldeMoyenHoraire;
    dst.IndexDebutIntercoOrigine        = src.IndexDebutIntercoOrigine;
    dst.IndexSuivantIntercoOrigine      = src.IndexSuivantIntercoOrigine;
    dst.IndexDebutIntercoExtremite      = src.IndexDebutIntercoExtremite;
    dst.IndexSuivantIntercoExtremite    = src.IndexSuivantIntercoExtremite;
    dst.NumeroDeContrainteEnergieHydraulique  = src.NumeroDeContrainteEnergieHydraulique;
    dst.NumeroDeContrainteMinEnergieHydraulique = src.NumeroDeContrainteMinEnergieHydraulique;
    dst.NumeroDeContrainteMaxEnergieHydraulique = src.NumeroDeContrainteMaxEnergieHydraulique;
    dst.NumeroDeContrainteMaxPompage    = src.NumeroDeContrainteMaxPompage;
    dst.NumeroDeContrainteDeSoldeDEchange = src.NumeroDeContrainteDeSoldeDEchange;
    dst.NumeroDeContrainteEquivalenceStockFinal = src.NumeroDeContrainteEquivalenceStockFinal;
    dst.NumeroDeContrainteExpressionStockFinal  = src.NumeroDeContrainteExpressionStockFinal;
    dst.NumeroDeVariableStockFinal      = src.NumeroDeVariableStockFinal;
    dst.NumeroDeVariableDeTrancheDeStock = src.NumeroDeVariableDeTrancheDeStock;
    dst.AllMustRunGeneration            = src.AllMustRunGeneration;
    dst.CoefficientEcretementPMaxHydraulique = src.CoefficientEcretementPMaxHydraulique;
    dst.previousSimulationFinalLevel    = src.previousSimulationFinalLevel;
    dst.ResultatsHoraires               = src.ResultatsHoraires;
    dst.VariablesDualesDesContraintesDeNTC = src.VariablesDualesDesContraintesDeNTC;
    dst.coutOptimalSolution1            = src.coutOptimalSolution1;
    dst.coutOptimalSolution2            = src.coutOptimalSolution2;
    dst.NbGrpCourbeGuide                = src.NbGrpCourbeGuide;
    dst.NbGrpOpt                        = src.NbGrpOpt;
    dst.timeMeasure                     = src.timeMeasure;

    // CORRESPONDANCES_DES_VARIABLES (deleted copy ctor) - copy member-by-member
    {
        const std::size_t n = src.CorrespondanceVarNativesVarOptim.size();
        dst.CorrespondanceVarNativesVarOptim.resize(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            const auto& s = src.CorrespondanceVarNativesVarOptim[i];
            auto& d = dst.CorrespondanceVarNativesVarOptim[i];
            d.NumeroDeVariableDeLInterconnexion                            = s.NumeroDeVariableDeLInterconnexion;
            d.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion   = s.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion;
            d.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion   = s.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion;
            d.NumeroDeVariableDuPalierThermique           = s.NumeroDeVariableDuPalierThermique;
            d.NumeroDeVariablesDeLaProdHyd                = s.NumeroDeVariablesDeLaProdHyd;
            d.NumeroDeVariablesDePompage                  = s.NumeroDeVariablesDePompage;
            d.NumeroDeVariablesDeNiveau                   = s.NumeroDeVariablesDeNiveau;
            d.NumeroDeVariablesDeDebordement              = s.NumeroDeVariablesDeDebordement;
            d.NumeroDeVariableDefaillancePositive         = s.NumeroDeVariableDefaillancePositive;
            d.NumeroDeVariableDefaillanceNegative         = s.NumeroDeVariableDefaillanceNegative;
            d.NumeroDeVariablesVariationHydALaBaisse      = s.NumeroDeVariablesVariationHydALaBaisse;
            d.NumeroDeVariablesVariationHydALaHausse      = s.NumeroDeVariablesVariationHydALaHausse;
            d.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique
              = s.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
            d.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
              = s.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
            d.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
              = s.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
            d.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
              = s.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;
            d.SIM_ShortTermStorage.InjectionVariable
              = s.SIM_ShortTermStorage.InjectionVariable;
            d.SIM_ShortTermStorage.WithdrawalVariable
              = s.SIM_ShortTermStorage.WithdrawalVariable;
            d.SIM_ShortTermStorage.LevelVariable
              = s.SIM_ShortTermStorage.LevelVariable;
            d.SIM_ShortTermStorage.CostVariationInjection
              = s.SIM_ShortTermStorage.CostVariationInjection;
            d.SIM_ShortTermStorage.CostVariationWithdrawal
              = s.SIM_ShortTermStorage.CostVariationWithdrawal;
            d.SIM_ShortTermStorage.OverflowVariable
              = s.SIM_ShortTermStorage.OverflowVariable;
        }
    }

    // CORRESPONDANCES_DES_CONTRAINTES (deleted copy ctor) - copy member-by-member
    {
        const std::size_t n = src.CorrespondanceCntNativesCntOptim.size();
        dst.CorrespondanceCntNativesCntOptim.resize(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            const auto& s = src.CorrespondanceCntNativesCntOptim[i];
            auto& d = dst.CorrespondanceCntNativesCntOptim[i];
            d.NumeroDeContrainteDesBilansPays              = s.NumeroDeContrainteDesBilansPays;
            d.NumeroDeContraintePourEviterLesChargesFictives
              = s.NumeroDeContraintePourEviterLesChargesFictives;
            d.NumeroPremiereContrainteDeReserveParZone     = s.NumeroPremiereContrainteDeReserveParZone;
            d.NumeroDeuxiemeContrainteDeReserveParZone     = s.NumeroDeuxiemeContrainteDeReserveParZone;
            d.NumeroDeContrainteDeDissociationDeFlux       = s.NumeroDeContrainteDeDissociationDeFlux;
            d.NumeroDeContrainteDesContraintesCouplantes   = s.NumeroDeContrainteDesContraintesCouplantes;
            d.NumeroDeContrainteDesContraintesDeDureeMinDeMarche
              = s.NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
            d.NumeroDeContrainteDesContraintesDeDureeMinDArret
              = s.NumeroDeContrainteDesContraintesDeDureeMinDArret;
            d.NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
              = s.NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne;
            d.NumeroDeContrainteDesNiveauxPays             = s.NumeroDeContrainteDesNiveauxPays;
            d.ShortTermStorageLevelConstraint              = s.ShortTermStorageLevelConstraint;
            d.ShortTermStorageCostVariationInjectionForward
              = s.ShortTermStorageCostVariationInjectionForward;
            d.ShortTermStorageCostVariationInjectionBackward
              = s.ShortTermStorageCostVariationInjectionBackward;
            d.ShortTermStorageCostVariationWithdrawalForward
              = s.ShortTermStorageCostVariationWithdrawalForward;
            d.ShortTermStorageCostVariationWithdrawalBackward
              = s.ShortTermStorageCostVariationWithdrawalBackward;
        }
    }

    // CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES
    {
        const std::size_t n = src.CorrespondanceCntNativesCntOptimJournalieres.size();
        dst.CorrespondanceCntNativesCntOptimJournalieres.resize(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            dst.CorrespondanceCntNativesCntOptimJournalieres[i]
              .NumeroDeContrainteDesContraintesCouplantes
              = src.CorrespondanceCntNativesCntOptimJournalieres[i]
                  .NumeroDeContrainteDesContraintesCouplantes;
        }
    }

    // CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES
    dst.CorrespondanceCntNativesCntOptimHebdomadaires
      .NumeroDeContrainteDesContraintesCouplantes
      = src.CorrespondanceCntNativesCntOptimHebdomadaires
          .NumeroDeContrainteDesContraintesCouplantes;
    dst.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation
      = src.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation;

    // ReserveJMoins1 (no deleted ctor, but copy it explicitly for clarity)
    dst.ReserveJMoins1 = src.ReserveJMoins1;

    // shared_ptr fields - shared read-only access is safe
    dst.adequacyPatchRuntimeData = src.adequacyPatchRuntimeData;

    // modelerData is a raw pointer (nullptr in normal simulations)
    dst.modelerData = src.modelerData;

    // optimizationStatistics: std::atomic-based, non-copyable.
    // Start fresh for the clone (reset to zero); the solve will accumulate into them.
    dst.optimizationStatistics[0].reset();
    dst.optimizationStatistics[1].reset();

    // Step 2: Create a fresh PROBLEME_ANTARES_A_RESOUDRE for the clone.
    // The pointer-of-pointers vectors (AdresseOuPlacerLaValeurDes*) are
    // rebuilt unconditionally at the start of each LP solve, so we only
    // need to copy the structural (non-pointer) members.
    dst.ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
    {
        const auto& sPAR = *src.ProblemeAResoudre;
        auto& dPAR = *dst.ProblemeAResoudre;

        dPAR.NombreDeVariables   = sPAR.NombreDeVariables;
        dPAR.NombreDeContraintes = sPAR.NombreDeContraintes;
        dPAR.Sens                = sPAR.Sens;
        dPAR.IndicesDebutDeLigne = sPAR.IndicesDebutDeLigne;
        dPAR.NombreDeTermesDesLignes = sPAR.NombreDeTermesDesLignes;
        dPAR.CoefficientsDeLaMatriceDesContraintes = sPAR.CoefficientsDeLaMatriceDesContraintes;
        dPAR.IndicesColonnes     = sPAR.IndicesColonnes;
        dPAR.IncrementDAllocationMatriceDesContraintes
          = sPAR.IncrementDAllocationMatriceDesContraintes;
        dPAR.NombreDeTermesDansLaMatriceDesContraintes
          = sPAR.NombreDeTermesDansLaMatriceDesContraintes;
        dPAR.CoutQuadratique     = sPAR.CoutQuadratique;
        dPAR.CoutLineaire        = sPAR.CoutLineaire;
        dPAR.TypeDeVariable      = sPAR.TypeDeVariable;
        dPAR.Xmin                = sPAR.Xmin;
        dPAR.Xmax                = sPAR.Xmax;
        dPAR.SecondMembre        = sPAR.SecondMembre;
        // Pointer-of-pointers vectors: leave empty; rebuilt by the LP solve
        dPAR.AdresseOuPlacerLaValeurDesVariablesOptimisees.assign(sPAR.NombreDeVariables, nullptr);
        dPAR.X                   = sPAR.X;
        dPAR.AdresseOuPlacerLaValeurDesCoutsMarginaux.assign(sPAR.NombreDeContraintes, nullptr);
        dPAR.CoutsMarginauxDesContraintes = sPAR.CoutsMarginauxDesContraintes;
        dPAR.AdresseOuPlacerLaValeurDesCoutsReduits.assign(sPAR.NombreDeVariables, nullptr);
        dPAR.CoutsReduits        = sPAR.CoutsReduits;
        dPAR.ExistenceDUneSolution = sPAR.ExistenceDUneSolution;
        // ProblemesSpx: each clone needs its own LP solver instances (not shared)
        // They are created lazily during the solve, so start with an empty vector.
        // dPAR.ProblemesSpx stays default-constructed (empty).
        dPAR.PositionDeLaVariable = sPAR.PositionDeLaVariable;
        dPAR.ComplementDeLaBase   = sPAR.ComplementDeLaBase;
        dPAR.Pi                   = sPAR.Pi;
        dPAR.Colonne              = sPAR.Colonne;
        dPAR.NomDesVariables      = sPAR.NomDesVariables;
        dPAR.NomDesContraintes    = sPAR.NomDesContraintes;
        dPAR.VariablesEntieres    = sPAR.VariablesEntieres;
        // basisStatus: start fresh so each week's LP can converge independently
        dPAR.clearBasis();
    }

    // Step 3: Override previousSimulationFinalLevel with precomputed values.
    assert(precomputedInitialLevels.size() == dst.previousSimulationFinalLevel.size());
    for (std::size_t areaIdx = 0; areaIdx < precomputedInitialLevels.size(); ++areaIdx)
    {
        dst.previousSimulationFinalLevel[areaIdx] = precomputedInitialLevels[areaIdx];
    }

    return dst;
}


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
        SIM_AllocationShortermStorageCumulation(problem, study);
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

    problem.CoutDeDebordement.assign(nbPays, 0);

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

        variablesMapping.SIM_ShortTermStorage.OverflowVariable.assign(shortTermStorageCount, 0);

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

void SIM_AllocationShortermStorageCumulation(PROBLEME_HEBDO& problem,
                                             const Antares::Data::Study& study)
{
    problem.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation
      .assign(study.runtime.shortTermStorageCumulativeConstraintCount, 0);
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

        auto linkCount = bc->linkCount();
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDeLInterconnexion.assign(linkCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDeLInterconnexion.assign(linkCount, 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLInterco.assign(linkCount, 0);

        auto clusterCount = bc->clusterCount();
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDuPalierDispatch.assign(clusterCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDuPalierDispatch.assign(clusterCount, 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLePalierDispatch.assign(clusterCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PaysDuPalierDispatch.assign(clusterCount, 0);

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
        problem.ResultatsHoraires[k].CoutsMarginauxHorairesCSR.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].niveauxHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].valeurH2oHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].debordementsHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesNetechangeModeler.assign(NombreDePasDeTemps,
                                                                              0.);

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
        problem.ResultatsHoraires[k].ShortTermStorage.resize(nbShortTermStorage);
        for (uint sts = 0; sts < nbShortTermStorage; sts++)
        {
            problem.ResultatsHoraires[k].ShortTermStorage[sts].injection.resize(NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].withdrawal.resize(
              NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].level.resize(NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].overflow.resize(NombreDePasDeTemps);
        }
    }
}
