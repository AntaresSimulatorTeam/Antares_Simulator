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
   FONCTION: Gestion des listes de variables hors base et des bornes
	           sur les variables en base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"

# define MARGE_ABSOLUE_MIN 1.e-12 /*1.e-12*/
# define MARGE_ABSOLUE_MAX 1.e-9 /*1.e-9*/

/*----------------------------------------------------------------------------*/

void SPX_InitialiserLeNombreDesVariablesHorsBaseDesContraintes( PROBLEME_SPX * Spx ) 
{
int il; int ilMax; int Cnt; char * PositionDeLaVariable; int Nb; int * Mdeb; int * NbTerm;
int * Indcol; int * NombreDeVariablesHorsBaseDeLaContrainte;

Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
NombreDeVariablesHorsBaseDeLaContrainte = Spx->NombreDeVariablesHorsBaseDeLaContrainte;
PositionDeLaVariable = Spx->PositionDeLaVariable;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	Nb = 0;
	while ( il < ilMax ) {
    if ( PositionDeLaVariable[Indcol[il]] != EN_BASE_LIBRE ) Nb++;
    il++;
	}
  NombreDeVariablesHorsBaseDeLaContrainte[Cnt] = Nb;
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_MettreAJourLeNombreDesVariablesHorsBaseDesContraintes( PROBLEME_SPX * Spx ) 
{
int ic; int icMx; int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; int * NombreDeVariablesHorsBaseDeLaContrainte;

Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;
NombreDeVariablesHorsBaseDeLaContrainte = Spx->NombreDeVariablesHorsBaseDeLaContrainte;

/* Mise a jour du nombre de variables hors base de chaque contrainte */
ic = Cdeb[Spx->VariableSortante];
icMx = ic + CNbTerm[Spx->VariableSortante];
while ( ic < icMx ) {
  NombreDeVariablesHorsBaseDeLaContrainte[NumeroDeContrainte[ic]]++;
  ic++;
}
ic = Cdeb[Spx->VariableEntrante];
icMx = ic + CNbTerm[Spx->VariableEntrante];
while ( ic < icMx ) {
  NombreDeVariablesHorsBaseDeLaContrainte[NumeroDeContrainte[ic]]--;
  ic++;
}
return;
}

/*----------------------------------------------------------------------------*/

void SPX_InitialiserLeTableauDesVariablesHorsBase( PROBLEME_SPX * Spx ) 
{
int Var; char * PositionDeLaVariable; int * CNbTerm;  int * NumerosDesVariablesHorsBase;
int NombreDeVariablesHorsBase; int * IndexDeLaVariableDansLesVariablesHorsBase; int NombreDeVariables;
char * InDualFramework; double * Xmin; double * Xmax; char LeSteepestEdgeEstInitilise;  

SPX_InitialiserLeNombreDesVariablesHorsBaseDesContraintes( Spx );
/*SPX_InitMatriceHorsBase( Spx );*/

NombreDeVariables = Spx->NombreDeVariables;
PositionDeLaVariable = Spx->PositionDeLaVariable;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) CNbTerm = Spx->CNbTermProblemeReduit;
else CNbTerm = Spx->CNbTerm;

CNbTerm = Spx->CNbTerm; /* Pour l'instant on les met toutes car dans la phase 1 on les positionne pour etre
                           dual admissible et on cree les bornes auxiliaires necessaires. Donc s'il manque
													 des variables hors base, la partie des variables qui intervient uniquement dans
													 la partie hors probleme reduit n'est pas prise en compte et en realite la
													 base de depart n'est pas duale realisable */

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
IndexDeLaVariableDansLesVariablesHorsBase = Spx->IndexDeLaVariableDansLesVariablesHorsBase;

NombreDeVariablesHorsBase = 0;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) IndexDeLaVariableDansLesVariablesHorsBase[Var] = -1;

Spx->PresenceDeVariablesDeBornesIdentiques = NON_SPX;

Xmin = Spx->Xmin;
Xmax = Spx->Xmax;
InDualFramework = Spx->InDualFramework;
LeSteepestEdgeEstInitilise = Spx->LeSteepestEdgeEstInitilise;	
if ( LeSteepestEdgeEstInitilise == OUI_SPX ) {
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {

		if ( CNbTerm[Var] == 0 ) continue;
			
		if ( Spx->PresenceDeVariablesDeBornesIdentiques == NON_SPX ) {
			if ( Xmin[Var] == Xmax[Var] ) Spx->PresenceDeVariablesDeBornesIdentiques = OUI_SPX;        
		}
    if ( PositionDeLaVariable[Var] != EN_BASE_LIBRE ) {		
      /* Si la variable a Xmin = Xmax (c'est a dire 0) on ne la met pas dans la liste.
         Et alors, soit il y a une solution avec le probleme reduit et alors elle
	       est optimale, soit il n'y en a pas et il n'y en aurait de toutes facons pas
	       meme si la variable n'avait pas ete enlevee de la liste */
			/* Noter que si la variable etait dans le framework du steepest edge, on ne sait pas
			   la faire sortir proprement. Cela entrainera donc une reinitialisation du
				 steepest edge a un moment donne. Mais les essais numeriques montrent que c'est
				 malgre tout une facon de faire qui accelere les temps de calcul */				 	 
      if ( InDualFramework[Var] == OUI_SPX ) {
        NumerosDesVariablesHorsBase[NombreDeVariablesHorsBase] = Var;
	 	    IndexDeLaVariableDansLesVariablesHorsBase[Var] = NombreDeVariablesHorsBase;
        NombreDeVariablesHorsBase++;				  
			}
      else if ( Xmin[Var] != Xmax[Var] ) {
        NumerosDesVariablesHorsBase[NombreDeVariablesHorsBase] = Var;
	 	    IndexDeLaVariableDansLesVariablesHorsBase[Var] = NombreDeVariablesHorsBase;
        NombreDeVariablesHorsBase++;
		  }
		}
  }
}
else {
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {

	  if ( CNbTerm[Var] == 0 ) continue;
		
	  if ( Spx->PresenceDeVariablesDeBornesIdentiques == NON_SPX ) {
		  if ( Xmin[Var] == Xmax[Var] ) Spx->PresenceDeVariablesDeBornesIdentiques = OUI_SPX;        
		}		
    if ( PositionDeLaVariable[Var] != EN_BASE_LIBRE ) {		
    /* Si la variable a Xmin = Xmax (c'est a dire 0) on ne la met pas dans la liste.
       Et alors, soit il y a une solution avec le probleme reduit et alors elle
	     est optimale, soit il n'y en a pas et il n'y en aurait de toutes facons pas
	     meme si la variable n'avait pas ete enlevee de la liste */
		/* Noter que si la variable etait dans le framework du steepest edge, on ne sait pas
		   la faire sortir proprement. Cela entrainera donc une reinitialisation du
			 steepest edge a un moment donne. Mais les essais numeriques montrent que c'est
			 malgre tout une facon de faire qui accelere les temps de calcul */				 	 
      if ( Xmin[Var] != Xmax[Var] ) {
        NumerosDesVariablesHorsBase[NombreDeVariablesHorsBase] = Var;
	 	    IndexDeLaVariableDansLesVariablesHorsBase[Var] = NombreDeVariablesHorsBase;
        NombreDeVariablesHorsBase++;
		  }
		}  	
	}
}
	
Spx->NombreDeVariablesHorsBase = NombreDeVariablesHorsBase;
/*
printf("SPX_InitialiserLeTableauDesVariablesHorsBase NombreDeVariablesHorsBase %d au lieu de %d\n",
        NombreDeVariablesHorsBase, Spx->NombreDeVariables-Spx->NombreDeContraintes);
*/
return;
}

/*----------------------------------------------------------------------------*/

void SPX_MajDuTableauDesVariablesHorsBase( PROBLEME_SPX * Spx ) 
{
int i; int j; int Var; int VariableEntrante; int VariableSortante;
int * IndexDeLaVariableDansLesVariablesHorsBase; int * NumerosDesVariablesHorsBase;
double * Xmin; double * Xmax;

SPX_MettreAJourLeNombreDesVariablesHorsBaseDesContraintes( Spx );
/*SPX_MettreAJourLaMatriceHorsBase( Spx );*/

VariableEntrante = Spx->VariableEntrante;
VariableSortante = Spx->VariableSortante;
IndexDeLaVariableDansLesVariablesHorsBase = Spx->IndexDeLaVariableDansLesVariablesHorsBase;
NumerosDesVariablesHorsBase               = Spx->NumerosDesVariablesHorsBase;
Xmin = Spx->Xmin;
Xmax = Spx->Xmax;

i = IndexDeLaVariableDansLesVariablesHorsBase[VariableEntrante];
NumerosDesVariablesHorsBase[i] = VariableSortante;

/* Si la variable sortante a Xmin = Xmax (c'est a dire 0) on la sort de la liste.
   Et alors, soit il y a une solution avec le probleme reduit et alors elle
   est optimale, soit il n'y en a pas et il n'y en aurait de toutes facons pas
	 meme si la variable n'avait pas ete enlevee de la liste */
if ( Xmin[VariableSortante] != Xmax[VariableSortante] ) {
  IndexDeLaVariableDansLesVariablesHorsBase[VariableSortante] = i;
  IndexDeLaVariableDansLesVariablesHorsBase[VariableEntrante] = -1;				
}
else {	
  IndexDeLaVariableDansLesVariablesHorsBase[VariableSortante] = -1;		
  IndexDeLaVariableDansLesVariablesHorsBase[VariableEntrante] = -1;		
	j = Spx->NombreDeVariablesHorsBase - 1;		
	if ( j != i ) {
	  Var = NumerosDesVariablesHorsBase[j];
	  IndexDeLaVariableDansLesVariablesHorsBase[Var] = i;		
    NumerosDesVariablesHorsBase              [i]   = Var;			
	}
  Spx->NombreDeVariablesHorsBase--;		
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_InitialiserLesVariablesEnBaseAControler( PROBLEME_SPX * Spx ) 
{
int Var; int NbSurv; double X; int IndexMax; int Index; double BorSup; double BorInf;
int * VariableEnBaseDeLaContrainte; char * TypeDeVariable; double * Xmax ;
double * SeuilDeViolationDeBorne; double * ValeurDeViolationDeBorne;
double * DualPoids; int * IndexDansContrainteASurveiller; double MargeAbsolue;
int * NumerosDesContraintesASurveiller; double * BBarre; double ValBBarre;
int * ColonneDeLaBaseFactorisee; 

Spx->A1 = PNE_Rand( Spx->A1 ); /* Nombre aleatoire entre 0 et 1 */
MargeAbsolue = MARGE_ABSOLUE_MIN;
MargeAbsolue += Spx->A1 * ( MARGE_ABSOLUE_MAX - MARGE_ABSOLUE_MIN );

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
TypeDeVariable               = Spx->TypeDeVariable;
SeuilDeViolationDeBorne      = Spx->SeuilDeViolationDeBorne;
Xmax                         = Spx->Xmax;

BBarre                           = Spx->BBarre;
DualPoids                        = Spx->DualPoids;
ValeurDeViolationDeBorne         = Spx->ValeurDeViolationDeBorne;
IndexDansContrainteASurveiller   = Spx->IndexDansContrainteASurveiller;
NumerosDesContraintesASurveiller = Spx->NumerosDesContraintesASurveiller;
NbSurv = 0;

ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee; /* Ici a cause des warning de compilation */
if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) IndexMax = Spx->RangDeLaMatriceFactorisee;
else IndexMax = Spx->NombreDeContraintes;

for ( Index = 0 ; Index < IndexMax ; Index++ ) IndexDansContrainteASurveiller[Index] = -1;

for ( Index = 0 ; Index < IndexMax ; Index++ ) {
  if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
    Var = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[Index]];
	}
	else {
    Var = VariableEnBaseDeLaContrainte[Index];
	}

	/* Initialisation des bornes et constitution de la liste */   
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue;
  else {
	  ValBBarre = BBarre[Index];
	  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
		
		  # if PRICING_AVEC_VIOLATIONS_STRICTES == OUI_SPX
				if ( SeuilDeViolationDeBorne[Var] > MargeAbsolue ) BorInf = -MargeAbsolue;
				else BorInf = -SeuilDeViolationDeBorne[Var];
			# else 
        BorInf = -SeuilDeViolationDeBorne[Var];
			# endif
						
      if ( ValBBarre < BorInf ) {
		    /* On ajoute la contrainte dans la liste */		
		    NumerosDesContraintesASurveiller[NbSurv] = Index;

			  # if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX							
		      ValeurDeViolationDeBorne[NbSurv] = ( ValBBarre * ValBBarre ) / DualPoids[Index];
				# else
		      ValeurDeViolationDeBorne[NbSurv] = ValBBarre * ValBBarre;
				# endif
				
		    IndexDansContrainteASurveiller[Index] = NbSurv;
		    NbSurv++;		 
	    }
			
	  }
    else {
		  /* La variable est bornee */
			
		  # if PRICING_AVEC_VIOLATIONS_STRICTES == OUI_SPX
				if ( SeuilDeViolationDeBorne[Var] > MargeAbsolue ) {								
          BorInf = -MargeAbsolue;					
	        BorSup = Xmax[Var] + MargeAbsolue;					
				}
				else {
          BorInf = -SeuilDeViolationDeBorne[Var];										
	        BorSup = Xmax[Var] + SeuilDeViolationDeBorne[Var];					
				}
			# else 
        BorInf = -SeuilDeViolationDeBorne[Var];								
	      BorSup = Xmax[Var] + SeuilDeViolationDeBorne[Var];				
			# endif
			
      if ( ValBBarre < BorInf ) {
		    /* On ajoute la contrainte dans la liste */		
		    NumerosDesContraintesASurveiller[NbSurv] = Index;
								
			  # if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX							
		      ValeurDeViolationDeBorne[NbSurv] = ( ValBBarre * ValBBarre ) / DualPoids[Index];		
				# else
		      ValeurDeViolationDeBorne[NbSurv] = ValBBarre * ValBBarre;		
				# endif
				
		    IndexDansContrainteASurveiller[Index] = NbSurv;
		    NbSurv++;		 
	    }				
	    else if ( ValBBarre > BorSup ) {
		    /* On ajoute la contrainte dans la liste */
		    NumerosDesContraintesASurveiller[NbSurv] = Index;				
	      X = ValBBarre - Xmax[Var];
				
			  # if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX							
	        ValeurDeViolationDeBorne[NbSurv] = ( X * X ) / DualPoids[Index];
				# else
	        ValeurDeViolationDeBorne[NbSurv] = X * X;
				# endif
				
		    IndexDansContrainteASurveiller[Index] = NbSurv;
		    NbSurv++;		
	    }
		}		
	}
}

Spx->NombreDeContraintesASurveiller = NbSurv;
	
return;
}

/*----------------------------------------------------------------------------*/

void SPX_MajDesVariablesEnBaseAControler( PROBLEME_SPX * Spx , int NbBBarreModifies,
                                          int * IndexDeBBarreModifies, int * T ) 
{
int Var; int Index; int IndexMax; char Viole; int NbSurv; int * IndexDansContrainteASurveiller;
int * NumerosDesContraintesASurveiller; int Index1; int IndexCntBase; int k; int i; double BorSup;
double BorInf; int * VariableEnBaseDeLaContrainte; char * TypeDeVariable; double * SeuilDeViolationDeBorne;
double X; double * DualPoids; double * ValeurDeViolationDeBorne; int VariableSortante;
int VariableEntrante; double ValeurDeBBarre; double * BBarre; double * Xmax;
int * ColonneDeLaBaseFactorisee; double MargeAbsolue;

Spx->A1 = PNE_Rand( Spx->A1 ); /* Nombre aleatoire entre 0 et 1 */
MargeAbsolue = MARGE_ABSOLUE_MIN;
MargeAbsolue += Spx->A1 * ( MARGE_ABSOLUE_MAX - MARGE_ABSOLUE_MIN );

VariableSortante = Spx->VariableSortante;
VariableEntrante = Spx->VariableEntrante;
 
ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee; /* Ici a cause des warning de compilation */
if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  IndexMax = Spx->RangDeLaMatriceFactorisee;
	IndexCntBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
}
else {
  IndexMax = Spx->NombreDeContraintes;
  IndexCntBase = Spx->ContrainteDeLaVariableEnBase[VariableSortante];
}

# ifdef UTILISER_BORNES_AUXILIAIRES
  if ( Spx->ChangementDeBase == OUI_SPX ) {
    /* Si la variable entrante avait sur une borne sup auxiliaire on libere la borne */
		if ( Spx->NombreDeBornesAuxiliairesUtilisees > 0 ) SPX_SupprimerUneBorneAuxiliaire( Spx, VariableEntrante );
  }
# endif  

SeuilDeViolationDeBorne      = Spx->SeuilDeViolationDeBorne;
Xmax                         = Spx->Xmax;
TypeDeVariable               = Spx->TypeDeVariable;

BBarre    = Spx->BBarre;
DualPoids = Spx->DualPoids;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

NbSurv = Spx->NombreDeContraintesASurveiller;
IndexDansContrainteASurveiller   = Spx->IndexDansContrainteASurveiller;
NumerosDesContraintesASurveiller = Spx->NumerosDesContraintesASurveiller;
ValeurDeViolationDeBorne         = Spx->ValeurDeViolationDeBorne;

for ( k = 0 ; k < NbBBarreModifies ; k++ ) {
  Index = IndexDeBBarreModifies[k];
	T[Index] = -1;
	Viole  = NON_SPX;
  ValeurDeBBarre = BBarre[Index];
	
  if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
    Var = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[Index]];
	}
	else {
    Var = VariableEnBaseDeLaContrainte[Index];
	}
		
	if ( Var == VariableSortante ) Var = VariableEntrante;

  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
	
		# if PRICING_AVEC_VIOLATIONS_STRICTES == OUI_SPX
			if ( SeuilDeViolationDeBorne[Var] > MargeAbsolue ) BorInf = -MargeAbsolue;							
			else BorInf = -SeuilDeViolationDeBorne[Var];							
    # else			
      BorInf = -SeuilDeViolationDeBorne[Var];			
		# endif			

	  if ( ValeurDeBBarre < BorInf ) {		
	    Viole = OUI_SPX;

			# if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX			
	      X = ( ValeurDeBBarre * ValeurDeBBarre ) / DualPoids[Index];
			# else
	      X = ValeurDeBBarre * ValeurDeBBarre;
			# endif
			
		}		
				
  }
  else if ( TypeDeVariable[Var] == BORNEE ) {
	
		# if PRICING_AVEC_VIOLATIONS_STRICTES == OUI_SPX
			if ( SeuilDeViolationDeBorne[Var] > MargeAbsolue ) {									
        BorInf = -MargeAbsolue;								
	      BorSup = Xmax[Var] + MargeAbsolue;				
			}
			else {				
        BorInf = -SeuilDeViolationDeBorne[Var];								
	      BorSup = Xmax[Var] + SeuilDeViolationDeBorne[Var];								
			}
	  # else			
      BorInf = -SeuilDeViolationDeBorne[Var];			
	    BorSup = Xmax[Var] + SeuilDeViolationDeBorne[Var];
		# endif

	  if ( ValeurDeBBarre < BorInf ) {		
	    Viole = OUI_SPX;

			# if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX			
	      X = ( ValeurDeBBarre * ValeurDeBBarre ) / DualPoids[Index];
			# else
	      X = ValeurDeBBarre * ValeurDeBBarre;
			# endif
			
		}
		 
	  else if ( ValeurDeBBarre > BorSup ) {	
		  Viole = OUI_SPX;
	    X = ValeurDeBBarre - Xmax[Var];						
			# if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX			
	      X = ( X * X ) / DualPoids[Index];
			# else
	      X = X * X;
			# endif
			
		}
	}
					
 	i = IndexDansContrainteASurveiller[Index];
	if ( Viole == NON_SPX ) {
		/* On enleve la contrainte de la liste */
	  if ( i >= 0 ) {		
		  /* On enleve la contrainte de la liste */
			IndexDansContrainteASurveiller[Index] = -1;
			Index1 = NumerosDesContraintesASurveiller[NbSurv-1];
			if ( Index !=Index1 ) {
			  NumerosDesContraintesASurveiller[i] = Index1;
			  ValeurDeViolationDeBorne        [i] = ValeurDeViolationDeBorne[NbSurv-1];							
			  IndexDansContrainteASurveiller[Index1] = i;
			}				
			NbSurv--;
		}    
	}
	else {
	  if ( i < 0 ) {		
      /* La Contrainte est nouvellement violee */
		  /* On ajoute la contrainte dans la liste */
			NumerosDesContraintesASurveiller[NbSurv] = Index;			
			ValeurDeViolationDeBorne        [NbSurv] = X;			
			IndexDansContrainteASurveiller[Index] = NbSurv;
		  NbSurv++;
    }
		else {		
      /* La variable est deja dans la liste: on actualise la valeur */
			ValeurDeViolationDeBorne[i] = X;
		} 
	}
}

Spx->NombreDeContraintesASurveiller = NbSurv;

return;
}


