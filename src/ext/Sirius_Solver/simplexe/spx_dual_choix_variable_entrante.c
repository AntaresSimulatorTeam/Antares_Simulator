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

   FONCTION: Algorithme dual: choix de la variable qui entre en base.

   Rappel: dans l'alogrithme dual le test du ratio sert a determiner 
           la variable hors base qui entre en base alors que dans
           l'algorithme primal c'est l'inverse  
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define TRI_RAPPORTS_AVEC_HARRIS 1  
# define TRI_RAPPORTS_SANS_HARRIS 0

# define CYCLE_POUR_CHOIX_DU_TRI_RAPIDE 10
# define SEUIL_DECLENCHEMENT_TRI_RAPIDE 10
# define SEUIL_2_POUR_TRI_RAPIDE        100

int SPX_PartitionTriRapide( PROBLEME_SPX * , double * , int , int , char );
void SPX_TriRapide( PROBLEME_SPX * , double * , int , int , char );
void SPX_DualTestDuRatioChoixDeLaVariableEntrante( PROBLEME_SPX * , int * );

/*----------------------------------------------------------------------------*/
int SPX_PartitionTriRapide( PROBLEME_SPX * Spx , double * Tableau , int Deb, int Fin , char TypeDeTri )
{
int Compt; double Pivot; int i; double X; /*char Ch;*/ int iV; int DebPlus1;
/*double * CBarreSurNBarreRAvecTolerance    ; */ double * CBarreSurNBarreR      ;
int * NumeroDesVariableATester ;

TypeDeTri = 0; /* Pour ne pas avoir de warning a la compilation */

DebPlus1 = Deb + 1;
Compt    = Deb;
Pivot    = Tableau[Deb];

/* Tri sans tolerance de Harris */ 
CBarreSurNBarreR         = Spx->CBarreSurNBarreR;
NumeroDesVariableATester = Spx->NumeroDesVariableATester;
for ( i = DebPlus1 ; i <= Fin ; i++) {
  if ( Tableau[i] < Pivot) {
    Compt++;            
    /* On inverse le tableau sans tolerances */
    X = CBarreSurNBarreR[Compt]; 
    CBarreSurNBarreR[Compt] = CBarreSurNBarreR[i];
    CBarreSurNBarreR[i] = X;
    iV = NumeroDesVariableATester[Compt];
    NumeroDesVariableATester[Compt] = NumeroDesVariableATester[i];            
    NumeroDesVariableATester[i] = iV;
      
  }
}
X = CBarreSurNBarreR[Compt]; 
CBarreSurNBarreR[Compt] = CBarreSurNBarreR[Deb];
CBarreSurNBarreR[Deb] = X;
iV = NumeroDesVariableATester[Compt];
NumeroDesVariableATester[Compt] = NumeroDesVariableATester[Deb];            
NumeroDesVariableATester[Deb] = iV; 
return(Compt);
}
/*----------------------------------------------------------------------------*/
void SPX_TriRapide( PROBLEME_SPX * Spx , double * Tableau , int Debut , int Fin , char TypeDeTri )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = SPX_PartitionTriRapide( Spx , Tableau , Debut , Fin , TypeDeTri );  
  SPX_TriRapide( Spx , Tableau , Debut   , Pivot-1 , TypeDeTri );
  SPX_TriRapide( Spx , Tableau , Pivot+1 , Fin     , TypeDeTri );
}
return;
}

/*----------------------------------------------------------------------------*/

void SPX_DualTestDuRatioChoixDeLaVariableEntrante( PROBLEME_SPX * Spx , int * i0Harris )
{
int j; int i; double SeuilHarris; double NBarreRMx; int Var;
int iChoisi    ; double * CBarreSurNBarreRAvecTolerance;
int * NumeroDesVariableATester; double * CBarreSurNBarreR; double * NBarreR;
int NombreDeVariablesATester; int * NumerosDesVariables; int jChoisi;

NombreDeVariablesATester      = Spx->NombreDeVariablesATester;
CBarreSurNBarreRAvecTolerance = Spx->CBarreSurNBarreRAvecTolerance;
NumeroDesVariableATester      = Spx->NumeroDesVariableATester;
CBarreSurNBarreR              = Spx->CBarreSurNBarreR;
NBarreR                       = Spx->NBarreR;

if ( Spx->TypeDeStockageDeNBarreR == VECTEUR_SPX ) NumerosDesVariables = Spx->NumerosDesVariablesHorsBase;
else NumerosDesVariables = Spx->NumVarNBarreRNonNul;

if ( Spx->FaireTriRapide == OUI_SPX ) {
  if ( *i0Harris >= NombreDeVariablesATester ) return;
  SeuilHarris = LINFINI_SPX;
  while ( *i0Harris < NombreDeVariablesATester ) {
    /* Car il peut y avoir des FlagVariableATester a 0 des le depart */
    if ( /*FlagVariableATester[*i0Harris] == 1*/ 1 ) { 
      SeuilHarris = CBarreSurNBarreRAvecTolerance[*i0Harris];
      *i0Harris = *i0Harris + 1; /* Attention c'est pas bon car c'est pas forcement celle la qu'on va choisir */
	    break;
    }
    else *i0Harris = *i0Harris + 1;
  }    
  iChoisi   = -1;
  NBarreRMx = -LINFINI_SPX;
  for ( j = 0 ; j < NombreDeVariablesATester ; j++ ) {
    i = NumeroDesVariableATester[j];
    if ( i < 0 ) continue;
	  Var = NumerosDesVariables[i];			
    if ( CBarreSurNBarreR[j]  > SeuilHarris ) break;
    if ( fabs( NBarreR[Var] ) > NBarreRMx ) {								
      NBarreRMx = fabs( NBarreR[Var] );
      iChoisi   = j;
      Spx->VariableEntrante = Var;      
    } 
  }			 	
  if ( iChoisi >= 0 ) NumeroDesVariableATester[iChoisi] = -1;	
  return;
}

/* On a pas fait de tri prealable */

SeuilHarris = LINFINI_SPX;
jChoisi     = -1;
for ( j = 0 ; j < NombreDeVariablesATester ; j++ ) {	
  if ( CBarreSurNBarreRAvecTolerance[j] < SeuilHarris ) {
    SeuilHarris = CBarreSurNBarreRAvecTolerance[j];		
    jChoisi     = j;
  }
}

if ( jChoisi < 0 ) return;

jChoisi   = -1;
NBarreRMx = -LINFINI_SPX;

for ( j = 0 ; j < NombreDeVariablesATester ; j++ ) {
  if ( CBarreSurNBarreR[j] > SeuilHarris ) continue;	
	i = NumeroDesVariableATester[j];
  if ( i < 0 ) continue;	
	Var = NumerosDesVariables[i];	
  if ( fabs( NBarreR[Var] ) > NBarreRMx ) {					
    NBarreRMx = fabs( NBarreR[Var] );
    jChoisi   = j;
    Spx->VariableEntrante = Var;		
  }		
}

/* Si on n'a pas fait de tri, les indices des tableaux CBarreSurNBarreR et CBarreSurNBarreRAvecTolerance
   sont les memes */
if ( jChoisi >= 0 ) {
  CBarreSurNBarreR             [jChoisi] = CBarreSurNBarreR[NombreDeVariablesATester-1];
  CBarreSurNBarreRAvecTolerance[jChoisi] = CBarreSurNBarreRAvecTolerance[NombreDeVariablesATester-1];
	NumeroDesVariableATester     [jChoisi] = NumeroDesVariableATester[NombreDeVariablesATester-1];
	Spx->NombreDeVariablesATester--;
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_DualTestDuRatio( PROBLEME_SPX * Spx , int * i0Harris )
{
Spx->DeltaPiSurLaVariableEnBase = LINFINI_SPX;
Spx->VariableEntrante           = -1;
Spx->ChangementDeBase           = NON_SPX;

if ( Spx->PremierPassage == OUI_SPX ) {

  if ( Spx->NbItBoundFlip >= CYCLE_POUR_CHOIX_DU_TRI_RAPIDE ) {
		if ( Spx->BuffNbBoundFlip / Spx->NbItBoundFlip > SEUIL_DECLENCHEMENT_TRI_RAPIDE ) Spx->FaireTriRapide = OUI_SPX;
		else Spx->FaireTriRapide = NON_SPX;
    Spx->BuffNbBoundFlip = 0;		
    Spx->NbItBoundFlip = 0;
	}

  if ( Spx->NbBoundFlipIterationPrecedente > SEUIL_2_POUR_TRI_RAPIDE ) {
	  Spx->FaireTriRapide = OUI_SPX;		
	}	

	/* Inhibition pour l'instant 4/2/2011 car erreur sur flag var a tester */
	Spx->FaireTriRapide = NON_SPX;
	  
  if ( Spx->FaireTriRapide == OUI_SPX ) {
    SPX_TriRapide( Spx , Spx->CBarreSurNBarreR , 0 , Spx->NombreDeVariablesATester - 1 , TRI_RAPPORTS_SANS_HARRIS );
  }
  
  Spx->PremierPassage = NON_SPX;
}

SPX_DualTestDuRatioChoixDeLaVariableEntrante( Spx , i0Harris );

return;
}

/*----------------------------------------------------------------------------*/

void SPX_DualChoixDeLaVariableEntrante( PROBLEME_SPX * Spx )
{
int IndexCntBase; int DerniereVariableEntrante; double BBarreDeCntBase;  int NbTermesBoundFlip; double SeuilBound; 
double DeltaXSurLaVariableHorsBase; int i0Harris; double * Xmax; double * NBarreR; char * PositionDeLaVariable;
char * TypeDeVariable ; double * Xmin; int * CNbTerm; char * StatutBorneSupCourante; double XsVariableSortante;
double XiVariableSortante;

double tk; double tk_1; double zk_1; double zk; double * CBarre;

tk_1 = 0.0;
zk_1 = 0.0;
CBarre = Spx->CBarre;

StatutBorneSupCourante = Spx->StatutBorneSupCourante;

i0Harris = 0;

Spx->PremierPassage = OUI_SPX;

Spx->NbBoundFlip = 0; 
Spx->FaireMiseAJourDeBBarre = OUI_SPX;

SeuilBound = 0.1; 

DerniereVariableEntrante    = -1;
NbTermesBoundFlip           = 0;
DeltaXSurLaVariableHorsBase = 0.0;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
	IndexCntBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];	
}
else {
  IndexCntBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
}

BBarreDeCntBase = Spx->BBarre[IndexCntBase];

Xmax    = Spx->Xmax;
Xmin    = Spx->Xmin;
NBarreR = Spx->NBarreR;
PositionDeLaVariable = Spx->PositionDeLaVariable;
TypeDeVariable       = Spx->TypeDeVariable;

CNbTerm = Spx->CNbTerm;

XsVariableSortante = Xmax[Spx->VariableSortante];
XiVariableSortante = Xmin[Spx->VariableSortante];

Spx->ChangementDeBase = NON_SPX;

while ( Spx->ChangementDeBase == NON_SPX ) {
  
  SPX_DualTestDuRatio( Spx , &i0Harris );
  if ( Spx->VariableEntrante < 0 ) break; /* Optimum non borne */
	
  /* Calcul du deplacement primal sur la variable entrante */
  Spx->ChangementDeBase = OUI_SPX;

  if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
    Spx->DeltaXSurLaVariableHorsBase = BBarreDeCntBase / NBarreR[Spx->VariableEntrante];
  }
  else { /* SORT_SUR_XMAX */
    Spx->DeltaXSurLaVariableHorsBase = ( BBarreDeCntBase - XsVariableSortante ) / NBarreR[Spx->VariableEntrante];
  }

	tk = CBarre[Spx->VariableEntrante] / NBarreR[Spx->VariableEntrante];
  if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) zk = zk_1 + ( (tk - tk_1 ) * BBarreDeCntBase );
	else zk = zk_1 + ( (tk - tk_1 ) * ( BBarreDeCntBase - XsVariableSortante ) );
	zk_1 = zk;
	tk_1 = tk;
	
  if ( fabs( Spx->DeltaXSurLaVariableHorsBase ) > Xmax[Spx->VariableEntrante] &&
	     TypeDeVariable[Spx->VariableEntrante] == BORNEE ) {			 		
		/*
    printf(" *** Iteration %d simple changement de borne sur la variable %d position %d DeltaXSurLaVariableHorsBase %lf Xmax %lf\n",
            Spx->Iteration,Spx->VariableEntrante,(int) Spx->PositionDeLaVariable[Spx->VariableEntrante] ,Spx->DeltaXSurLaVariableHorsBase,
						Spx->Xmax[Spx->VariableEntrante]); 
    */	    
    /* Simple changement de borne */
    if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_SUP ) {
		  # ifdef UTILISER_BORNES_AUXILIAIRES
        if ( StatutBorneSupCourante[Spx->VariableEntrante] != BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
			# endif		 
      Spx->ChangementDeBase = NON_SPX;		
			/* Attention, il faut revenir en arriere sur le changement de position si on ne fait pas le changement
			   de base */
      DeltaXSurLaVariableHorsBase = -Xmax[Spx->VariableEntrante];
      Spx->BoundFlip[Spx->NbBoundFlip] = -( Spx->VariableEntrante + 1 );
      Spx->NbBoundFlip++;
      if ( Spx->NbBoundFlip == MAX_BOUND_FLIP ) {
			  Spx->FaireMiseAJourDeBBarre = NON_SPX;								
      }			
      NbTermesBoundFlip+= CNbTerm[Spx->VariableEntrante];
      if ( NbTermesBoundFlip > (int) ( SeuilBound * Spx->NombreDeContraintes ) ) Spx->FaireMiseAJourDeBBarre = NON_SPX;								
		  # ifdef UTILISER_BORNES_AUXILIAIRES
			  }
			# endif		 			
    }
    else { /* HORS_BASE_SUR_BORNE_INF */
      # ifdef UTILISER_BORNES_AUXILIAIRES
		    /* Si la variable a une borne sup non native, on ne fait pas de bound flip puisque cela signifie qu'elle n'a pas
			     de borne sup native */		
        if ( StatutBorneSupCourante[Spx->VariableEntrante] == BORNE_NATIVE ) {			 
			# endif		
      Spx->ChangementDeBase = NON_SPX;			
			/* Attention, il faut revenir en arriere sur le changement de position si on ne fait pas le changement
			   de base */			
      DeltaXSurLaVariableHorsBase = Xmax[Spx->VariableEntrante];
      Spx->BoundFlip[Spx->NbBoundFlip] = Spx->VariableEntrante + 1;
      Spx->NbBoundFlip++;
      if ( Spx->NbBoundFlip == MAX_BOUND_FLIP ) {
			  Spx->FaireMiseAJourDeBBarre = NON_SPX;								
      }						
      NbTermesBoundFlip+= CNbTerm[Spx->VariableEntrante];			  
      if ( NbTermesBoundFlip > (int) ( SeuilBound * Spx->NombreDeContraintes ) ) Spx->FaireMiseAJourDeBBarre = NON_SPX;				
		  # ifdef UTILISER_BORNES_AUXILIAIRES
			  }
			# endif
    }
  }    	
  /* Dans le cas du bound flip, mise a jour de BBarre pour la variable sortante */ 
  if ( Spx->ChangementDeBase == NON_SPX ) {		
    BBarreDeCntBase-= DeltaXSurLaVariableHorsBase * NBarreR[Spx->VariableEntrante];
    if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) { 
      if ( BBarreDeCntBase >= XiVariableSortante ) {
        /* On arrete les bound flip */
        Spx->ChangementDeBase = OUI_SPX;
        /* Comme on fait effectivement le changement de base il ne faut pas faire le bound flip
           sur cette variable */				
				Spx->NbBoundFlip--;	 
        break;
      }
    }
    else { /* La variable sort sur Xmax */
      if ( BBarreDeCntBase <= XsVariableSortante ) { 
        /* On arrete les bound flip */
        Spx->ChangementDeBase = OUI_SPX;
        /* Comme on fait effectivement le changement de base il ne faut pas faire le bound flip
           sur cette variable */										 
				Spx->NbBoundFlip--;	 
        break;
      }
    }
  }

  DerniereVariableEntrante = Spx->VariableEntrante;

}

if ( Spx->VariableEntrante < 0 ) {
  /* Par securite on fait toujours le changement car la mise a jour de BBarre est entachee d'erreur 
     et pourrait conduire a un diagnotic premature d'absence de solution */
  Spx->VariableEntrante = DerniereVariableEntrante;
	
  if ( Spx->VariableEntrante >= 0 ) {
    /* La derniere variable avait fait un bound flip => pour les besoins de la mise a jour de BBarre on 
       initialise son deplacement a 0 */	 
		Spx->NbBoundFlip--;		
  }
}
			   
if ( Spx->VariableEntrante >= 0 ) {
	
  Spx->ChangementDeBase = OUI_SPX;
	
  Spx->BuffNbBoundFlip+= Spx->NbBoundFlip;		
  Spx->NbItBoundFlip++;
	
	Spx->NbBoundFlipIterationPrecedente = Spx->NbBoundFlip;
	
}

return;
}



