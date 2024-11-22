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

#ifndef __SOLVER_SIMULATION_ECO_STRUCTS_H__
#define __SOLVER_SIMULATION_ECO_STRUCTS_H__

#include <memory>
#include <vector>

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/utils/optimization_statistics.h"
#include "antares/study/fwd.h"
#include "antares/study/study.h"

class AdequacyPatchRuntimeData;

struct CORRESPONDANCES_DES_VARIABLES
{
    // Avoid accidental copies
    CORRESPONDANCES_DES_VARIABLES() = default;
    CORRESPONDANCES_DES_VARIABLES(const CORRESPONDANCES_DES_VARIABLES&) = delete;
    CORRESPONDANCES_DES_VARIABLES(CORRESPONDANCES_DES_VARIABLES&&) = default;

    std::vector<int> NumeroDeVariableDeLInterconnexion;
    std::vector<int> NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion;
    std::vector<int> NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion;

    std::vector<int> NumeroDeVariableDuPalierThermique;

    std::vector<int> NumeroDeVariablesDeLaProdHyd;

    std::vector<int> NumeroDeVariablesDePompage;
    std::vector<int> NumeroDeVariablesDeNiveau;
    std::vector<int> NumeroDeVariablesDeDebordement;

    std::vector<int> NumeroDeVariableDefaillancePositive;

    std::vector<int> NumeroDeVariableDefaillanceNegative;

    std::vector<int> NumeroDeVariablesVariationHydALaBaisse;

    std::vector<int> NumeroDeVariablesVariationHydALaHausse;

    std::vector<int> NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
    std::vector<int> NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
    std::vector<int> NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
    std::vector<int> NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;

    struct
    {
        std::vector<int> InjectionVariable;
        std::vector<int> WithdrawalVariable;
        std::vector<int> LevelVariable;
        std::vector<int> CostVariationInjection;
        std::vector<int> CostVariationWithdrawal;
    } SIM_ShortTermStorage;
};

struct CORRESPONDANCES_DES_CONTRAINTES
{
    // Avoid accidental copies
    CORRESPONDANCES_DES_CONTRAINTES() = default;
    CORRESPONDANCES_DES_CONTRAINTES(const CORRESPONDANCES_DES_CONTRAINTES&) = delete;
    CORRESPONDANCES_DES_CONTRAINTES(CORRESPONDANCES_DES_CONTRAINTES&&) = default;

    std::vector<int> NumeroDeContrainteDesBilansPays;
    std::vector<int> NumeroDeContraintePourEviterLesChargesFictives;

    std::vector<int> NumeroPremiereContrainteDeReserveParZone;
    std::vector<int> NumeroDeuxiemeContrainteDeReserveParZone;

    std::vector<int> NumeroDeContrainteDeDissociationDeFlux;
    std::vector<int> NumeroDeContrainteDesContraintesCouplantes;

    std::vector<int> NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
    std::vector<int> NumeroDeContrainteDesContraintesDeDureeMinDArret;
    std::vector<int> NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne;

    std::vector<int> NumeroDeContrainteDesNiveauxPays;

    std::vector<int> ShortTermStorageLevelConstraint;
    std::vector<int> ShortTermStorageCostVariationInjectionForward;
    std::vector<int> ShortTermStorageCostVariationInjectionBackward;
    std::vector<int> ShortTermStorageCostVariationWithdrawalForward;
    std::vector<int> ShortTermStorageCostVariationWithdrawalBackward;
};

struct CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES
{
    std::vector<int> NumeroDeContrainteDesContraintesCouplantes;
};

struct CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES
{
    std::vector<int> NumeroDeContrainteDesContraintesCouplantes;
};

struct VALEURS_DE_NTC_ET_RESISTANCES
{
    std::vector<double> ValeurDeNTCOrigineVersExtremite;
    std::vector<double> ValeurDeNTCExtremiteVersOrigine;
    std::vector<double> ValeurDeLoopFlowOrigineVersExtremite;
    std::vector<double> ValeurDuFlux;

    std::vector<double> ResistanceApparente;
};

struct TRANSFER_BOUND_AND_LEVEL_MARKET_EDGE
{
    double* TransferBoundMarketEdge;
    double* TransferLevelMarketEdge;
};

struct TRANSFER_BOUND_AND_LEVEL_FLEX_UP_EDGE
{
    double* TransferBoundFlexUpEdge;
    double* TransferLevelFlexUpEdge;
};

struct TRANSFER_BOUND_AND_LEVEL_FLEX_DOWN_EDGE
{
    double* TransferBoundFlexDownEdge;
    double* TransferLevelFlexDownEdge;
};

struct CONTRAINTES_COUPLANTES
{
    char TypeDeContrainteCouplante;
    char SensDeLaContrainteCouplante;

    std::vector<double> SecondMembreDeLaContrainteCouplante;

    int NombreDElementsDansLaContrainteCouplante;
    int NombreDInterconnexionsDansLaContrainteCouplante;

    std::vector<double> PoidsDeLInterconnexion;
    std::vector<int> NumeroDeLInterconnexion;
    std::vector<int> OffsetTemporelSurLInterco;

    int NombreDePaliersDispatchDansLaContrainteCouplante;

    std::vector<double> PoidsDuPalierDispatch;
    std::vector<int> PaysDuPalierDispatch;
    std::vector<int> NumeroDuPalierDispatch;
    std::vector<int> OffsetTemporelSurLePalierDispatch;

    const char* NomDeLaContrainteCouplante;

    std::shared_ptr<Data::BindingConstraint> bindingConstraint;
};

namespace ShortTermStorage
{
struct PROPERTIES
{
    double reservoirCapacity;
    double injectionNominalCapacity;
    double withdrawalNominalCapacity;
    double injectionEfficiency;
    double withdrawalEfficiency;
    double initialLevel;
    bool initialLevelOptim;
    bool penalizeVariationWithdrawal;
    bool penalizeVariationInjection;

    std::shared_ptr<Antares::Data::ShortTermStorage::Series> series;

    int clusterGlobalIndex;
    std::string name;
};

using AREA_INPUT = std::vector<::ShortTermStorage::PROPERTIES>; // index is local

struct RESULTS
{
    // Index is the number of the STS in the area
    std::vector<double> level;      // MWh
    std::vector<double> injection;  // MWh
    std::vector<double> withdrawal; // MWh
};
} // namespace ShortTermStorage

struct DEMAND_MARKET_POOL
{
    double* TotalDemandOfMarketPool;
};

struct DEMAND_FLEX_UP_POOL
{
    double* FosteredDemandOfFlexUpPool;
};

struct DEMAND_FLEX_DOWN_POOL
{
    double* FosteredDemandOfFlexDownPool;
};

struct BOUND_FLEX_UP_NODE
{
    double* BoundFlexUpNode;
};

struct BOUND_FLEX_DOWN_NODE
{
    double* BoundFlexDownNode;
};

struct LEVEL_FLEX_UP_NODE
{
    double* LevelFlexUpNode;
};

struct LEVEL_FLEX_DOWN_NODE
{
    double* LevelFlexDownNode;
};

struct CONSOMMATIONS_ABATTUES
{
    std::vector<double> ConsommationAbattueDuPays;
};

struct ALL_MUST_RUN_GENERATION
{
    std::vector<double> AllMustRunGenerationOfArea;
};

struct SOLDE_MOYEN_DES_ECHANGES
{
    std::vector<double> SoldeMoyenDuPays;
};

struct PDISP_ET_COUTS_HORAIRES_PAR_PALIER
{
    std::vector<double> PuissanceDisponibleDuPalierThermique;
    std::vector<double> PuissanceDisponibleDuPalierThermiqueRef;

    std::vector<double> PuissanceMinDuPalierThermique;
    std::vector<double> PuissanceMinDuPalierThermiqueRef;

    std::vector<double> CoutHoraireDeProductionDuPalierThermique;

    std::vector<int> NombreMaxDeGroupesEnMarcheDuPalierThermique;
    std::vector<int> NombreMinDeGroupesEnMarcheDuPalierThermique;
};

struct PALIERS_THERMIQUES
{
    int NombreDePaliersThermiques;

    std::vector<int> minUpDownTime;

    std::vector<double> TailleUnitaireDUnGroupeDuPalierThermique;
    std::vector<double> PminDuPalierThermiquePendantUneHeure;
    std::vector<double> PminDuPalierThermiquePendantUnJour;
    std::vector<int> NumeroDuPalierDansLEnsembleDesPaliersThermiques;
    std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER> PuissanceDisponibleEtCout;

    std::vector<double> CoutDeDemarrageDUnGroupeDuPalierThermique;
    std::vector<double> CoutDArretDUnGroupeDuPalierThermique;
    std::vector<double> CoutFixeDeMarcheDUnGroupeDuPalierThermique;
    std::vector<double> pminDUnGroupeDuPalierThermique;
    std::vector<double> PmaxDUnGroupeDuPalierThermique;
    std::vector<int> DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
    std::vector<int> DureeMinimaleDArretDUnGroupeDuPalierThermique;
    std::vector<std::string> NomsDesPaliersThermiques;
};

struct ENERGIES_ET_PUISSANCES_HYDRAULIQUES
{
    std::vector<double> MinEnergieHydrauParIntervalleOptimise;
    std::vector<double> MaxEnergieHydrauParIntervalleOptimise;

    std::vector<double> CntEnergieH2OParIntervalleOptimise;
    std::vector<double> CntEnergieH2OParJour;

    std::vector<double> ContrainteDePmaxHydrauliqueHoraire;
    std::vector<double> ContrainteDePmaxHydrauliqueHoraireRef;

    std::vector<double> MaxEnergiePompageParIntervalleOptimise;
    std::vector<double> ContrainteDePmaxPompageHoraire;

    double MaxDesPmaxHydrauliques;

    bool PresenceDePompageModulable;
    bool PresenceDHydrauliqueModulable;

    double PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;
    double PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;

    double WeeklyWaterValueStateRegular;

    bool TurbinageEntreBornes;
    bool SansHeuristique;
    bool SuiviNiveauHoraire;

    std::vector<double> NiveauHoraireSup;
    std::vector<double> NiveauHoraireInf;

    std::vector<double> ApportNaturelHoraire;
    std::vector<double> MingenHoraire; /*Minimum Hourly Hydro-Storage Generation*/
    double NiveauInitialReservoir;
    double TailleReservoir;
    double PumpingRatio;

    double WeeklyGeneratingModulation;
    double WeeklyPumpingModulation;
    bool DirectLevelAccess; /*  determines the type of constraints bearing on the final stok level*/
    bool AccurateWaterValue;     /*  determines the type of modelling used for water budget*/
    double LevelForTimeInterval; /*  value computed by the simulator in water-value based modes*/
    std::vector<double> WaterLayerValues;      /*  reference costs for the last time step (caution :
                                      dimension set to      100, should be made dynamic)*/
    std::vector<double> InflowForTimeInterval; /*  Energy input to the reservoir, used to in the
                                      bounding constraint on final level*/
};

class computeTimeStepLevel
{
private:
    int step;
    double level;

    double capacity;
    std::vector<double>& inflows;
    std::vector<double>& ovf;
    const std::vector<double>& turb;
    double pumpRatio;
    const std::vector<double>& pump;
    double excessDown;

public:
    computeTimeStepLevel(const double& startLvl,
                         std::vector<double>& infl,
                         std::vector<double>& overfl,
                         const std::vector<double>& H,
                         double pumpEff,
                         const std::vector<double>& Pump,
                         double rc):
        step(0),
        level(startLvl),
        capacity(rc),
        inflows(infl),
        ovf(overfl),
        turb(H),
        pumpRatio(pumpEff),
        pump(Pump),
        excessDown(0.)
    {
    }

    void run()
    {
        excessDown = 0.;

        level = level + inflows[step] - turb[step] + pumpRatio * pump[step];

        if (level > capacity)
        {
            ovf[step] = level - capacity;
            level = capacity;
        }

        if (level < 0)
        {
            excessDown = -level;
            level = 0.;
            inflows[step] += excessDown;
        }
    }

    void prepareNextStep()
    {
        step++;

        inflows[step] -= excessDown;
    }

    double getLevel()
    {
        return level;
    }
};

struct RESERVE_JMOINS1
{
    std::vector<double> ReserveHoraireJMoins1;
};

struct PRODUCTION_THERMIQUE_OPTIMALE
{
    std::vector<double> ProductionThermiqueDuPalier;

    std::vector<double> NombreDeGroupesEnMarcheDuPalier;
    std::vector<double> NombreDeGroupesQuiDemarrentDuPalier;

    std::vector<double> NombreDeGroupesQuiSArretentDuPalier;

    std::vector<double> NombreDeGroupesQuiTombentEnPanneDuPalier;
};

struct RESULTATS_HORAIRES
{
    std::vector<double> ValeursHorairesDeDefaillancePositive;
    std::vector<double> ValeursHorairesDeDefaillancePositiveCSR;
    std::vector<double> ValeursHorairesDENS;       // adq patch domestic unsupplied energy
    std::vector<int> ValeursHorairesLmrViolations; // adq patch lmr violations
    std::vector<double> ValeursHorairesDtgMrgCsr;  // adq patch DTG MRG after CSR

    std::vector<double> ValeursHorairesDeDefaillanceNegative;

    std::vector<double> PompageHoraire;
    std::vector<double> TurbinageHoraire;

    std::vector<double> niveauxHoraires;
    std::vector<double> valeurH2oHoraire;

    std::vector<double> debordementsHoraires;

    std::vector<double> CoutsMarginauxHoraires;
    std::vector<PRODUCTION_THERMIQUE_OPTIMALE> ProductionThermique; // index is pdtHebdo

    std::vector<::ShortTermStorage::RESULTS> ShortTermStorage;
};

struct COUTS_DE_TRANSPORT
{
    bool IntercoGereeAvecDesCouts;
    bool IntercoGereeAvecLoopFlow;
    std::vector<double> CoutDeTransportOrigineVersExtremite;
    std::vector<double> CoutDeTransportExtremiteVersOrigine;

    std::vector<double> CoutDeTransportOrigineVersExtremiteRef;
    std::vector<double> CoutDeTransportExtremiteVersOrigineRef;
};

struct TIME_MEASURE
{
    long solveTime = 0;
    long updateTime = 0;
};

using TIME_MEASURES = std::array<TIME_MEASURE, 2>;

struct VARIABLES_DUALES_INTERCONNEXIONS
{
    std::vector<double> VariableDualeParInterconnexion;
};

struct PROBLEME_HEBDO
{
    uint32_t weekInTheYear = 0;
    uint32_t year = 0;

    /* Business problem */
    bool OptimisationAuPasHebdomadaire = false;
    char TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
    bool WaterValueAccurate = false;
    bool OptimisationAvecCoutsDeDemarrage = false;
    bool OptimisationAvecVariablesEntieres = false;
    uint32_t NombreDePays = 0;
    std::vector<const char*> NomsDesPays;
    uint32_t NombreDePaliersThermiques = 0;

    uint32_t NombreDInterconnexions = 0;
    std::vector<int> PaysOrigineDeLInterconnexion;
    std::vector<int> PaysExtremiteDeLInterconnexion;
    std::vector<COUTS_DE_TRANSPORT> CoutDeTransport;

    std::vector<VALEURS_DE_NTC_ET_RESISTANCES> ValeursDeNTC;

    uint32_t NombreDePasDeTemps = 0;
    std::vector<int32_t> NumeroDeJourDuPasDeTemps;

    // TODO use uint32_t and figure why tests fails
    int32_t NombreDePasDeTempsPourUneOptimisation = 0;
    std::vector<int32_t> NumeroDIntervalleOptimiseDuPasDeTemps;
    uint32_t NombreDeJours = 0;

    // TODO same as NombreDePasDeTemps
    int32_t NombreDePasDeTempsDUneJournee = 0;

    std::vector<CONSOMMATIONS_ABATTUES> ConsommationsAbattues;

    std::vector<double> CoutDeDefaillancePositive;
    std::vector<double> CoutDeDefaillanceNegative;

    std::vector<PALIERS_THERMIQUES> PaliersThermiquesDuPays;
    std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;

    uint32_t NumberOfShortTermStorages = 0;
    // problemeHebdo->ShortTermStorage[areaIndex][clusterIndex].capacity;
    std::vector<::ShortTermStorage::AREA_INPUT> ShortTermStorage;

    /* Optimization problem */
    std::vector<bool> DefaillanceNegativeUtiliserPMinThermique;
    std::vector<bool> DefaillanceNegativeUtiliserHydro;
    std::vector<bool> DefaillanceNegativeUtiliserConsoAbattue;

    char TypeDOptimisation = OPTIMISATION_LINEAIRE; // OPTIMISATION_LINEAIRE or
                                                    // OPTIMISATION_QUADRATIQUE
    std::vector<std::vector<double>> BruitSurCoutHydraulique;

    uint32_t NombreDeContraintesCouplantes = 0;
    std::vector<CONTRAINTES_COUPLANTES> MatriceDesContraintesCouplantes;
    std::unordered_map<std::shared_ptr<Data::BindingConstraint>, std::vector<double>>
      ResultatsContraintesCouplantes;

    std::vector<SOLDE_MOYEN_DES_ECHANGES> SoldeMoyenHoraire; // Used for quadratic opt
    /* Implementation details : I/O, error management, etc. */
    bool ReinitOptimisation = false;

    Data::mpsExportStatus ExportMPS = Data::mpsExportStatus::NO_EXPORT;
    bool exportMPSOnError = false;
    bool ExportStructure = false;
    bool NamedProblems = false;

    uint32_t HeureDansLAnnee = 0;
    bool LeProblemeADejaEteInstancie = false;
    bool firstWeekOfSimulation = false;

    std::vector<CORRESPONDANCES_DES_VARIABLES> CorrespondanceVarNativesVarOptim;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES> CorrespondanceCntNativesCntOptim;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>
      CorrespondanceCntNativesCntOptimJournalieres;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES CorrespondanceCntNativesCntOptimHebdomadaires;

    std::vector<RESERVE_JMOINS1> ReserveJMoins1;

    std::vector<int> IndexDebutIntercoOrigine;
    std::vector<int> IndexSuivantIntercoOrigine;
    std::vector<int> IndexDebutIntercoExtremite;
    std::vector<int> IndexSuivantIntercoExtremite;

    bool Expansion = false;

    std::vector<int> NumeroDeContrainteEnergieHydraulique;
    std::vector<int> NumeroDeContrainteMinEnergieHydraulique;
    std::vector<int> NumeroDeContrainteMaxEnergieHydraulique;
    std::vector<int> NumeroDeContrainteMaxPompage;
    std::vector<int> NumeroDeContrainteDeSoldeDEchange;

    std::vector<int> NumeroDeContrainteEquivalenceStockFinal;
    std::vector<int> NumeroDeContrainteExpressionStockFinal;

    std::vector<int> NumeroDeVariableStockFinal;
    std::vector<std::vector<int>> NumeroDeVariableDeTrancheDeStock;

    bool YaDeLaReserveJmoins1 = false;

    std::vector<ALL_MUST_RUN_GENERATION> AllMustRunGeneration;

    OptimizationStatistics optimizationStatistics[2];

    /* Adequacy Patch */
    std::shared_ptr<AdequacyPatchRuntimeData> adequacyPatchRuntimeData;

    /* Hydro management */
    std::vector<double> CoefficientEcretementPMaxHydraulique;
    std::vector<double> previousSimulationFinalLevel;

    /* Results */
    std::vector<RESULTATS_HORAIRES> ResultatsHoraires;
    std::vector<VARIABLES_DUALES_INTERCONNEXIONS> VariablesDualesDesContraintesDeNTC;

    std::vector<double> coutOptimalSolution1;
    std::vector<double> coutOptimalSolution2;

    TIME_MEASURES timeMeasure;

    /* Unused for now, will be used in future revisions */
#if 0
    char SecondeOptimisationRelaxee;
    char MarketPoolActivated;
    char FlexUpPoolActivated;
    char FlexDownPoolActivated;
    char ImpressionDuCritere;
    char UnitCommitmentExact;
    char BorneDeLaDefaillancePositive;

    int NumberOfMarketOfferPool;
    int NumberOfMarketDemandPool;
    int NumberOfFlexUpOfferPool;
    int NumberOfFlexUpDemandPool;
    int NumberOfFlexDownOfferPool;
    int NumberOfFlexDownDemandPool;

    int** IsInMarketOfferPool;
    int** IsInMarketDemandPool;
    int** IsInFlexUpOfferPool;
    int** IsInFlexUpDemandPool;
    int** IsInFlexDownOfferPool;
    int** IsInFlexDownDemandPool;

    int NumberOfMarketEdge;
    int* OfferPoolOfMarketEdge;
    int* DemandPoolOfMarketEdge;

    int* IndexFirstMarketEdgeOffer;
    int* IndexNextMarketEdgeOffer;
    int* IndexFirstMarketEdgeDemand;
    int* IndexNextMarketEdgeDemand;

    int NumberOfFlexUpEdge;
    int* OfferPoolOfFlexUpEdge;
    int* DemandPoolOfFlexUpEdge;

    int* IndexFirstFlexUpEdgeOffer;
    int* IndexNextFlexUpEdgeOffer;
    int* IndexFirstFlexUpEdgeDemand;
    int* IndexNextFlexUpEdgeDemand;

    int NumberOfFlexDownEdge;
    int* OfferPoolOfFlexDownEdge;
    int* DemandPoolOfFlexDownEdge;

    int* IndexFirstFlexDownEdgeOffer;
    int* IndexNextFlexDownEdgeOffer;
    int* IndexFirstFlexDownEdgeDemand;
    int* IndexNextFlexDownEdgeDemand;

    double** MarketPoolQuota;

    double** FlexUpPoolQuota;

    double** FlexDownPoolQuota;

    TRANSFER_BOUND_AND_LEVEL_MARKET_EDGE** DataMarketEdges;
    TRANSFER_BOUND_AND_LEVEL_FLEX_UP_EDGE** DataFlexUpEdges;
    TRANSFER_BOUND_AND_LEVEL_FLEX_DOWN_EDGE** DataFlexDownEdges;

    DEMAND_MARKET_POOL** DemandMarketPool;
    DEMAND_FLEX_UP_POOL** DemandFlexUpPool;
    DEMAND_FLEX_DOWN_POOL** DemandFlexDownPool;
    BOUND_FLEX_UP_NODE** BoundFlexUpNode;
    BOUND_FLEX_DOWN_NODE** BoundFlexDownNode;
    LEVEL_FLEX_UP_NODE** LevelFlexUpNode;
    LEVEL_FLEX_DOWN_NODE** LevelFlexDownNode;

    int* MarketOfferPoolOfNode;
    int* MarketDemandPoolOfNode;
    int* FlexUpOfferPoolOfNode;
    int* FlexUpDemandPoolOfNode;
    int* FlexDownOfferPoolOfNode;
    int* FlexDownDemandPoolOfNode;
#endif

public:
    /* Unknown status */
    std::vector<int> NbGrpCourbeGuide; // ?
    std::vector<int> NbGrpOpt;         // ?

    std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE> ProblemeAResoudre;

    double maxPminThermiqueByDay[366];
};
#endif
