/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
/***********************************************************************

   FONCTION: 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"  

# include "lu_define.h"
# include "lu_fonctions.h"

# define SEUIL_CARRE_DE_NORME_TROP_FAIBLE  1.e-4 /*1.e-4*/
# define DUAL_POIDS_INITIAL 1.0

# define VERBOSE_SPX 0

/*----------------------------------------------------------------------------*/
void SPX_InitDualPoids( PROBLEME_SPX * Spx )
{
int Var; int Cnt; char * PositionDeLaVariable; char * InDualFramework;
double * DualPoids; double * Tau;

# if VERBOSE_SPX
  printf("SPX_InitDualPoids\n");
# endif

PositionDeLaVariable = Spx->PositionDeLaVariable;
InDualFramework      = Spx->InDualFramework;

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) InDualFramework[Var] = OUI_SPX;
  else                                              InDualFramework[Var] = NON_SPX;
}

DualPoids = Spx->DualPoids;
Tau       = Spx->Tau ;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) { 
  DualPoids[Cnt] = DUAL_POIDS_INITIAL;
  Tau      [Cnt] = 0.0;
}

Spx->LeSteepestEdgeEstInitilise = OUI_SPX;

Spx->CalculTauEnHyperCreux         = OUI_SPX;
Spx->CalculTauEnHyperCreuxPossible = OUI_SPX;
Spx->CountEchecsTau                = 0;                
Spx->AvertissementsEchecsTau       = 0;
Spx->NbEchecsTau                   = 0;
 
return;
}

/*----------------------------------------------------------------------------*/
void SPX_ResetReferenceSpace( PROBLEME_SPX * Spx ) 
{
char A1; char A2;

A1 = Spx->PositionDeLaVariable[Spx->VariableSortante];
A2 = Spx->PositionDeLaVariable[Spx->VariableEntrante];

Spx->PositionDeLaVariable[Spx->VariableSortante] = HORS_BASE_SUR_BORNE_INF;
Spx->PositionDeLaVariable[Spx->VariableEntrante] = EN_BASE_LIBRE;

SPX_InitDualPoids( Spx );

Spx->PositionDeLaVariable[Spx->VariableSortante] = A1;
Spx->PositionDeLaVariable[Spx->VariableEntrante] = A2;

return;
}

/*----------------------------------------------------------------------------*/
void SPX_DualSteepestGestionIndicateursHyperCreux( PROBLEME_SPX * Spx, char ResolutionEnHyperCreux,
                                                   char HyperCreuxInitial, char TypeDeSortie,
																									 char * StockageDeTau, char * ResetRefSpace ) 
{
int il; int i; char * InDualFramework; int * NumerosDesVariablesHorsBase;
if ( ResolutionEnHyperCreux == OUI_SPX ) {
  if ( TypeDeSortie == VECTEUR_LU ) {		
    *StockageDeTau = VECTEUR_SPX;
    /* Ca s'est pas bien passe et on s'est forcement retrouve en VECTEUR_LU */
		Spx->NbEchecsTau++;			
		/*printf("Echec hyper creux Tau iteration %d\n",Spx->Iteration);*/
		if ( Spx->NbEchecsTau >= SEUIL_ECHEC_CREUX_STEEPEST  ) {
      # if VERBOSE_SPX
		    printf("Arret de l'hyper creux pour le steepest edge, iteration %d\n",Spx->Iteration);
	    # endif
		  Spx->CalculTauEnHyperCreux = NON_SPX;
		  Spx->CountEchecsTau = 0;
		}			
  }
	else Spx->NbEchecsTau = 0;
}
/* Si on est pas en hyper creux, on essaie d'y revenir */
if ( HyperCreuxInitial == NON_SPX ) {
  if ( Spx->CalculABarreSEnHyperCreuxPossible == OUI_SPX ) {
    if ( Spx->CalculTauEnHyperCreuxPossible == OUI_SPX ) {
      if ( Spx->CalculTauEnHyperCreux == NON_SPX ) {
        SPX_TenterRestaurationCalculTauEnHyperCreux( Spx );
				
        /* Test: on n'arrive plus a faire de l'hyper creux alors que ce serait possible: on fait un reset
					 de l'espace de reference pour ameliorer le creux du vecteur Tau */
				InDualFramework = Spx->InDualFramework;
				NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
				if ( Spx->CalculTauEnHyperCreuxPossible == NON_SPX ) {
					if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE && Spx->CalculABarreSEnHyperCreux == OUI_SPX &&
							 Spx->CalculErBMoinsUnEnHyperCreux == OUI_SPX ) {
					  /* On compte les variable HB dans le framework. S'il y en a beaucoup alors il y a peu de chances
							 qu'on puisse faire de l'hyper creux dans le steepest edge */
            for ( il = 0 , i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
              if ( InDualFramework[NumerosDesVariablesHorsBase[i]] == OUI_SPX ) il++;
						}
						if ( il > ceil ( 0.2 * Spx->NombreDeVariablesHorsBase ) )  {							
              SPX_ResetReferenceSpace( Spx );
              # if VERBOSE_SPX
							  printf("Iteration %d reset reference space car on peut plus faire d'hyper creux\n",Spx->Iteration);
	            # endif
							*ResetRefSpace = OUI_SPX;
							return;  
						}
					}								 
				}					
        /* Fin test */
				
	    }
		}
  }   
}

return;
}
/*----------------------------------------------------------------------------*/
void SPX_DualSteepestControleDuPoidsDeCntBase( PROBLEME_SPX * Spx, double BetaP, char * ResetRefSpace ) 
{
int IndexCntBase;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
	IndexCntBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
}
else {
  IndexCntBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
}

if ( Spx->DualPoids[IndexCntBase] > /*2.*/ 4. * BetaP || Spx->DualPoids[IndexCntBase] < /*0.5*/ 0.25 * BetaP ) {
  #if VERBOSE_SPX
    printf("Iteration %d BetaP %e Spx->DualPoids[CntBase] %e CntBase %d\n",Spx->Iteration,BetaP,Spx->DualPoids[IndexCntBase],IndexCntBase); 
    printf("VariableSortante %d VariableEntrante %d\n",Spx->VariableSortante,Spx->VariableEntrante); 
    printf("Iteration %d RESET REFERENCE SPACE par derive excessive des poids\n",Spx->Iteration);
  #endif
	*ResetRefSpace = OUI_SPX;
  SPX_ResetReferenceSpace( Spx );	
  return;
}

return;
}
/*----------------------------------------------------------------------------*/
void SPX_DualSteepestVariablesDeBornesIdentiques( PROBLEME_SPX * Spx, double * BetaP ) 
{
/* Dynamic steepest edge: si la variable sortante est une variable basique artificielle on l'enleve de 
   l'espace de reference */
if ( Spx->Xmin[Spx->VariableSortante] == Spx->Xmax[Spx->VariableSortante] ) {
  if ( Spx->InDualFramework[Spx->VariableSortante] == OUI_SPX ) { 
    Spx->InDualFramework[Spx->VariableSortante] = NON_SPX;
    *BetaP = *BetaP - DUAL_POIDS_INITIAL;
  }
}

return;
}
/*----------------------------------------------------------------------------*/
/*        Attention il s'agit de la methode "Projected Steepest Edge"         */

void SPX_MajPoidsDualSteepestEdge( PROBLEME_SPX * Spx )
{
int Index; int IndexCntBase; char ResetRefSpace; double BetaP; double Rapport; double X;
double UnSurABarreSCntBase ; char ResoudreLeSysteme; char * InDualFramework; int i;
double *ArrayABarreS; int VariableEntrante; double ABarreS; double * DualPoids; double * Tau;
int VariableSortante; int * CntDeABarreSNonNuls; char ResolutionEnHyperCreux;                   
int * IndexTermesNonNulsDeTau; int NbTermesNonNulsDeTau; char StockageDeTau;
int IndexMax; 
 
if ( Spx->TypeDePricing == PRICING_DANTZIG ) return;

VariableEntrante = Spx->VariableEntrante;
VariableSortante = Spx->VariableSortante;

/* Calcul de BetaP */  
InDualFramework = Spx->InDualFramework;
BetaP = 0.;
if ( InDualFramework[VariableSortante] == OUI_SPX ) BetaP = DUAL_POIDS_INITIAL;

if ( Spx->CalculABarreSEnHyperCreux == OUI_SPX && Spx->CalculTauEnHyperCreux == OUI_SPX ) ResolutionEnHyperCreux = OUI_SPX;
else ResolutionEnHyperCreux = NON_SPX;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_DualSteepestEdgeResolutionBaseReduite( Spx, &BetaP, ResolutionEnHyperCreux, &ResetRefSpace, &NbTermesNonNulsDeTau, &StockageDeTau, &ResoudreLeSysteme );
}
else {
  SPX_DualSteepestEdgeResolutionAvecBaseComplete( Spx, &BetaP, ResolutionEnHyperCreux, &ResetRefSpace, &NbTermesNonNulsDeTau, &StockageDeTau, &ResoudreLeSysteme );
}

if ( ResetRefSpace == OUI_SPX ) return;

DualPoids = Spx->DualPoids;
ArrayABarreS = Spx->ABarreS;  
Tau = Spx->Tau;
IndexTermesNonNulsDeTau = (int *) Spx->ErBMoinsUn;  	
UnSurABarreSCntBase = 1. / Spx->ABarreSCntBase;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  IndexMax = Spx->RangDeLaMatriceFactorisee;
	IndexCntBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[VariableSortante]];
}
else {
  IndexMax = Spx->NombreDeContraintes;
  IndexCntBase = Spx->ContrainteDeLaVariableEnBase[VariableSortante];
}

/* On n'aurait pas besoin de calculer DualPoids des contraintes dont la variable en base est
   non bornee car ces variables ne sortent jamais de la base */
	 
if ( ResoudreLeSysteme == OUI_SPX ) {
  if ( Spx->TypeDeStockageDeABarreS == VECTEUR_SPX ) { 		
		if ( StockageDeTau == VECTEUR_SPX ) {
      for ( Index = 0 ; Index < IndexMax  ; Index++ ) {    
        ABarreS = ArrayABarreS[Index];      
        if ( ABarreS != 0.0 ) {
		      Rapport = ABarreS * UnSurABarreSCntBase;									
          X = DualPoids[Index] + ( ( ( -2. * Tau[Index] ) + ( Rapport * BetaP ) ) * Rapport );			
          if ( X < SEUIL_CARRE_DE_NORME_TROP_FAIBLE ) {			  
	          #if VERBOSE_SPX
              printf("Iteration %d Cnt %d X %e => RESET REFERENCE SPACE par carre de norme trop faible\n",Spx->Iteration,Index,X);
            #endif				
            SPX_ResetReferenceSpace( Spx ); 
            return;				
          }										
          DualPoids[Index] = X;    
        }    
        Tau[Index] = 0.0;
      }
	  }
		else {
      for ( Index = 0 ; Index < IndexMax  ; Index++ ) {    
        ABarreS = ArrayABarreS[Index];      
        if ( ABarreS != 0.0 ) {
		      Rapport = ABarreS * UnSurABarreSCntBase;									
          X = DualPoids[Index] + ( ( ( -2. * Tau[Index] ) + ( Rapport * BetaP ) ) * Rapport );			
          if ( X < SEUIL_CARRE_DE_NORME_TROP_FAIBLE ) {			  
	          #if VERBOSE_SPX
              printf("Iteration %d Cnt %d X %e => RESET REFERENCE SPACE par carre de norme trop faible\n",Spx->Iteration,Index,X);
            #endif				
            SPX_ResetReferenceSpace( Spx ); 
            return;				
          }									
          DualPoids[Index] = X;    
        }    
      }
			for ( i = 0 ; i < NbTermesNonNulsDeTau ; i++ ) Tau[IndexTermesNonNulsDeTau[i]] = 0.0;
		}
	}
	else {
    CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
    for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
		  Rapport = ArrayABarreS[i] * UnSurABarreSCntBase;		
      Index = CntDeABarreSNonNuls[i];
      DualPoids[Index] += ( ( -2. * Tau[Index] ) + ( Rapport * BetaP ) ) * Rapport;			
      if ( DualPoids[Index] < SEUIL_CARRE_DE_NORME_TROP_FAIBLE ) {			  
	      #if VERBOSE_SPX
          printf("Iteration %d Cnt %d X %e => RESET REFERENCE SPACE par carre de norme trop faible\n",
					        Spx->Iteration,Index,DualPoids[Index]);
        #endif					
        SPX_ResetReferenceSpace( Spx ); 
        return;				
      }						
    }
		if ( StockageDeTau != VECTEUR_SPX ) {
		  for ( i = 0 ; i < NbTermesNonNulsDeTau ; i++ ) Tau[IndexTermesNonNulsDeTau[i]] = 0.0;
		}
		else {
		  for ( i = 0 ; i < IndexMax ; i++ ) Tau[i] = 0.0;			
		}
	}
}
else {
  if ( Spx->TypeDeStockageDeABarreS == VECTEUR_SPX ) {
    for ( Index = 0 ; Index < IndexMax  ; Index++ ) {    
      ABarreS = ArrayABarreS[Index];
      if ( ABarreS != 0.0 ) {		  
        Rapport = ABarreS * UnSurABarreSCntBase;						
        X = DualPoids[Index] + ( Rapport * BetaP * Rapport );						
        if ( X < SEUIL_CARRE_DE_NORME_TROP_FAIBLE ) {			  
	        #if VERBOSE_SPX
            printf("Iteration %d Cnt %d X %e => RESET REFERENCE SPACE par carre de norme trop faible\n",Spx->Iteration,Index,X);
          #endif				
          SPX_ResetReferenceSpace( Spx ); 
          return;				
        }								
        DualPoids[Index] = X;
      }
    }
	}
	else {
    CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
    for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
		  Rapport = ArrayABarreS[i] * UnSurABarreSCntBase;		
      Index = CntDeABarreSNonNuls[i];
      X = DualPoids[Index] + ( Rapport * BetaP * Rapport );									
      if ( X < SEUIL_CARRE_DE_NORME_TROP_FAIBLE ) {			  
	      #if VERBOSE_SPX
          printf("Iteration %d Cnt %d X %e => RESET REFERENCE SPACE par carre de norme trop faible\n",Spx->Iteration,Index,X);
        #endif				
        SPX_ResetReferenceSpace( Spx ); 
        return;				
      }			
      DualPoids[Index] = X;    
    }
	}
}

DualPoids[IndexCntBase] = BetaP * UnSurABarreSCntBase * UnSurABarreSCntBase;  

/* Dynamic steepest edge: on ajoute la variable entrante dans l'espace de reference */
if ( InDualFramework[VariableEntrante] == NON_SPX ) {
  InDualFramework[VariableEntrante] = OUI_SPX;
  DualPoids[IndexCntBase] += DUAL_POIDS_INITIAL;
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_TenterRestaurationCalculTauEnHyperCreux( PROBLEME_SPX * Spx )
{
int Index; double * Tau; int Count; int IndexMax; 

if ( Spx->CountEchecsTau == 0 ) {
  if ( Spx->Iteration % CYCLE_TENTATIVE_HYPER_CREUX == 0 ) {
		Spx->NbEchecsTau    = SEUIL_REUSSITE_CREUX;
		Spx->CountEchecsTau = SEUIL_REUSSITE_CREUX + 2;
	}
}
if ( Spx->CountEchecsTau == 0 ) return;

Spx->CountEchecsTau--;		
/* On compte le nombre de termes non nuls du resultat */
Tau = Spx->Tau;
Count = 0;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  IndexMax = Spx->RangDeLaMatriceFactorisee;
}
else {
  IndexMax = Spx->NombreDeContraintes;
}

for ( Index = 0 ; Index < IndexMax ; Index++ ) if ( Tau[Index] != 0.0 ) Count++;


if ( Count < 0.1 * IndexMax ) Spx->NbEchecsTau--;
if ( Spx->NbEchecsTau <= 0 ) {
  # if VERBOSE_SPX
    printf("Remise en service de l'hyper creux pour le steepest edge, iteration %d\n",Spx->Iteration);
	# endif
  Spx->AvertissementsEchecsTau = 0;
  Spx->CountEchecsTau = 0;
	Spx->CalculTauEnHyperCreux = OUI_SPX;
}
else if ( Spx->CountEchecsTau <= 0 ) {
  Spx->CountEchecsTau = 0;
  if ( Spx->CalculTauEnHyperCreux == NON_SPX ) Spx->AvertissementsEchecsTau++;
  if ( Spx->AvertissementsEchecsTau >= SEUIL_ABANDON_HYPER_CREUX ) {
	  # if VERBOSE_SPX
      printf("Arret prolonge de l'hyper creux pour le steepest edge, iteration %d\n",Spx->Iteration);
	  # endif
	  Spx->CalculTauEnHyperCreuxPossible = NON_SPX;
	}
}

return;
}
  
  
