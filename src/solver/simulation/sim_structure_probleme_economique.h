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

#ifndef __SOLVER_SIMULATION_ECO_STRUCTS_H__
#define __SOLVER_SIMULATION_ECO_STRUCTS_H__

#include "../optimisation/opt_structure_probleme_a_resoudre.h"
#include "../utils/optimization_statistics.h"
#include "../../libs/antares/study/fwd.h"
#include "../../libs/antares/study/study.h"
#include <vector>
#include <optional>
#include <memory>

class AdequacyPatchRuntimeData;

struct CORRESPONDANCES_DES_VARIABLES
{
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
    } SIM_ShortTermStorage;
};

struct CORRESPONDANCES_DES_CONTRAINTES
{
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
    std::vector<double> SecondMembreDeLaContrainteCouplanteRef;

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
};

namespace ShortTermStorage
{
struct PROPERTIES
{
    double reservoirCapacity;
    double injectionNominalCapacity;
    double withdrawalNominalCapacity;
    double efficiency;
    std::optional<double> initialLevel;

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

struct RESULTATS_CONTRAINTES_COUPLANTES
{
    std::vector<double> variablesDuales;
};

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

    std::vector<double> CoutHoraireDuPalierThermiqueUp;
    std::vector<double> CoutHoraireDuPalierThermiqueDown;

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
    mutable std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER> PuissanceDisponibleEtCout;

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
    double WeeklyWaterValueStateUp;
    double WeeklyWaterValueStateDown;

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
    std::vector<double> WaterLayerValues; /*  reference costs for the last time step (caution : dimension set to
                                 100, should be made dynamic)*/
    std::vector<double> InflowForTimeInterval; /*  Energy input to the reservoir, used to in the bounding
                                      constraint on final level*/
};

class computeTimeStepLevel
{
private:
    int step;
    double level;

    double capacity;
    std::vector<double>& inflows;
    std::vector<double>& ovf;
    std::vector<double>& turb;
    double pumpRatio;
    std::vector<double>& pump;
    double excessDown;

public:
    computeTimeStepLevel(
            const double& startLvl,
            std::vector<double>& infl,
            std::vector<double>& overfl,
            std::vector<double>& H,
            double pumpEff,
            std::vector<double>& Pump,
            double rc) :
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

    std::vector<double> ProductionThermiqueDuPalierUp;
    std::vector<double> ProductionThermiqueDuPalierDown;

    std::vector<double> NombreDeGroupesEnMarcheDuPalier;
    std::vector<double> NombreDeGroupesQuiDemarrentDuPalier;

    std::vector<double> NombreDeGroupesQuiSArretentDuPalier;

    std::vector<double> NombreDeGroupesQuiTombentEnPanneDuPalier;

};

struct RESULTATS_HORAIRES
{
    std::vector<double> ValeursHorairesDeDefaillancePositive;
    std::vector<double> ValeursHorairesDENS;                  // adq patch domestic unsupplied energy
    mutable std::vector<int> ValeursHorairesLmrViolations;    // adq patch lmr violations
    std::vector<double> ValeursHorairesSpilledEnergyAfterCSR; // adq patch spillage after CSR
    std::vector<double> ValeursHorairesDtgMrgCsr;             // adq patch DTG MRG after CSR
    std::vector<double> ValeursHorairesDeDefaillancePositiveUp;
    std::vector<double> ValeursHorairesDeDefaillancePositiveDown;
    std::vector<double> ValeursHorairesDeDefaillancePositiveAny;

    std::vector<double> ValeursHorairesDeDefaillanceNegative;
    std::vector<double> ValeursHorairesDeDefaillanceNegativeUp;
    std::vector<double> ValeursHorairesDeDefaillanceNegativeDown;
    std::vector<double> ValeursHorairesDeDefaillanceNegativeAny;

    std::vector<double> ValeursHorairesDeDefaillanceEnReserve;
    std::vector<double> PompageHoraire;
    std::vector<double> TurbinageHoraire;
    std::vector<double> TurbinageHoraireUp;
    std::vector<double> TurbinageHoraireDown;

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

struct VARIABLES_DUALES_INTERCONNEXIONS
{
    std::vector<double> VariableDualeParInterconnexion;
};

struct PROBLEME_HEBDO
{
    unsigned int weekInTheYear = 0;
    unsigned int year = 0;

    /* Business problem */
    bool OptimisationAuPasHebdomadaire = false;
    char TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
    bool WaterValueAccurate = false;
    bool OptimisationAvecCoutsDeDemarrage = false;
    int NombreDePays = 0;
    std::vector<const char*> NomsDesPays;
    int NombreDePaliersThermiques = 0;

    int NombreDInterconnexions = 0;
    std::vector<int> PaysOrigineDeLInterconnexion;
    std::vector<int> PaysExtremiteDeLInterconnexion;
    mutable std::vector<COUTS_DE_TRANSPORT> CoutDeTransport;

    mutable std::vector<VALEURS_DE_NTC_ET_RESISTANCES> ValeursDeNTC;

    unsigned int NombreDePasDeTemps = 0;
    std::vector<int> NumeroDeJourDuPasDeTemps;

    int NombreDePasDeTempsPourUneOptimisation = 0;
    std::vector<int> NumeroDIntervalleOptimiseDuPasDeTemps;
    int NombreDeJours = 0;

    int NombreDePasDeTempsDUneJournee = 0;

    mutable std::vector<CONSOMMATIONS_ABATTUES> ConsommationsAbattues;
    std::vector<CONSOMMATIONS_ABATTUES> ConsommationsAbattuesRef;

    std::vector<double> CoutDeDefaillancePositive;
    std::vector<double> CoutDeDefaillanceNegative;
    std::vector<double> CoutDeDefaillanceEnReserve;

    std::vector<PALIERS_THERMIQUES> PaliersThermiquesDuPays;
    mutable std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;

    int NumberOfShortTermStorages = 0;
    // problemeHebdo->ShortTermStorage[areaIndex][clusterIndex].capacity;
    std::vector<::ShortTermStorage::AREA_INPUT> ShortTermStorage;

    /* Optimization problem */
    int NbTermesContraintesPourLesCoutsDeDemarrage = 0;
    std::vector<bool> DefaillanceNegativeUtiliserPMinThermique;
    std::vector<bool> DefaillanceNegativeUtiliserHydro;
    std::vector<bool> DefaillanceNegativeUtiliserConsoAbattue;

    char TypeDOptimisation = OPTIMISATION_LINEAIRE; // OPTIMISATION_LINEAIRE or OPTIMISATION_QUADRATIQUE
    std::vector<std::vector<double>> BruitSurCoutHydraulique;

    int NombreDeContraintesCouplantes = 0;
    mutable std::vector<CONTRAINTES_COUPLANTES> MatriceDesContraintesCouplantes;
    std::vector<RESULTATS_CONTRAINTES_COUPLANTES> ResultatsContraintesCouplantes;

    std::vector<SOLDE_MOYEN_DES_ECHANGES> SoldeMoyenHoraire; // Used for quadratic opt
    /* Implementation details : I/O, error management, etc. */
    bool ReinitOptimisation = false;

    Data::mpsExportStatus ExportMPS = Data::mpsExportStatus::NO_EXPORT;
    bool exportMPSOnError = false;
    bool ExportStructure = false;
    bool NamedProblems = false;

    unsigned int HeureDansLAnnee = 0;
    bool LeProblemeADejaEteInstancie = false;
    bool firstWeekOfSimulation = false;

    // TODO VP: Not working if we're not using a pointer, need more investigation
    std::vector<CORRESPONDANCES_DES_VARIABLES*> CorrespondanceVarNativesVarOptim;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES> CorrespondanceCntNativesCntOptim;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES> CorrespondanceCntNativesCntOptimJournalieres;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES CorrespondanceCntNativesCntOptimHebdomadaires;

    mutable std::vector<RESERVE_JMOINS1> ReserveJMoins1;

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

    std::vector<double> previousYearFinalLevels;
    std::vector<ALL_MUST_RUN_GENERATION> AllMustRunGeneration;

    OptimizationStatistics optimizationStatistics[2];

    /* Adequacy Patch */
    std::shared_ptr<AdequacyPatchRuntimeData> adequacyPatchRuntimeData;

    /* Hydro management */
    std::vector<double> CoefficientEcretementPMaxHydraulique;
    bool hydroHotStart = false;
    std::vector<double> previousSimulationFinalLevel;

    /* Results */
    std::vector<RESULTATS_HORAIRES> ResultatsHoraires;
    std::vector<VARIABLES_DUALES_INTERCONNEXIONS> VariablesDualesDesContraintesDeNTC;

    std::vector<double> coutOptimalSolution1;
    std::vector<double> coutOptimalSolution2;

    std::vector<double> tempsResolution1;
    std::vector<double> tempsResolution2;

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

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = nullptr;

    double maxPminThermiqueByDay[366];
};
#endif
