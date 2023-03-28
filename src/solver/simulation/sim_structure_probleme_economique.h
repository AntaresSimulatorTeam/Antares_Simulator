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

typedef struct
{
    int* NumeroDeVariableDeLInterconnexion;
    int* NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion;
    int* NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion;

    int* NumeroDeVariableDuPalierThermique;

    int* NumeroDeVariablesDeLaProdHyd;

    int* NumeroDeVariablesDePompage;
    int* NumeroDeVariablesDeNiveau;
    int* NumeroDeVariablesDeDebordement;

    int* NumeroDeVariableDefaillancePositive;

    int* NumeroDeVariableDefaillanceNegative;

    int* NumeroDeVariablesVariationHydALaBaisse;

    int* NumeroDeVariablesVariationHydALaHausse;

    int* NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;

    struct
    {
        int* InjectionVariable;
        int* WithdrawalVariable;
        int* LevelVariable;
    } ShortTermStorage;
} CORRESPONDANCES_DES_VARIABLES;

typedef struct
{
    int* NumeroDeContrainteDesBilansPays;
    int* NumeroDeContraintePourEviterLesChargesFictives;

    int* NumeroPremiereContrainteDeReserveParZone;
    int* NumeroDeuxiemeContrainteDeReserveParZone;

    int* NumeroDeContrainteDeDissociationDeFlux;
    int* NumeroDeContrainteDesContraintesCouplantes;

    int* NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
    int* NumeroDeContrainteDesContraintesDeDureeMinDArret;
    int* NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne;

    int* NumeroDeContrainteDesNiveauxPays;

    int* ShortTermStorageLevelConstraint;
} CORRESPONDANCES_DES_CONTRAINTES;

typedef struct
{
    int* NumeroDeContrainteDesContraintesCouplantes;

} CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES;

typedef struct
{
    int* NumeroDeContrainteDesContraintesCouplantes;

} CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES;

typedef struct
{
    double* ValeurDeNTCOrigineVersExtremite;
    double* ValeurDeNTCExtremiteVersOrigine;
    double* ValeurDeLoopFlowOrigineVersExtremite;
    double* ValeurDuFlux;

    double* ValeurDuFluxUp;
    double* ValeurDuFluxDown;

    double* ResistanceApparente;
} VALEURS_DE_NTC_ET_RESISTANCES;

typedef struct
{
    double* TransferBoundMarketEdge;
    double* TransferLevelMarketEdge;
} TRANSFER_BOUND_AND_LEVEL_MARKET_EDGE;

typedef struct
{
    double* TransferBoundFlexUpEdge;
    double* TransferLevelFlexUpEdge;
} TRANSFER_BOUND_AND_LEVEL_FLEX_UP_EDGE;

typedef struct
{
    double* TransferBoundFlexDownEdge;
    double* TransferLevelFlexDownEdge;
} TRANSFER_BOUND_AND_LEVEL_FLEX_DOWN_EDGE;

typedef struct
{
    char TypeDeContrainteCouplante;

    char SensDeLaContrainteCouplante;

    double* SecondMembreDeLaContrainteCouplante;

    double* SecondMembreDeLaContrainteCouplanteRef;

    int NombreDElementsDansLaContrainteCouplante;
    int NombreDInterconnexionsDansLaContrainteCouplante;
    double* PoidsDeLInterconnexion;
    int* NumeroDeLInterconnexion;
    int* OffsetTemporelSurLInterco;
    int NombreDePaliersDispatchDansLaContrainteCouplante;
    double* PoidsDuPalierDispatch;
    int* PaysDuPalierDispatch;
    int* NumeroDuPalierDispatch;
    int* OffsetTemporelSurLePalierDispatch;
    const char* NomDeLaContrainteCouplante;
} CONTRAINTES_COUPLANTES;

namespace ShortTermStorage
{
struct PROPERTIES
{
    double capacity;
    double injectionCapacity;
    double withdrawalCapacity;
    double efficiency;
    std::optional<double> initialLevel;

    std::shared_ptr<Antares::Data::ShortTermStorage::Series> series;

    int globalIndex;
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

// TODO remove
// double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
//                               ->ShortTermStorage[pdtHebdo].level[index]);

typedef struct
{
    double* variablesDuales;
} RESULTATS_CONTRAINTES_COUPLANTES;

typedef struct
{
    double* TotalDemandOfMarketPool;
} DEMAND_MARKET_POOL;

typedef struct
{
    double* FosteredDemandOfFlexUpPool;
} DEMAND_FLEX_UP_POOL;

typedef struct
{
    double* FosteredDemandOfFlexDownPool;
} DEMAND_FLEX_DOWN_POOL;

typedef struct
{
    double* BoundFlexUpNode;
} BOUND_FLEX_UP_NODE;

typedef struct
{
    double* BoundFlexDownNode;
} BOUND_FLEX_DOWN_NODE;

typedef struct
{
    double* LevelFlexUpNode;
} LEVEL_FLEX_UP_NODE;

typedef struct
{
    double* LevelFlexDownNode;
} LEVEL_FLEX_DOWN_NODE;

typedef struct
{
    double* ConsommationAbattueDuPays;
} CONSOMMATIONS_ABATTUES;

typedef struct
{
    double* AllMustRunGenerationOfArea;
} ALL_MUST_RUN_GENERATION;

typedef struct
{
    double* SoldeMoyenDuPays;

} SOLDE_MOYEN_DES_ECHANGES;

typedef struct
{
    double* PuissanceDisponibleDuPalierThermique;

    double* PuissanceDisponibleDuPalierThermiqueRef;
    double* PuissanceDisponibleDuPalierThermiqueRef_SV;

    double* PuissanceMinDuPalierThermique;
    double* PuissanceMinDuPalierThermique_SV;

    double* CoutHoraireDeProductionDuPalierThermique;

    double* CoutHoraireDeProductionDuPalierThermiqueRef;

    double* CoutHoraireDuPalierThermiqueUp;
    double* CoutHoraireDuPalierThermiqueDown;

    int* NombreMaxDeGroupesEnMarcheDuPalierThermique;
    int* NombreMinDeGroupesEnMarcheDuPalierThermique;

} PDISP_ET_COUTS_HORAIRES_PAR_PALIER;

typedef struct
{
    int NombreDePaliersThermiques;

    int* minUpDownTime;

    double* TailleUnitaireDUnGroupeDuPalierThermique;
    double* PminDuPalierThermiquePendantUneHeure;
    double* PminDuPalierThermiquePendantUnJour;
    int* NumeroDuPalierDansLEnsembleDesPaliersThermiques;
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER** PuissanceDisponibleEtCout;

    double* CoutDeDemarrageDUnGroupeDuPalierThermique;
    double* CoutDArretDUnGroupeDuPalierThermique;
    double* CoutFixeDeMarcheDUnGroupeDuPalierThermique;
    double* pminDUnGroupeDuPalierThermique;
    double* PmaxDUnGroupeDuPalierThermique;
    int* DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
    int* DureeMinimaleDArretDUnGroupeDuPalierThermique;
} PALIERS_THERMIQUES;

typedef struct
{
    double* MaxEnergieHydrauParIntervalleOptimise;

    double* MinEnergieHydrauParIntervalleOptimise;

    double* CntEnergieH2OParIntervalleOptimise;
    double* CntEnergieH2OParJour;
    double* ContrainteDePmaxHydrauliqueHoraire;

    double MaxDesPmaxHydrauliques;

    double* CntEnergieH2OParIntervalleOptimiseRef;
    double* ContrainteDePmaxHydrauliqueHoraireRef;
    double MaxDesPmaxHydrauliquesRef;

    double* MaxEnergiePompageParIntervalleOptimise;
    double* ContrainteDePmaxPompageHoraire;

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

    double* NiveauHoraireSup;
    double* NiveauHoraireInf;

    double* ApportNaturelHoraire;
    double NiveauInitialReservoir;
    double TailleReservoir;
    double PumpingRatio;

    double WeeklyGeneratingModulation;
    double WeeklyPumpingModulation;
    bool DirectLevelAccess; /*  determines the type of constraints bearing on the final stok level*/
    bool AccurateWaterValue;     /*  determines the type of modelling used for water budget*/
    double LevelForTimeInterval; /*  value computed by the simulator in water-value based modes*/
    double* WaterLayerValues; /*  reference costs for the last time step (caution : dimension set to
                                 100, should be made dynamic)*/
    double* InflowForTimeInterval; /*  Energy input to the reservoir, used to in the bounding
                                      constraint on final level*/
} ENERGIES_ET_PUISSANCES_HYDRAULIQUES;

class computeTimeStepLevel
{
private:
    int step;
    double level;

    double capacity;
    double* inflows;
    double* ovf;
    double* turb;
    double pumpRatio;
    double* pump;
    double excessDown;

public:
    computeTimeStepLevel() :
     step(0),
     level(0.),
     capacity(0.),
     inflows(nullptr),
     ovf(nullptr),
     turb(nullptr),
     pumpRatio(0.),
     pump(nullptr),
     excessDown(0.)
    {
    }

    void setParameters(const double& startLvl,
                       double* Infl,
                       double* overfl,
                       double* H,
                       double pumpEff,
                       double* Pump,
                       double rc)
    {
        step = 0;
        level = startLvl;
        inflows = Infl;
        ovf = overfl;
        turb = H;
        pumpRatio = pumpEff;
        pump = Pump;
        capacity = rc;
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

typedef struct
{
    double* ReserveHoraireJMoins1 = nullptr;
    double* ReserveHoraireJMoins1Ref = nullptr;
} RESERVE_JMOINS1;

typedef struct
{
    double* ProductionThermiqueDuPalier;

    double* ProductionThermiqueDuPalierUp;
    double* ProductionThermiqueDuPalierDown;

    double* NombreDeGroupesEnMarcheDuPalier;
    double* NombreDeGroupesQuiDemarrentDuPalier;

    double* NombreDeGroupesQuiSArretentDuPalier;

    double* NombreDeGroupesQuiTombentEnPanneDuPalier;

} PRODUCTION_THERMIQUE_OPTIMALE;

typedef struct
{
    double* ValeursHorairesDeDefaillancePositive;
    double* ValeursHorairesDENS;                  // adq patch domestic unsupplied energy
    int* ValeursHorairesLmrViolations;            // adq patch lmr violations
    double* ValeursHorairesSpilledEnergyAfterCSR; // adq patch spillage after CSR
    double* ValeursHorairesDtgMrgCsr;             // adq patch DTG MRG after CSR
    double* ValeursHorairesDeDefaillancePositiveUp;
    double* ValeursHorairesDeDefaillancePositiveDown;
    double* ValeursHorairesDeDefaillancePositiveAny;

    double* ValeursHorairesDeDefaillanceNegative;
    double* ValeursHorairesDeDefaillanceNegativeUp;
    double* ValeursHorairesDeDefaillanceNegativeDown;
    double* ValeursHorairesDeDefaillanceNegativeAny;

    double* ValeursHorairesDeDefaillanceEnReserve;
    double* PompageHoraire;
    double* TurbinageHoraire;
    double* TurbinageHoraireUp;
    double* TurbinageHoraireDown;

    double* niveauxHoraires;
    double* valeurH2oHoraire;

    double* debordementsHoraires;

    double* CoutsMarginauxHoraires;
    PRODUCTION_THERMIQUE_OPTIMALE** ProductionThermique; // index is pdtHebdo

    std::vector<::ShortTermStorage::RESULTS>* ShortTermStorage = nullptr;
} RESULTATS_HORAIRES;

typedef struct
{
    bool IntercoGereeAvecDesCouts;
    bool IntercoGereeAvecLoopFlow;
    double* CoutDeTransportOrigineVersExtremite;
    double* CoutDeTransportExtremiteVersOrigine;

    double* CoutDeTransportOrigineVersExtremiteRef;
    double* CoutDeTransportExtremiteVersOrigineRef;

} COUTS_DE_TRANSPORT;

typedef struct
{
    double* VariableDualeParInterconnexion = nullptr;
} VARIABLES_DUALES_INTERCONNEXIONS;

typedef struct
{
    double* CoutsMarginauxHorairesDeLaReserveParZone = nullptr;
} COUTS_MARGINAUX_ZONES_DE_RESERVE;

struct AdequacyPatchParameters
{
    bool AdequacyFirstStep;
    bool SetNTCOutsideToInsideToZero;
    bool SetNTCOutsideToOutsideToZero;
    bool IncludeHurdleCostCsr;
    bool CheckCsrCostFunctionValue;
    Antares::Data::AdequacyPatch::AdqPatchPTO PriceTakingOrder;
    double ThresholdRunCurtailmentSharingRule;
    double ThresholdDisplayLocalMatchingRuleViolations;
    double ThresholdCSRVarBoundsRelaxation;
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
    const char** NomsDesPays = nullptr;
    int NombreDePaliersThermiques = 0;

    int NombreDInterconnexions = 0;
    int* PaysOrigineDeLInterconnexion = nullptr;
    int* PaysExtremiteDeLInterconnexion = nullptr;
    COUTS_DE_TRANSPORT** CoutDeTransport = nullptr;

    VALEURS_DE_NTC_ET_RESISTANCES** ValeursDeNTC = nullptr;
    VALEURS_DE_NTC_ET_RESISTANCES** ValeursDeNTCRef = nullptr;

    int NombreDePasDeTemps = 0;
    int NombreDePasDeTempsRef = 0;
    int* NumeroDeJourDuPasDeTemps = nullptr;

    int NombreDePasDeTempsPourUneOptimisation = 0;
    int* NumeroDIntervalleOptimiseDuPasDeTemps = nullptr;
    int NombreDeJours = 0;

    int NombreDePasDeTempsDUneJournee = 0;
    int NombreDePasDeTempsDUneJourneeRef = 0;

    CONSOMMATIONS_ABATTUES** ConsommationsAbattues = nullptr;

    CONSOMMATIONS_ABATTUES** ConsommationsAbattuesRef = nullptr;

    double* CoutDeDefaillancePositive = nullptr;
    double* CoutDeDefaillanceNegative = nullptr;
    double* CoutDeDefaillanceEnReserve = nullptr;

    PALIERS_THERMIQUES** PaliersThermiquesDuPays = nullptr;
    ENERGIES_ET_PUISSANCES_HYDRAULIQUES** CaracteristiquesHydrauliques = nullptr;

    int NumberOfShortTermStorages = 0;
    // problemeHebdo->ShortTermStorage[area_idx][ststor_idx].capacity;
    std::vector<::ShortTermStorage::AREA_INPUT>* ShortTermStorage = nullptr;

    /* Optimization problem */
    int NbTermesContraintesPourLesCoutsDeDemarrage = 0;
    bool* DefaillanceNegativeUtiliserPMinThermique = nullptr;
    bool* DefaillanceNegativeUtiliserHydro = nullptr;
    bool* DefaillanceNegativeUtiliserConsoAbattue = nullptr;

    char TypeDOptimisation = OPTIMISATION_LINEAIRE; // OPTIMISATION_LINEAIRE or OPTIMISATION_QUADRATIQUE

    double** BruitSurCoutHydraulique = nullptr;

    int NombreDeContraintesCouplantes = 0;
    CONTRAINTES_COUPLANTES** MatriceDesContraintesCouplantes = nullptr;
    RESULTATS_CONTRAINTES_COUPLANTES* ResultatsContraintesCouplantes = nullptr;

    SOLDE_MOYEN_DES_ECHANGES** SoldeMoyenHoraire = nullptr; // Used for quadratic opt
    /* Implementation details : I/O, error management, etc. */
    bool ReinitOptimisation = false;

    Data::mpsExportStatus ExportMPS = Data::mpsExportStatus::NO_EXPORT;
    bool exportMPSOnError = false;
    bool ExportStructure = false;

    unsigned int HeureDansLAnnee = 0;
    bool LeProblemeADejaEteInstancie = false;
    bool firstWeekOfSimulation = false;

    CORRESPONDANCES_DES_VARIABLES** CorrespondanceVarNativesVarOptim = nullptr;
    CORRESPONDANCES_DES_CONTRAINTES** CorrespondanceCntNativesCntOptim = nullptr;
    CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES** CorrespondanceCntNativesCntOptimJournalieres = nullptr;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES** CorrespondanceCntNativesCntOptimHebdomadaires = nullptr;

    RESERVE_JMOINS1** ReserveJMoins1 = nullptr;

    int* IndexDebutIntercoOrigine = nullptr;
    int* IndexSuivantIntercoOrigine = nullptr;
    int* IndexDebutIntercoExtremite = nullptr;
    int* IndexSuivantIntercoExtremite = nullptr;

    bool Expansion = false;

    int* NumeroDeContrainteEnergieHydraulique = nullptr;
    int* NumeroDeContrainteMinEnergieHydraulique = nullptr;
    int* NumeroDeContrainteMaxEnergieHydraulique = nullptr;
    int* NumeroDeContrainteMaxPompage = nullptr;

    int* NumeroDeContrainteDeSoldeDEchange = nullptr;

    int* NumeroDeContrainteEquivalenceStockFinal = nullptr;
    int* NumeroDeContrainteExpressionStockFinal = nullptr;

    int* NumeroDeVariableStockFinal = nullptr;
    int** NumeroDeVariableDeTrancheDeStock = nullptr;

    bool YaDeLaReserveJmoins1 = false;

    double* previousYearFinalLevels = nullptr;
    ALL_MUST_RUN_GENERATION** AllMustRunGeneration = nullptr;

    OptimizationStatistics optimizationStatistics[2];

    /* Adequacy Patch */
    std::shared_ptr<AdequacyPatchParameters> adqPatchParams;
    std::shared_ptr<AdequacyPatchRuntimeData> adequacyPatchRuntimeData;

    /* Hydro management */
    double* CoefficientEcretementPMaxHydraulique = nullptr;
    bool hydroHotStart = false;
    double* previousSimulationFinalLevel = nullptr;
    computeTimeStepLevel computeLvl_object;

    /* Results */
    RESULTATS_HORAIRES** ResultatsHoraires = nullptr;
    VARIABLES_DUALES_INTERCONNEXIONS** VariablesDualesDesContraintesDeNTC = nullptr;

    double* coutOptimalSolution1 = nullptr;
    double* coutOptimalSolution2 = nullptr;

    double* tempsResolution1 = nullptr;
    double* tempsResolution2 = nullptr;

    COUTS_MARGINAUX_ZONES_DE_RESERVE** CoutsMarginauxDesContraintesDeReserveParZone = nullptr;
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
    int* NbGrpCourbeGuide = nullptr; // ?
    int* NbGrpOpt = nullptr;         // ?

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = nullptr;

    double maxPminThermiqueByDay[366];
};
#endif
