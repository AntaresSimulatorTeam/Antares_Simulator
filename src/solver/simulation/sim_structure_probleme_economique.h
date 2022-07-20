/*
** Copyright 2007-2018 RTE
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

#include <memory>
#include <yuni/core/math.h>

using namespace Antares::Data::AdequacyPatch;

typedef struct
{
    int* NumeroDeVariableDeLInterconnexion;
    int* NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion;
    int* NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion;

    int* NumeroDeVariableDuPalierThermique;
    int* NumeroDeVariableDuPalierThermiqueUp;
    int* NumeroDeVariableDuPalierThermiqueDown;

    int* NumeroDeVariablesDeLaProdHyd;
    int* NumeroDeVariablesDeLaProdHydUp;
    int* NumeroDeVariablesDeLaProdHydDown;

    int* NumeroDeVariablesDePompage;
    int* NumeroDeVariablesDeNiveau;
    int* NumeroDeVariablesDeDebordement;

    int* NumeroDeVariableDefaillancePositive;
    int* NumeroDeVariableDefaillancePositiveUp;
    int* NumeroDeVariableDefaillancePositiveDown;
    int* NumeroDeVariableDefaillancePositiveAny;

    int* NumeroDeVariableDefaillanceNegative;
    int* NumeroDeVariableDefaillanceNegativeUp;
    int* NumeroDeVariableDefaillanceNegativeDown;
    int* NumeroDeVariableDefaillanceNegativeAny;

    int* NumeroDeVariableDefaillanceEnReserve;

    int* NumeroDeVariablesVariationHydALaBaisse;
    int* NumeroDeVariablesVariationHydALaBaisseUp;
    int* NumeroDeVariablesVariationHydALaBaisseDown;

    int* NumeroDeVariablesVariationHydALaHausse;
    int* NumeroDeVariablesVariationHydALaHausseUp;
    int* NumeroDeVariablesVariationHydALaHausseDown;

    int* NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
    int* NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;

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
    double* PminDUnGroupeDuPalierThermique;
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

    char PresenceDePompageModulable;
    char PresenceDHydrauliqueModulable;

    double PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;
    double PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;

    double WeeklyWaterValueStateRegular;
    double WeeklyWaterValueStateUp;
    double WeeklyWaterValueStateDown;

    char TurbinageEntreBornes;

    char SuiviNiveauHoraire;

    double* NiveauHoraireSup;
    double* NiveauHoraireInf;

    double* ApportNaturelHoraire;
    double NiveauInitialReservoir;
    double TailleReservoir;
    double PumpingRatio;

    double WeeklyGeneratingModulation;
    double WeeklyPumpingModulation;
    char DirectLevelAccess; /*  determines the type of constraints bearing on the final stok level*/
    char AccurateWaterValue;     /*  determines the type of modelling used for water budget*/
    double LevelForTimeInterval; /*  value computed by the simulator in water-value based modes*/
    double* WaterLayerValues; /*  reference costs for the last time step (caution : dimension set to
                                 100, should be made dynamic)*/
    double* InflowForTimeInterval; /*  Energy input to the reservoir, used to in the bounding
                                      constraint on final level*/
} ENERGIES_ET_PUISSANCES_HYDRAULIQUES;

class AdequacyPatchRuntimeData
{
private:
    using adqPatchParamsMode = Antares::Data::AdequacyPatch::AdequacyPatchMode;

public:
    std::vector<adqPatchParamsMode> areaMode;
    std::vector<adqPatchParamsMode> originAreaMode;
    std::vector<adqPatchParamsMode> extremityAreaMode;
    void initialize(Antares::Data::Study& study)
    {
        for (uint i = 0; i != study.areas.size(); ++i)
        {
            auto& area = *(study.areas[i]);
            areaMode.push_back(area.adequacyPatchMode);
        }
        for (uint i = 0; i < study.runtime->interconnectionsCount; ++i)
        {
            auto& link = *(study.runtime->areaLink[i]);
            originAreaMode.push_back(link.from->adequacyPatchMode);
            extremityAreaMode.push_back(link.with->adequacyPatchMode);
        }
    }
};

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
    double* ReserveHoraireJMoins1;

    double* ReserveHoraireJMoins1Ref;

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
    double* ValeursHorairesDENS; // adq patch domestic unsupplied energy
    int* ValeursHorairesLmrViolations; // adq patch lmr violations
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
    PRODUCTION_THERMIQUE_OPTIMALE** ProductionThermique;
} RESULTATS_HORAIRES;

typedef struct
{
    char IntercoGereeAvecDesCouts;
    char IntercoGereeAvecLoopFlow;
    double* CoutDeTransportOrigineVersExtremite;
    double* CoutDeTransportExtremiteVersOrigine;

    double* CoutDeTransportOrigineVersExtremiteRef;
    double* CoutDeTransportExtremiteVersOrigineRef;

} COUTS_DE_TRANSPORT;

typedef struct
{
    double* VariableDualeParInterconnexion;
} VARIABLES_DUALES_INTERCONNEXIONS;

typedef struct
{
    double* CoutsMarginauxHorairesDeLaReserveParZone;
} COUTS_MARGINAUX_ZONES_DE_RESERVE;

struct AdequacyPatchParameters
{
    bool AdequacyFirstStep;
    bool SetNTCOutsideToInsideToZero;
    bool SetNTCOutsideToOutsideToZero;
    bool SaveIntermediateResults;
    AdqPatchPTO PriceTakingOrder;
    float ThresholdInitiateCurtailmentSharingRule;
    float ThresholdDisplayLocalMatchingRuleViolations;
};

class HOURLY_CSR_PROBLEM;

struct PROBLEME_HEBDO
{
    /* Business problem */
    char OptimisationAuPasHebdomadaire;
    char TypeDeLissageHydraulique;
    char WaterValueAccurate; /* OUI_ANTARES /NON_ANTARES*/
    char OptimisationAvecCoutsDeDemarrage;
    int NombreDePays;
    const char** NomsDesPays;
    int NombreDePaliersThermiques;

    int NombreDInterconnexions;
    int* PaysOrigineDeLInterconnexion;
    int* PaysExtremiteDeLInterconnexion;
    COUTS_DE_TRANSPORT** CoutDeTransport;

    VALEURS_DE_NTC_ET_RESISTANCES** ValeursDeNTC;
    VALEURS_DE_NTC_ET_RESISTANCES** ValeursDeNTCRef;

    int NombreDePasDeTemps;
    int NombreDePasDeTempsRef;
    int* NumeroDeJourDuPasDeTemps;

    int NombreDePasDeTempsPourUneOptimisation;
    int* NumeroDIntervalleOptimiseDuPasDeTemps;
    int NombreDeJours;

    int NombreDePasDeTempsDUneJournee;
    int NombreDePasDeTempsDUneJourneeRef;

    CONSOMMATIONS_ABATTUES** ConsommationsAbattues;

    CONSOMMATIONS_ABATTUES** ConsommationsAbattuesRef;

    double* CoutDeDefaillancePositive;
    double* CoutDeDefaillanceNegative;
    double* CoutDeDefaillanceEnReserve;

    PALIERS_THERMIQUES** PaliersThermiquesDuPays;
    ENERGIES_ET_PUISSANCES_HYDRAULIQUES** CaracteristiquesHydrauliques;
    /* Optimization problem */
    int NbTermesContraintesPourLesCoutsDeDemarrage;
    char* DefaillanceNegativeUtiliserPMinThermique;
    char* DefaillanceNegativeUtiliserHydro;
    char* DefaillanceNegativeUtiliserConsoAbattue;

    char TypeDOptimisation; // OPTIMISATION_LINEAIRE or OPTIMISATION_QUADRATIQUE

    double** BruitSurCoutHydraulique;

    int NombreDeContraintesCouplantes;
    CONTRAINTES_COUPLANTES** MatriceDesContraintesCouplantes;

    SOLDE_MOYEN_DES_ECHANGES** SoldeMoyenHoraire; // Used for quadratic opt
    /* Implementation details : I/O, error management, etc. */
    char ReinitOptimisation;

    char ExportMPS;
    bool SplitExportedMPS;
    bool exportMPSOnError;
    bool ExportStructure;

    unsigned int HeureDansLAnnee;
    char LeProblemeADejaEteInstancie;
    bool firstWeekOfSimulation;

    CORRESPONDANCES_DES_VARIABLES** CorrespondanceVarNativesVarOptim;
    CORRESPONDANCES_DES_CONTRAINTES** CorrespondanceCntNativesCntOptim;
    CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES** CorrespondanceCntNativesCntOptimJournalieres;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES** CorrespondanceCntNativesCntOptimHebdomadaires;

    RESERVE_JMOINS1** ReserveJMoins1;

    int* IndexDebutIntercoOrigine;
    int* IndexSuivantIntercoOrigine;
    int* IndexDebutIntercoExtremite;
    int* IndexSuivantIntercoExtremite;

    char Expansion;

    int* NumeroDeContrainteEnergieHydraulique;
    int* NumeroDeContrainteMinEnergieHydraulique;
    int* NumeroDeContrainteMaxEnergieHydraulique;
    int* NumeroDeContrainteMaxPompage;

    int* NumeroDeContrainteDeSoldeDEchange;

    int* NumeroDeContrainteBorneStockFinal;
    int* NumeroDeContrainteEquivalenceStockFinal;
    int* NumeroDeContrainteExpressionStockFinal;

    int* NumeroDeVariableStockFinal;
    int** NumeroDeVariableDeTrancheDeStock;

    int* numeroOptimisation;

    char YaDeLaReserveJmoins1;
    char ContrainteDeReserveJMoins1ParZone;
    int NombreDeZonesDeReserveJMoins1;
    int* NumeroDeZoneDeReserveJMoins1;

    double* previousYearFinalLevels;
    ALL_MUST_RUN_GENERATION** AllMustRunGeneration;

    /* Adequacy Patch */
    std::unique_ptr<AdequacyPatchParameters> adqPatchParams = nullptr;
    AdequacyPatchRuntimeData adequacyPatchRuntimeData;

    optimizationStatistics optimizationStatistics_object;
    /* Hydro management */
    double* CoefficientEcretementPMaxHydraulique;
    bool hydroHotStart;
    double* previousSimulationFinalLevel;
    computeTimeStepLevel computeLvl_object;

    /* Results */
    RESULTATS_HORAIRES** ResultatsHoraires;
    VARIABLES_DUALES_INTERCONNEXIONS** VariablesDualesDesContraintesDeNTC;

    double* coutOptimalSolution1;
    double* coutOptimalSolution2;

    COUTS_MARGINAUX_ZONES_DE_RESERVE** CoutsMarginauxDesContraintesDeReserveParZone;
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
    int* NbGrpCourbeGuide; // ?
    int* NbGrpOpt;         // ?

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    double maxPminThermiqueByDay[366];
};

// hourly CSR problem structure
class HOURLY_CSR_PROBLEM
{
private:
    void calculateCsrParameters();
    void resetProblem();
    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem(uint week, int year);
public:
    void run(uint week, int year);
public:
    int hourInWeekTriggeredCsr;
    PROBLEME_HEBDO* pWeeklyProblemBelongedTo;
    HOURLY_CSR_PROBLEM(int hourInWeek, PROBLEME_HEBDO* pProblemeHebdo)
    {
        hourInWeekTriggeredCsr = hourInWeek;
        pWeeklyProblemBelongedTo = pProblemeHebdo;
    };
    std::map<int, int> numberOfConstraintCsrEns;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
    std::map<int, int> numberOfConstraintCsrFlowDissociation;
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> densNewValues;
    std::map<int, double> rhsAreaBalanceValues;
};

#endif
