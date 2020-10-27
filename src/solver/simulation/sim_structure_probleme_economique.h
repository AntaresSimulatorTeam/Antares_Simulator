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
# define __SOLVER_SIMULATION_ECO_STRUCTS_H__

# include "../optimisation/opt_structure_probleme_a_resoudre.h"

# define GROSSES_VARIABLES  NON_ANTARES 
# define COEFF_GROSSES_VARIABLES 100

# include <yuni/core/math.h>	



typedef enum {
	Classe01 =  1 , Classe02 =  2 , Classe03 =  3 , Classe04 =  4 , Classe05 =  5 , Classe06 =  6 ,
	Classe07 =  7 , Classe08 =  8 , Classe09 =  9 , Classe10 = 10 , Classe11 = 11 , Classe12 = 12 ,
	Classe13 = 13 , Classe14 = 14 , Classe15 = 15 , Classe16 = 16 , Classe17 = 17 , Classe18 = 18 ,
	Classe19 = 19 , Classe20 = 20 , Classe21 = 21 , Classe22 = 22 , Classe23 = 23 , Classe24 = 24 ,
	Classe25 = 25 , Classe26 = 26 , Classe27 = 27 , Classe28 = 28 , Classe29 = 29 , Classe30 = 30 ,
	Classe31 = 31 , Classe32 = 32 , Classe33 = 33 , Classe34 = 34 , Classe35 = 35 , Classe36 = 36 ,
	Classe37 = 37 , Classe38 = 38 , Classe39 = 39 , Classe40 = 40 , Classe41 = 41 , Classe42 = 42 ,
	Classe43 = 43 , Classe44 = 44 , Classe45 = 45 , Classe46 = 46 , Classe47 = 47 , Classe48 = 48 ,
	Classe49 = 49 , Classe50 = 50 , Classe51 = 51 , Classe52 = 52 , Classe53 = 53 , Classe54 = 54 ,
	Classe55 = 55 , Classe56 = 56 , Classe57 = 57 , Classe58 = 58 , Classe59 = 59 , Classe60 = 60 ,
	Classe61 = 61 , Classe62 = 62 , Classe63 = 63 , Classe64 = 64 , Classe65 = 65 , Classe66 = 66 ,
	Classe67 = 67 , Classe68 = 68 , Classe69 = 69 , Classe70 = 70 , Classe71 = 71 , Classe72 = 72 ,
	Classe73 = 73 , Classe74 = 74 , Classe75 = 75 , Classe76 = 76 , Classe77 = 77 , Classe78 = 78 ,
	Classe79 = 79 , Classe80 = 80 , Classe81 = 81 , Classe82 = 82 , Classe83 = 83 , Classe84 = 84 ,
	Classe85 = 85 , Classe86 = 86 , Classe87 = 87 , Classe88 = 88 , Classe89 = 89 , Classe90 = 90 ,
	Classe91 = 91 , Classe92 = 92 , Classe93 = 93 , Classe94 = 94 , Classe95 = 95 , Classe96 = 96 ,
	Classe97 = 97 , Classe98 = 98 , Classe99 = 99 , Classe100 = 100, Classe101 = 101 , Classe102 = 102 ,
	Classe103 = 103 , Classe104 = 104 , Classe105 = 105 , Classe106 = 106 , Classe107 = 107 , Classe108 = 108 ,
	Classe109 = 109 , Classe110 = 110 , Classe111 = 111 , Classe112 = 112 , Classe113 = 113 , Classe114 = 114 ,
	Classe115 = 115 , Classe116 = 116 , Classe117 = 117 , Classe118 = 118 , Classe119 = 119 , Classe120 = 120 ,
	Classe121 = 121 , Classe122 = 122 , Classe123 = 123 , Classe124 = 124 , Classe125 = 125 , Classe126 = 126 ,
	Classe127 = 127 , Classe128 = 128 , Classe129 = 129 , Classe130 = 130 , Classe131 = 131 , Classe132 = 132 ,
	Classe133 = 133 , Classe134 = 134 , Classe135 = 135 , Classe136 = 136 , Classe137 = 137 , Classe138 = 138 ,
	Classe139 = 139 , Classe140 = 140 , Classe141 = 141 , Classe142 = 142 , Classe143 = 143 , Classe144 = 144 ,
	Classe145 = 145 , Classe146 = 146 , Classe147 = 147 , Classe148 = 148 , Classe149 = 149 , Classe150 = 150 ,
	Classe151 = 151 , Classe152 = 152 , Classe153 = 153 , Classe154 = 154 , Classe155 = 155 , Classe156 = 156 ,
	Classe157 = 157 , Classe158 = 158 , Classe159 = 159 , Classe160 = 160 , Classe161 = 161 , Classe162 = 162 ,
	Classe163 = 163 , Classe164 = 164 , Classe165 = 165 , Classe166 = 166 , Classe167 = 167 , Classe168 = 168
}
CLASSE_DE_MANOEUVRABILITE;




typedef struct {
	int * NumeroDeVariableDeLInterconnexion;    
	int * NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion; 
	int * NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion; 
		
	int * NumeroDeVariableDuPalierThermique;    
	int * NumeroDeVariableDuPalierThermiqueUp;								
	int * NumeroDeVariableDuPalierThermiqueDown;							
	
	int * NumeroDeVariablesDeLaProdHyd;         
	int * NumeroDeVariablesDeLaProdHydUp;									
	int * NumeroDeVariablesDeLaProdHydDown;									

	int * NumeroDeVariablesDePompage;			
	int * NumeroDeVariablesDeNiveau;			
	int * NumeroDeVariablesDeDebordement;		

	int * NumeroDeVariableDefaillancePositive;  
	int * NumeroDeVariableDefaillancePositiveUp;							
	int * NumeroDeVariableDefaillancePositiveDown;							
	int * NumeroDeVariableDefaillancePositiveAny;							
	

	int * NumeroDeVariableDefaillanceNegative;  
	int * NumeroDeVariableDefaillanceNegativeUp;							
	int * NumeroDeVariableDefaillanceNegativeDown;							
	int * NumeroDeVariableDefaillanceNegativeAny;							


	int * NumeroDeGrosseVariableDefaillancePositive;  
	int * NumeroDeGrosseVariableDefaillanceNegative;  
	int * NumeroDeVariableDefaillanceEnReserve; 
	int * NumeroDeGrosseVariableDefaillanceEnReserve; 
	
	
	int * NumeroDeVariablesVariationHydALaBaisse;
	int * NumeroDeVariablesVariationHydALaBaisseUp;							
	int * NumeroDeVariablesVariationHydALaBaisseDown;						

	int * NumeroDeVariablesVariationHydALaHausse;
	int * NumeroDeVariablesVariationHydALaHausseUp;							
	int * NumeroDeVariablesVariationHydALaHausseDown;						


	
	
	int *	NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
	int *	NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
	int *	NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
	int *	NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;

	
		
																			
		
	
																			
} CORRESPONDANCES_DES_VARIABLES;


typedef struct {
	int * NumeroDeContrainteDesBilansPays;                
	int * NumeroDeContraintePourEviterLesChargesFictives; 	
	        

	int * NumeroPremiereContrainteDeReserveParZone; 
	int * NumeroDeuxiemeContrainteDeReserveParZone; 

	int * NumeroDeContrainteDeDissociationDeFlux;     
	int * NumeroDeContrainteDesContraintesCouplantes; 

	
	int * NumeroDeContrainteDesContraintesDeDureeMinDeMarche; 
	int * NumeroDeContrainteDesContraintesDeDureeMinDArret; 
	int * NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne; 	
		
	int * NumeroDeContrainteDesNiveauxPays;    

	

} CORRESPONDANCES_DES_CONTRAINTES;


typedef struct {
	int * NumeroDeContrainteDesContraintesCouplantes; 

} CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES;


typedef struct {
	int * NumeroDeContrainteDesContraintesCouplantes; 

} CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES;


typedef struct {
	double * ValeurDeNTCOrigineVersExtremite;
	double * ValeurDeNTCExtremiteVersOrigine;
	double * ValeurDeLoopFlowOrigineVersExtremite;
	double * ValeurDuFlux;
																	
	double * ValeurDuFluxUp;										
	double * ValeurDuFluxDown;										

	double * ResistanceApparente;									
} VALEURS_DE_NTC_ET_RESISTANCES;										
	
typedef struct {													
	double * TransferBoundMarketEdge;								
	double * TransferLevelMarketEdge;								  
} TRANSFER_BOUND_AND_LEVEL_MARKET_EDGE;							    	
	
																
typedef struct {													
	double * TransferBoundFlexUpEdge;								
	double * TransferLevelFlexUpEdge;								  
} TRANSFER_BOUND_AND_LEVEL_FLEX_UP_EDGE;								
	
																
typedef struct {													
	double * TransferBoundFlexDownEdge;								
	double * TransferLevelFlexDownEdge;								  
} TRANSFER_BOUND_AND_LEVEL_FLEX_DOWN_EDGE;								
	


typedef struct {

	char     TypeDeContrainteCouplante; 


	char     SensDeLaContrainteCouplante; 



  double * SecondMembreDeLaContrainteCouplante; 


  
  double * SecondMembreDeLaContrainteCouplanteRef; 


  
 
  int		NombreDElementsDansLaContrainteCouplante;				
  int		NombreDInterconnexionsDansLaContrainteCouplante;
  double *	PoidsDeLInterconnexion;
  int *		NumeroDeLInterconnexion;
  int *		OffsetTemporelSurLInterco;
  int		NombreDePaliersDispatchDansLaContrainteCouplante;		
  double *	PoidsDuPalierDispatch;									
  int *		PaysDuPalierDispatch;									
  int *		NumeroDuPalierDispatch;									
  int *		OffsetTemporelSurLePalierDispatch;						
} CONTRAINTES_COUPLANTES;


typedef struct {													
  double * TotalDemandOfMarketPool;									
} DEMAND_MARKET_POOL;												

typedef struct {													
  double * FosteredDemandOfFlexUpPool;								
} DEMAND_FLEX_UP_POOL;												

typedef struct {													
  double * FosteredDemandOfFlexDownPool;							
} DEMAND_FLEX_DOWN_POOL;											
																		  

typedef struct {													
  double * BoundFlexUpNode;											
} BOUND_FLEX_UP_NODE;												

typedef struct {													
  double * BoundFlexDownNode;										
} BOUND_FLEX_DOWN_NODE;												


typedef struct {													
																	
  double * LevelFlexUpNode;											
} LEVEL_FLEX_UP_NODE;												

typedef struct {													
																	
  double * LevelFlexDownNode;									    
} LEVEL_FLEX_DOWN_NODE;												


 

typedef struct {
  double * ConsommationAbattueDuPays; 
} CONSOMMATIONS_ABATTUES;

																	
																	
typedef struct {													
  double * AllMustRunGenerationOfArea; 		
} ALL_MUST_RUN_GENERATION;											
																	
																	
																	
																	
																	




typedef struct {
  double * SoldeMoyenDuPays; 
																	
} SOLDE_MOYEN_DES_ECHANGES;


typedef struct {
  double * PuissanceDisponibleDuPalierThermique;     
  
  double * PuissanceDisponibleDuPalierThermiqueRef;
  double * PuissanceDisponibleDuPalierThermiqueRef_SV; 

  
  double * PuissanceMinDuPalierThermique;      
  double * PuissanceMinDuPalierThermique_SV;   

  double * CoutHoraireDeProductionDuPalierThermique; 
  
  double * CoutHoraireDeProductionDuPalierThermiqueRef;

  
  double * CoutHoraireDuPalierThermiqueUp;						    
  double * CoutHoraireDuPalierThermiqueDown;						

  
	


  int * NombreMaxDeGroupesEnMarcheDuPalierThermique;
  int * NombreMinDeGroupesEnMarcheDuPalierThermique;
																														
} PDISP_ET_COUTS_HORAIRES_PAR_PALIER;

typedef struct {
  int      NombreDePaliersThermiques;
  
  CLASSE_DE_MANOEUVRABILITE * ClasseDeManoeuvrabilite; 
  
  double * TailleUnitaireDUnGroupeDuPalierThermique;
  double * PminDuPalierThermiquePendantUneHeure;
  double * PminDuPalierThermiquePendantUnJour;
  double * PminDuPalierThermiquePendantUneSemaine;
  int *    NumeroDuPalierDansLEnsembleDesPaliersThermiques;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout; 
  
	
 	double * CoutDeDemarrageDUnGroupeDuPalierThermique;
	double * CoutDArretDUnGroupeDuPalierThermique;
	double * CoutFixeDeMarcheDUnGroupeDuPalierThermique;
	double * PminDUnGroupeDuPalierThermique;
	double * PmaxDUnGroupeDuPalierThermique;
	int *    DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
	int *    DureeMinimaleDArretDUnGroupeDuPalierThermique;
} PALIERS_THERMIQUES;


typedef struct {
 
  
																	
	double * MaxEnergieHydrauParIntervalleOptimise;					 
																	
	double * MinEnergieHydrauParIntervalleOptimise;					
																	
																	
	double * CntEnergieH2OParIntervalleOptimise;						
	double * CntEnergieH2OParJour;									    
	double * ContrainteDePmaxHydrauliqueHoraire;
  
	double   MaxDesPmaxHydrauliques;	

	
	double * CntEnergieH2OParIntervalleOptimiseRef;					
	double * ContrainteDePmaxHydrauliqueHoraireRef;					
	double   MaxDesPmaxHydrauliquesRef;								
	

   
	double * MaxEnergiePompageParIntervalleOptimise;					
	double * ContrainteDePmaxPompageHoraire;							
	
	

	char     PresenceDePompageModulable;								
	char     PresenceDHydrauliqueModulable; 

	double   PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;
	double   PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;
	
	double	 WeeklyWaterValueStateRegular;								
	double	 WeeklyWaterValueStateUp;									
	double	 WeeklyWaterValueStateDown;									
																		
																		

	char	TurbinageEntreBornes;										
																		
																		
	char	SuiviNiveauHoraire;											
																		
	
																		
	double * NiveauHoraireSup;											
	double * NiveauHoraireInf;											

																		
	double * ApportNaturelHoraire;										
	double   NiveauInitialReservoir;									
	double   TailleReservoir;											
	double	 PumpingRatio;												

	double	 WeeklyGeneratingModulation;								
	double	 WeeklyPumpingModulation;	
	char	 DirectLevelAccess;		/*  determines the type of constraints bearing on the final stok level*/  
	char	 AccurateWaterValue;	/*  determines the type of modelling used for water budget*/
	double   LevelForTimeInterval;	/*  value computed by the simulator in water-value based modes*/
	double * WaterLayerValues;		/*  reference costs for the last time step (caution : dimension set to 100, should be made dynamic)*/
	double * InflowForTimeInterval;	/*  Energy input to the reservoir, used to in the bounding constraint on final level*/   
} ENERGIES_ET_PUISSANCES_HYDRAULIQUES;


class computeTimeStepLevel
{
	private:
		
		int step;			
		double level;		

		double capacity;
		double * inflows;		
		double * ovf;			
		double * turb;			
		double pumpRatio;		
		double * pump;			
		double excessDown;

	public:
		
		computeTimeStepLevel()
			: step(0), level(0.), capacity(0.), inflows(nullptr), ovf(nullptr), turb(nullptr), pumpRatio(0.), pump(nullptr), excessDown(0.)
		{}

		void setParameters(const double & startLvl, double * Infl, double * overfl, double * H, double pumpEff, double * Pump, double rc)
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

			if (Yuni::Math::Zero(level))
				level = 0.;
			
			if (Yuni::Math::Zero(1. - level))
				level = capacity;

			
			
			
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

		double getLevel() { return level; }
};



typedef struct {
  double * ReserveHoraireJMoins1;
  
  double * ReserveHoraireJMoins1Ref;
  
} RESERVE_JMOINS1;










typedef struct {
  double * ProductionThermiqueDuPalier; 
	




  double * ProductionThermiqueDuPalierUp;					 
  double * ProductionThermiqueDuPalierDown;					

  			
	
  double * NombreDeGroupesEnMarcheDuPalier; 
  double * NombreDeGroupesQuiDemarrentDuPalier; 

  double * NombreDeGroupesQuiSArretentDuPalier; 

  double * NombreDeGroupesQuiTombentEnPanneDuPalier; 

	
} PRODUCTION_THERMIQUE_OPTIMALE;


typedef struct {
  double * ValeursHorairesDeDefaillancePositive;
  double * ValeursHorairesDeDefaillancePositiveUp;			 
  double * ValeursHorairesDeDefaillancePositiveDown;		
  double * ValeursHorairesDeDefaillancePositiveAny;			
    
  double * ValeursHorairesDeDefaillanceNegative;
  double * ValeursHorairesDeDefaillanceNegativeUp;			 
  double * ValeursHorairesDeDefaillanceNegativeDown;		
  double * ValeursHorairesDeDefaillanceNegativeAny;			
 
  double * ValeursHorairesDeDefaillanceEnReserve;  	
  double * PompageHoraire;									
  double * TurbinageHoraire;
  double * TurbinageHoraireUp;								 
  double * TurbinageHoraireDown;							

  
  double * niveauxHoraires;
  double * valeurH2oHoraire;	
								
  double * debordementsHoraires;
  
  
  double * CoutsMarginauxHoraires;
  PRODUCTION_THERMIQUE_OPTIMALE ** ProductionThermique;
} RESULTATS_HORAIRES;


typedef struct {
	char     IntercoGereeAvecDesCouts; 
	char     IntercoGereeAvecLoopFlow;
  double * CoutDeTransportOrigineVersExtremite; 
  double * CoutDeTransportExtremiteVersOrigine; 
	 
  
  double * CoutDeTransportOrigineVersExtremiteRef;
  double * CoutDeTransportExtremiteVersOrigineRef;
															 
} COUTS_DE_TRANSPORT;


typedef struct {
	double * VariableDualeParInterconnexion;
} VARIABLES_DUALES_INTERCONNEXIONS;


typedef struct {
  double * CoutsMarginauxHorairesDeLaReserveParZone;  
} COUTS_MARGINAUX_ZONES_DE_RESERVE;







struct PROBLEME_HEBDO {

	





	char OptimisationAuPasHebdomadaire;

	
	
	
		
	char ReinitOptimisation; 
															
	char SecondeOptimisationRelaxee;						
															
															

	char MarketPoolActivated;								
	char FlexUpPoolActivated;								
	char FlexDownPoolActivated;								



															
	char ImpressionDuCritere;								
															
															
															
	char UnitCommitmentExact;								

												
	




	char TypeDeLissageHydraulique;

	





	char OptimisationMUTetMDT;

	
	char ExportMPS;
	bool exportMPSOnError;
	bool ExportStructure;
	
	char WaterValueAccurate;	/* OUI_ANTARES /NON_ANTARES*/ 
	
 	char OptimisationAvecCoutsDeDemarrage; 
	int NbTermesContraintesPourLesCoutsDeDemarrage;

	






  

	char BorneDeLaDefaillancePositive;
	
	char* DefaillanceNegativeUtiliserPMinThermique;
	
	char* DefaillanceNegativeUtiliserHydro;
	
	char* DefaillanceNegativeUtiliserConsoAbattue;

	








	double* CoefficientEcretementPMaxHydraulique;

	










	double** BruitSurCoutHydraulique;

	


	unsigned int HeureDansLAnnee;

	char LeProblemeADejaEteInstancie;   
	char TypeDOptimisation;             
	char SolveurDuProblemeLineaire;

	

												
	int    NombreDePays;
	const char ** NomsDesPays;
	int    NombreDePaliersThermiques;

												
	int		NumberOfMarketOfferPool;			
	int		NumberOfMarketDemandPool;			
	int		NumberOfFlexUpOfferPool;			
	int		NumberOfFlexUpDemandPool;			
	int		NumberOfFlexDownOfferPool;			
	int		NumberOfFlexDownDemandPool;			

												
	int	*	MarketOfferPoolOfNode;				
	int	*	MarketDemandPoolOfNode;					
	int	*	FlexUpOfferPoolOfNode;					
	int	*	FlexUpDemandPoolOfNode;					
	int	*	FlexDownOfferPoolOfNode;				
	int	*	FlexDownDemandPoolOfNode;			

												
	int **  IsInMarketOfferPool;				
	int **  IsInMarketDemandPool;				
	int **  IsInFlexUpOfferPool;				
	int **  IsInFlexUpDemandPool;				
	int **  IsInFlexDownOfferPool;				
	int **  IsInFlexDownDemandPool;				


	int    NumberOfMarketEdge;					
	int *  OfferPoolOfMarketEdge;				
	int *  DemandPoolOfMarketEdge;				
												
	int * IndexFirstMarketEdgeOffer;			
	int * IndexNextMarketEdgeOffer;				
	int * IndexFirstMarketEdgeDemand;			
	int * IndexNextMarketEdgeDemand;			
		
	int    NumberOfFlexUpEdge;					
	int *  OfferPoolOfFlexUpEdge;				
	int *  DemandPoolOfFlexUpEdge;				
												
	int * IndexFirstFlexUpEdgeOffer;			
	int * IndexNextFlexUpEdgeOffer;				
	int * IndexFirstFlexUpEdgeDemand;			
	int * IndexNextFlexUpEdgeDemand;			


	int    NumberOfFlexDownEdge;				
	int *  OfferPoolOfFlexDownEdge;				
	int *  DemandPoolOfFlexDownEdge;			
												
	int * IndexFirstFlexDownEdgeOffer;			
	int * IndexNextFlexDownEdgeOffer;			
	int * IndexFirstFlexDownEdgeDemand;			
	int * IndexNextFlexDownEdgeDemand;			
	

												
	double **	MarketPoolQuota;				
												
												
	double **	FlexUpPoolQuota;				
												
												
	double **	FlexDownPoolQuota;				
												
												
												
												
	TRANSFER_BOUND_AND_LEVEL_MARKET_EDGE		** DataMarketEdges;		
	TRANSFER_BOUND_AND_LEVEL_FLEX_UP_EDGE		** DataFlexUpEdges;		
	TRANSFER_BOUND_AND_LEVEL_FLEX_DOWN_EDGE		** DataFlexDownEdges;	
	
	DEMAND_MARKET_POOL							** DemandMarketPool;	
	DEMAND_FLEX_UP_POOL							** DemandFlexUpPool;		
	DEMAND_FLEX_DOWN_POOL						** DemandFlexDownPool;		
	BOUND_FLEX_UP_NODE							** BoundFlexUpNode;		
	BOUND_FLEX_DOWN_NODE						** BoundFlexDownNode;	
	LEVEL_FLEX_UP_NODE							** LevelFlexUpNode;		
	LEVEL_FLEX_DOWN_NODE						** LevelFlexDownNode;	
				


	
	int   NombreDInterconnexions;
	int * PaysOrigineDeLInterconnexion;
	int * PaysExtremiteDeLInterconnexion;
	COUTS_DE_TRANSPORT ** CoutDeTransport;
	
	int * IndexDebutIntercoOrigine;     
	int * IndexSuivantIntercoOrigine;   
	int * IndexDebutIntercoExtremite;   
	int * IndexSuivantIntercoExtremite; 

			

		

	
	
	int   NombreDeClassesDeManoeuvrabiliteActives;
	CLASSE_DE_MANOEUVRABILITE * ClasseDeManoeuvrabiliteActive; 
	
	  
	
	int   NombreDePasDeTemps;
	
	int   NombreDePasDeTempsRef;
	
	int * NumeroDeJourDuPasDeTemps;
	
	int   NombreDePasDeTempsPourUneOptimisation;
	int * NumeroDIntervalleOptimiseDuPasDeTemps;
	int   NombreDeJours;
	char  YaDeLaReserveJmoins1; 
	char  Expansion;				
	
	
	int * NbGrpCourbeGuide; 
	int * NbGrpOpt;         
	
	VALEURS_DE_NTC_ET_RESISTANCES ** ValeursDeNTC;
	
	VALEURS_DE_NTC_ET_RESISTANCES ** ValeursDeNTCRef;
	

	
	int NombreDeContraintesCouplantes;
	CONTRAINTES_COUPLANTES ** MatriceDesContraintesCouplantes;

	
	CONSOMMATIONS_ABATTUES ** ConsommationsAbattues;
	
	CONSOMMATIONS_ABATTUES ** ConsommationsAbattuesRef;
	

	
	ALL_MUST_RUN_GENERATION ** AllMustRunGeneration;
	

	
	SOLDE_MOYEN_DES_ECHANGES ** SoldeMoyenHoraire;

	
	PALIERS_THERMIQUES ** PaliersThermiquesDuPays; 

	
	ENERGIES_ET_PUISSANCES_HYDRAULIQUES ** CaracteristiquesHydrauliques; 

	computeTimeStepLevel computeLvl_object;

	
	double * previousSimulationFinalLevel;

	
	
	bool hydroHotStart;
	
	
	double * previousYearFinalLevels;


	
	 
	
	char ContrainteDeReserveJMoins1ParZone;		
	int NombreDeZonesDeReserveJMoins1;			
												
	int * NumeroDeZoneDeReserveJMoins1;			
	COUTS_MARGINAUX_ZONES_DE_RESERVE ** CoutsMarginauxDesContraintesDeReserveParZone; 

	
	
	
	
	
	
	

	RESERVE_JMOINS1 ** ReserveJMoins1;  

	
																    
																	
	double * CoutDeDefaillancePositive;
	double * CoutDeDefaillanceNegative;
	double * CoutDeDefaillanceEnReserve ;

	
	int NombreDePasDeTempsDUneJournee;
	
	int NombreDePasDeTempsDUneJourneeRef;
	

	CORRESPONDANCES_DES_VARIABLES **   CorrespondanceVarNativesVarOptim;     
	CORRESPONDANCES_DES_CONTRAINTES ** CorrespondanceCntNativesCntOptim;     
	CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES  ** CorrespondanceCntNativesCntOptimJournalieres;  
	CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES ** CorrespondanceCntNativesCntOptimHebdomadaires; 

	int * NumeroDeContrainteEnergieHydraulique; 
	int * NumeroDeContrainteMinEnergieHydraulique;							
	int * NumeroDeContrainteMaxEnergieHydraulique;							
	int * NumeroDeContrainteMaxPompage;		



	

	int * NumeroDeContrainteDeSoldeDEchange;    

	int * NumeroDeContrainteBorneStockFinal;		
	int * NumeroDeContrainteEquivalenceStockFinal;	
	int * NumeroDeContrainteExpressionStockFinal;	

	int *   NumeroDeVariableStockFinal;				
	int	**	NumeroDeVariableDeTrancheDeStock;		
	
	RESULTATS_HORAIRES ** ResultatsHoraires; 
	
	VARIABLES_DUALES_INTERCONNEXIONS ** VariablesDualesDesContraintesDeNTC;

	
	int * numeroOptimisation;
	
	double * coutOptimalSolution1;
	
	double * coutOptimalSolution2;


	
	
	class  MatriceContraintes
	{
	public:
		
		
		
		MatriceContraintes();
		
		~MatriceContraintes();

		double* pi;
		int* columns;

		


		void reserve(unsigned int c);

	private:
		unsigned int pCapacity;
	};

	MatriceContraintes  matriceContraintes;

	PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;

	class Manoeuvrabilite
	{
	public:
		
		
		
		Manoeuvrabilite();
		
		~Manoeuvrabilite();
		

		double* sumOversE;
		double* sumEversO;
		double* sum;

		


		void reserve(unsigned int nbAreas, unsigned int nbLinks);

	}; 

	Manoeuvrabilite manoeuvrabilite;

	
	double maxPminThermiqueByDay[366];

}; 



#endif 
