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

   FONCTION: Comparaison de ABarreS et NBarreR. Si l'ecart est trop 
             grand, on refactorise la base et eventuellement on ne 
             fait pas le changement de base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define NOMBRE_DE_FOIS_SCALING_SI_DISCORANCE 0

# define CYCLE_POUR_LA_COMPARAISON_ENTRE_ABarreS_ET_NBarreR 10

/*----------------------------------------------------------------------------*/

void SPX_DualComparerABarreSEtNBarreR( PROBLEME_SPX * Spx )

{
double Seuil; double X; double ABarreS; double NBarreR; int VariableEntrante; int VariableSortante;

if ( Spx->Iteration % CYCLE_POUR_LA_COMPARAISON_ENTRE_ABarreS_ET_NBarreR != 0 ) return;

VariableEntrante = Spx->VariableEntrante;
VariableSortante = Spx->VariableSortante;

ABarreS = Spx->ABarreSCntBase;

NBarreR = Spx->NBarreR[VariableEntrante];

/* En cas de discordance de Spx->ABarreSCntBase et de Spx->NBarreR[Spx->VariableEntrante] alors il 
   faut s'empresser de refactoriser (et en principe il ne faudrait pas faire le changement de base) */
if ( fabs( ABarreS ) < 0.1 * VALEUR_DE_PIVOT_ACCEPTABLE ) {
  #if VERBOSE_SPX
    printf("Iteration %d factorisation de la base demandee car Spx->ABarreS trop faible on fait pas le changement de base\n",Spx->Iteration);
    printf("          Spx->ABarreS= %e \n",ABarreS);
  #endif
  
  Spx->FaireScalingLU                  = NOMBRE_DE_FOIS_SCALING_SI_DISCORANCE;
  Spx->FlagStabiliteDeLaFactorisation  = 1;
  Spx->FactoriserLaBase                = OUI_SPX;  
  Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;
  Spx->NombreMaxDeChoixAuHasard        = 2;   
  Spx->NombreDeChoixFaitsAuHasard      = 0; 
  Spx->FaireDuRaffinementIteratif      = 5;
  SPX_FactoriserLaBase( Spx );
  Spx->FaireChangementDeBase = NON_SPX;

	/* On augmente le seuil dual de pivotage */
	Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;
	
  #if VERBOSE_SPX
    printf("Iteration %d nombre de choix de pivot au hasard a faire parmi les choix acceptables: %d\n",
		        Spx->Iteration,Spx->NombreMaxDeChoixAuHasard);
  #endif						
  return;
} 

if ( ABarreS * NBarreR < 0. ) {
  #if VERBOSE_SPX
    printf("Iteration %d factorisation de la base demandee car Spx->ABarreS et Spx->NBarreR de signes differents\n",Spx->Iteration);
    printf("          Spx->ABarreS= %e  NBarreR= %e\n",ABarreS,NBarreR);
  #endif
   
  Spx->FaireScalingLU                  = NOMBRE_DE_FOIS_SCALING_SI_DISCORANCE;
  Spx->FlagStabiliteDeLaFactorisation  = 1;
  Spx->FactoriserLaBase                = OUI_SPX;
  Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;
  Spx->NombreMaxDeChoixAuHasard        = 1;   
  Spx->NombreDeChoixFaitsAuHasard      = 0; 
  Spx->FaireDuRaffinementIteratif      = 5;
  SPX_FactoriserLaBase( Spx );
  Spx->FaireChangementDeBase = NON_SPX;

	/* On augmente le seuil dual de pivotage */
	Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;
		
  #if VERBOSE_SPX
    printf("Iteration %d nombre de choix de pivot au hasard a faire parmi les choix acceptables: %d\n",
		        Spx->Iteration,Spx->NombreMaxDeChoixAuHasard);
  #endif


  return;
}  

if ( fabs( ABarreS ) < 0.5 * VALEUR_DE_PIVOT_ACCEPTABLE ) {
  #if VERBOSE_SPX
    printf("Iteration %d factorisation de la base demandee car Spx->ABarreS trop faible mais on fait le changement de base\n",Spx->Iteration);
    printf("          Spx->ABarreS= %e  NBarreR= %e\n",ABarreS,NBarreR );
  #endif
	
  Spx->FaireDuRaffinementIteratif     = 5; 
  Spx->FlagStabiliteDeLaFactorisation = 1;
  Spx->FactoriserLaBase = OUI_SPX;
	
	/* On augmente le seuil dual de pivotage */
	Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;	
	
  return;
}  


/*
Seuil = 1.e-4 * fabs ( NBarreR );
if ( Seuil < 1.e-6 ) Seuil = 1.e-6;
else if ( Seuil > 0.1 ) Seuil = 0.1;
X = fabs( ABarreS - NBarreR );
if ( X > Seuil && X > 1.e-4 ) {
*/

Seuil = 1.e-3;
X = fabs( ABarreS - NBarreR ) / ( 1. + fabs( NBarreR ) );
if ( X > Seuil ) {
  #if VERBOSE_SPX
    printf("Iteration %d factorisation de la base demandee car Spx->ABarreS trop different de Spx->NBarreR\n",Spx->Iteration);
    printf("          Spx->ABarreS= %e  NBarreR= %e\n",ABarreS,NBarreR );
  #endif
    
  if ( Spx->NombreDeChangementsDeBase < 10 ) {
	  /* Si ca se produit dans les premieres iterations apres une factorisation */
	  Spx->FlagStabiliteDeLaFactorisation = 1;   			
	}
  Spx->FactoriserLaBase = OUI_SPX;
  /* Ancienne strategie */
  /*Spx->FaireLeChangementDeBase = NON_SPX;*/ 
	/* Le changement de base risque de ne pas etre fiable */	  
	if ( X < 0.01 * fabs ( NBarreR ) ) Spx->FaireChangementDeBase = OUI_SPX;
	else Spx->FaireChangementDeBase = NON_SPX;
	
	/* On augmente le seuil dual de pivotage */
	Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;
	
  return;	
}

return;

}
