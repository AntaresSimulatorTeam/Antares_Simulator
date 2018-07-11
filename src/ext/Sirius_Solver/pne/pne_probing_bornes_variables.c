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

   FONCTION: Detection des contraintes de borne variable pendant le
	           variable probing.
						 Soient y la variable binaire instanciee et x la variable
						 continue. On genere des contraintes de type/
						 x <= xmax + (xmaxNew-xmax) * y si la borne change pour y = 1
						 x <= xmaxNew + (xmax-xmaxNew) * y si la borne change pour y = 0
						 x >= xmin + (xminNew-xmin) * y si la borne change pour y = 1
						 x >= xminNew + (xmin-xminNew) * y si la borne change pour y = 0
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define VERBOSE_CONTRAINTES_DE_BORNE  NON_PNE

# define MAX_CONTRAINTES_DE_BORNE_VARIABLE  50000
# define EGALITE 0
# define BORNE_SUP 1
# define BORNE_INF 2
# define MARGE 1 /*10*/
# define MARGE_SUR_NOUVELLE_BORNE 0.e-9 /* Il vaut mieux mettre 0 car si le B vaut 0 ca fait un Epsilon qui perturbe le calcul */
# define SEUIL_DE_REDUCTION_DE_PLAGE 0.1 /*0.5*/

# define SIZE_ALLOC_CONTRAINTES 1000 /* Nombre de contraintes allouees */
# define SIZE_ALLOC_TERMES_CONTRAINTES (SIZE_ALLOC_CONTRAINTES*2)

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE

void PNE_ProbingAllocContraintesDeBorneVariable( PROBLEME_PNE * );
void PNE_AugmenterNombreDeContraintesDeBorneVariable( PROBLEME_PNE * );
void PNE_AugmenterLaTailleDesContraintesDeBorneVariable( PROBLEME_PNE * );
void PNE_ProbingEtablirContraineDeBornes( PROBLEME_PNE * , int , int , double , double , double , char );
																					
/*----------------------------------------------------------------------------*/
/* On cherche simplement a savoir s'il y a une contrainte de borne variable
   sur la variable continue */
void PNE_ProbingInitDetectionDesBornesVariables( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int Var; int NbBin; int NombreDeContraintes; int NbCont;
int VarCont; double CoeffCont; int * Mdeb; int * NbTerm; int NombreDeVariables;
int * Nuvar; int * TypeDeVariable; double * A;  int * CntDeBorneSupVariable;
int * CntDeBorneInfVariable; char * SensContrainte; int * TypeDeBorne; double * Umin;
double * Umax; double * B;

# if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE		  
  return;
# endif

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

if ( Pne->CntDeBorneSupVariable == NULL ) {   
  Pne->CntDeBorneSupVariable  = (int *) malloc( NombreDeVariables * sizeof( int ) );
  if ( Pne->CntDeBorneSupVariable == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_ProbingDetectionDesBornesVariables \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}
if ( Pne->CntDeBorneInfVariable == NULL ) {   
  Pne->CntDeBorneInfVariable  = (int *) malloc( NombreDeVariables * sizeof( int ) );
  if ( Pne->CntDeBorneInfVariable == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_ProbingDetectionDesBornesVariables \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}

CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;
CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) { CntDeBorneSupVariable[Var] = -1; CntDeBorneInfVariable[Var] = -1; }

SensContrainte = Pne->SensContrainteTrav;
NbTerm = Pne->NbTermTrav;   
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
Nuvar = Pne->NuvarTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
A = Pne->ATrav;

/* Marquage des variables continues soumises a une borne sup ou inf variable */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SensContrainte[Cnt] != '<' ) continue;
  if ( NbTerm[Cnt] != 2 ) continue;
	NbCont = 0;
  CoeffCont = 0.0;
	NbBin = 0;
	VarCont = -1;
  il = Mdeb[Cnt];
  ilMax = il + 2;	
  while ( il < ilMax ) { 
    Var = Nuvar[il];
    if ( TypeDeBorne[Var] != VARIABLE_FIXE ) {
      if ( TypeDeVariable[Var] == ENTIER ) {
			  NbBin++;
				if ( NbBin > 1 ) break;
			}
      else {
	      NbCont++;
				VarCont = Var;
				CoeffCont = A[il];
				if ( NbCont > 1 ) break;
			}
		}
    il++;
  }
	if ( NbCont == 1 && NbBin == 1 && VarCont != -1 ) {		
		/* C'est une contrainte de borne variable */
		if ( CoeffCont > 0.0 ) CntDeBorneSupVariable[VarCont] = Cnt;
		else CntDeBorneInfVariable[VarCont] = Cnt;
	}
}

return;
}
/*----------------------------------------------------------------------------*/
void PNE_ProbingCloseDetectionDesBornesVariables( PROBLEME_PNE * Pne )
{
# if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE		  
  return;
# endif
free( Pne->CntDeBorneSupVariable );
free( Pne->CntDeBorneInfVariable );
Pne->CntDeBorneSupVariable = NULL;
Pne->CntDeBorneInfVariable = NULL;
return;
}

/*----------------------------------------------------------------------------*/
void PNE_ProbingAllocContraintesDeBorneVariable( PROBLEME_PNE * Pne )
{
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable;
ContraintesDeBorneVariable = (CONTRAITES_DE_BORNE_VARIABLE *) malloc( sizeof( CONTRAITES_DE_BORNE_VARIABLE ) );
if ( ContraintesDeBorneVariable == NULL ) return;

ContraintesDeBorneVariable->SecondMembre = (double *) malloc( SIZE_ALLOC_CONTRAINTES * sizeof( double ) );
if ( ContraintesDeBorneVariable->SecondMembre == NULL ) {
  return;
}
ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool = (char *) malloc( SIZE_ALLOC_CONTRAINTES * sizeof( char ) );
if ( ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool == NULL ) {
  free( ContraintesDeBorneVariable->SecondMembre );
	return;
}
ContraintesDeBorneVariable->First = (int *) malloc( SIZE_ALLOC_CONTRAINTES * sizeof( int ) );
if ( ContraintesDeBorneVariable->First == NULL ) {
  free( ContraintesDeBorneVariable->SecondMembre );	
	free( ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool ); 
  return;
}
ContraintesDeBorneVariable->NombreDeContraintesDeBorneAlloue = SIZE_ALLOC_CONTRAINTES;

ContraintesDeBorneVariable->Colonne = (int *) malloc( SIZE_ALLOC_TERMES_CONTRAINTES * sizeof( int ) );
if ( ContraintesDeBorneVariable->Colonne == NULL ) {
  free( ContraintesDeBorneVariable->SecondMembre );	
	free( ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool ); 
	free( ContraintesDeBorneVariable->First ); 
  return;
}
ContraintesDeBorneVariable->Coefficient = (double *) malloc( SIZE_ALLOC_TERMES_CONTRAINTES * sizeof( double ) );
if ( ContraintesDeBorneVariable->Coefficient == NULL ) {
  free( ContraintesDeBorneVariable->SecondMembre );	
	free( ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool ); 
	free( ContraintesDeBorneVariable->First ); 
	free( ContraintesDeBorneVariable->Colonne ); 
  return;
}
ContraintesDeBorneVariable->TailleContraintesDeBorneAllouee = SIZE_ALLOC_TERMES_CONTRAINTES;
ContraintesDeBorneVariable->IndexLibre = 0;
ContraintesDeBorneVariable->NombreDeContraintesDeBorne = 0;
ContraintesDeBorneVariable->Full = NON_PNE;
Pne->ContraintesDeBorneVariable = ContraintesDeBorneVariable;
return;
}
/*----------------------------------------------------------------------------*/
void PNE_AugmenterNombreDeContraintesDeBorneVariable( PROBLEME_PNE * Pne )
{
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int Size; double * SecondMembre;
char * LaContrainteDeBorneVariableEstDansLePool; int * First; 

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;
Size = ContraintesDeBorneVariable->NombreDeContraintesDeBorne + SIZE_ALLOC_CONTRAINTES;
SecondMembre = (double *) realloc( ContraintesDeBorneVariable->SecondMembre, Size * sizeof( double ) );
if ( SecondMembre == NULL ) {
	ContraintesDeBorneVariable->Full = OUI_PNE;
  return;
}
LaContrainteDeBorneVariableEstDansLePool = (char *) realloc( ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool, Size * sizeof( char ) );
if ( LaContrainteDeBorneVariableEstDansLePool == NULL ) {
  free( SecondMembre );
	ContraintesDeBorneVariable->Full = OUI_PNE;
	return;
}
First = (int *) realloc( ContraintesDeBorneVariable->First, Size * sizeof( int ) );
if ( First == NULL ) {
  free( SecondMembre );	
	free( LaContrainteDeBorneVariableEstDansLePool ); 
	ContraintesDeBorneVariable->Full = OUI_PNE;
  return;
}
ContraintesDeBorneVariable->NombreDeContraintesDeBorneAlloue = Size;
ContraintesDeBorneVariable->SecondMembre = SecondMembre;
ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool = LaContrainteDeBorneVariableEstDansLePool;
ContraintesDeBorneVariable->First = First;
return;
}
/*----------------------------------------------------------------------------*/
void PNE_AugmenterLaTailleDesContraintesDeBorneVariable( PROBLEME_PNE * Pne )
{
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int Size; int * Colonne; double * Coefficient;

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;
Size = ContraintesDeBorneVariable->TailleContraintesDeBorneAllouee + SIZE_ALLOC_TERMES_CONTRAINTES;

Colonne = (int *) realloc( ContraintesDeBorneVariable->Colonne, Size * sizeof( int ) );
if ( Colonne == NULL ) {
	ContraintesDeBorneVariable->Full = OUI_PNE;
  return;
}
Coefficient = (double *) realloc( ContraintesDeBorneVariable->Coefficient, Size * sizeof( double ) );
if ( Coefficient == NULL ) {
	free( Colonne ); 
	ContraintesDeBorneVariable->Full = OUI_PNE;
  return;
}
ContraintesDeBorneVariable->TailleContraintesDeBorneAllouee = Size;
ContraintesDeBorneVariable->Colonne = Colonne;
ContraintesDeBorneVariable->Coefficient = Coefficient;
return;
}
/*----------------------------------------------------------------------------*/
void PNE_ProbingEtablirContraineDeBornes( PROBLEME_PNE * Pne, int Var, int VariableInstanciee,
                                          double a0, double a, double b, char Borne )
{  
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int il; int Cnt;
/* On regarde si la variable entiere servait deja a construite une contrainte de borne */
if ( Borne == BORNE_SUP ) Cnt = Pne->CntDeBorneSupVariable[Var];
else if ( Borne == BORNE_INF ) Cnt = Pne->CntDeBorneInfVariable[Var];
else Cnt = -1;
if ( Cnt >= 0 ) {
  il = Pne->MdebTrav[Cnt];
  if ( Pne->NuvarTrav[il] == VariableInstanciee || Pne->NuvarTrav[il+1] == VariableInstanciee ) return;
}
if ( Pne->ContraintesDeBorneVariable == NULL ) {			
  PNE_ProbingAllocContraintesDeBorneVariable( Pne );
  if ( Pne->ContraintesDeBorneVariable == NULL ) return; /* Saturation memoire */		
}

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;

/* Place suffisante pour les contraintes */
if ( ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= ContraintesDeBorneVariable->NombreDeContraintesDeBorneAlloue ) {
  PNE_AugmenterNombreDeContraintesDeBorneVariable( Pne );		
  if ( Pne->ContraintesDeBorneVariable == NULL ) return;
  if ( ContraintesDeBorneVariable->Full == OUI_PNE ) return;
}
/* Place suffisante pour les termes */
if ( ContraintesDeBorneVariable->IndexLibre + 2 >= ContraintesDeBorneVariable->TailleContraintesDeBorneAllouee ) {
  PNE_AugmenterLaTailleDesContraintesDeBorneVariable( Pne );			 
  if ( Pne->ContraintesDeBorneVariable == NULL ) return;	 
  if ( ContraintesDeBorneVariable->Full == OUI_PNE ) return;
}
il = ContraintesDeBorneVariable->IndexLibre;
ContraintesDeBorneVariable->First[ContraintesDeBorneVariable->NombreDeContraintesDeBorne] = il;
/* La variable continue en premier */
ContraintesDeBorneVariable->Coefficient[il] = a0;
ContraintesDeBorneVariable->Colonne[il] = Var;
il++;
ContraintesDeBorneVariable->Coefficient[il] = a;
ContraintesDeBorneVariable->Colonne[il] = VariableInstanciee;
il++;
ContraintesDeBorneVariable->IndexLibre = il;
ContraintesDeBorneVariable->SecondMembre[ContraintesDeBorneVariable->NombreDeContraintesDeBorne] = b;
ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool[ContraintesDeBorneVariable->NombreDeContraintesDeBorne] = NON_PNE;
ContraintesDeBorneVariable->NombreDeContraintesDeBorne += 1;			
return;
}
/*----------------------------------------------------------------------------*/

void PNE_ProbingConstruireContraintesDeBornes( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve,
                                               double * Xmin0, double * Xmax0 )
{
int NombreDeVariables; int Var; int VariableInstanciee; double ValeurDeLaVariableInstanciee;
char * BorneInfConnue; int * TypeDeBorne; int * TypeDeVariable;double * ValeurDeBorneInf;
double * ValeurDeBorneSup; double a; double b; double a0; char Borne; char CreerUneContrainte;
double PlageInitiale; double PlageFinale; double Alpha; double PlusGrandTerme; double PlusPetitTerme;
double BorneSupCandidate; double BorneInfCandidate; char ContrainteCree;

# if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE		  
  return;
# endif

if ( ProbingOuNodePresolve->VariableInstanciee < 0 ) return;

if ( Pne->ContraintesDeBorneVariable != NULL ) {
  if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= MAX_CONTRAINTES_DE_BORNE_VARIABLE ) return;
}

VariableInstanciee = ProbingOuNodePresolve->VariableInstanciee;
ValeurDeLaVariableInstanciee = ProbingOuNodePresolve->ValeurDeLaVariableInstanciee;

BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;  

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;

PlusGrandTerme = Pne->PlusGrandTerme;
PlusPetitTerme = Pne->PlusPetitTerme;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	if ( TypeDeVariable[Var] != REEL ) continue;	
	if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;
  /*
	printf("Variable continue %d xmin %e xmax %e\n",Var,ValeurDeBorneInf[Var],ValeurDeBorneSup[Var]);
  */	
  /* Recherche d'une egalite */
	if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
    if ( ValeurDeBorneSup[Var] == ValeurDeBorneInf[Var] ) {
      if ( ValeurDeBorneSup[Var] != Xmax0[Var] && ValeurDeBorneSup[Var] != Xmin0[Var] ) {
			  ContrainteCree = OUI_PNE;
			  Alpha = ValeurDeBorneSup[Var];
			  Borne = EGALITE;
			  /* On cree 2 contraintes pour construire l'egalite a la valeur Alpha */
				/* Cas y = 1: 
				x <= xmax - (xmax-Alpha) * y  i.e  x + (xmax-Alpha) * y <= xmax
				x >= xmin + (Alpha - xmin) * y i.e. -x + (Alpha - xmin) * y <= -xmin
           Cas y = 0:
				x - (xmax-Alpha) * y <= Alpha
				-x - (Alpha - xmin) * y <= -Alpha */
				
        a0 = 1;
        if ( ValeurDeLaVariableInstanciee == 1 ) { a = Xmax0[Var] - Alpha; b = Xmax0[Var]; }
				else if ( ValeurDeLaVariableInstanciee == 0 ) { a = -(Xmax0[Var] - Alpha); b = Alpha; }
				else {
				  printf("Bug dans PNE_ProbingConstruireContraintesDeBornes ValeurDeLaVariableInstanciee %e\n",ValeurDeLaVariableInstanciee);
					continue;
				}
				
				if ( fabs( a ) >= PlusPetitTerme && fabs( a ) <= PlusGrandTerme ) {																	
			    Borne = EGALITE;
          PNE_ProbingEtablirContraineDeBornes( Pne, Var, VariableInstanciee, a0, a, b, Borne );
          if ( Pne->ContraintesDeBorneVariable != NULL ) {
            if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= MAX_CONTRAINTES_DE_BORNE_VARIABLE ) return;
          }
				}
				else ContrainteCree = NON_PNE;
				
        a0 = -1;
        if ( ValeurDeLaVariableInstanciee == 1 ) { a = Alpha - Xmin0[Var]; b = -Xmin0[Var]; }
				else if ( ValeurDeLaVariableInstanciee == 0 ) { a = -(Alpha - Xmin0[Var]); b = -Alpha; }
				else {
				  printf("Bug dans PNE_ProbingConstruireContraintesDeBornes ValeurDeLaVariableInstanciee %e\n",ValeurDeLaVariableInstanciee);
					continue;
				}
				if ( fabs( a ) >= PlusPetitTerme && fabs( a ) <= PlusGrandTerme ) {													
			    Borne = EGALITE;
          PNE_ProbingEtablirContraineDeBornes( Pne, Var, VariableInstanciee, a0, a, b, Borne );
          if ( Pne->ContraintesDeBorneVariable != NULL ) {
            if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= MAX_CONTRAINTES_DE_BORNE_VARIABLE ) return;
          }						
				}
				else ContrainteCree = NON_PNE;

				if ( ContrainteCree == OUI_PNE ) {
	        # if VERBOSE_CONTRAINTES_DE_BORNE == OUI_PNE			
            printf("Contrainte pour fixer la variable %d a %e  (xmax: %e xmin: %e) si la variable entier %d passe a %e\n",
					          Var,Alpha,Xmax0[Var],Xmin0[Var],VariableInstanciee,ValeurDeLaVariableInstanciee);									 
	        # endif
				  continue;
				}
				
	    }
    }
	}	
	/* Recherche d'une borne sup */
	if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
	  CreerUneContrainte = NON_PNE;
		BorneSupCandidate = ValeurDeBorneSup[Var] + MARGE_SUR_NOUVELLE_BORNE;
	  if ( BorneSupCandidate < Xmax0[Var] - MARGE ) CreerUneContrainte = OUI_PNE;
		if ( CreerUneContrainte == NON_PNE && TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
      PlageInitiale = Xmax0[Var] - Xmin0[Var];
      PlageFinale = BorneSupCandidate - Xmin0[Var];
		  if ( PlageFinale - PlageInitiale > SEUIL_DE_REDUCTION_DE_PLAGE * PlageInitiale ) CreerUneContrainte = OUI_PNE;		
		}		
	  if ( CreerUneContrainte == OUI_PNE ) {
		  a0 = 1;
      if ( ValeurDeLaVariableInstanciee == 1 ) {
			  b = Xmax0[Var];
				a = -(BorneSupCandidate - Xmax0[Var]);					
			}
			else if ( ValeurDeLaVariableInstanciee == 0 ) {				
			  b = BorneSupCandidate;
				a = -(Xmax0[Var] - BorneSupCandidate);							
			}
			else {
			  printf("Bug dans PNE_ProbingConstruireContraintesDeBornes ValeurDeLaVariableInstanciee %e\n",ValeurDeLaVariableInstanciee);
				continue;
			}
			/* Pour ne pas trop detruire le conditionnement */
			if ( fabs( a ) < PlusPetitTerme || fabs( a ) > PlusGrandTerme ) CreerUneContrainte = NON_PNE;						
			if ( CreerUneContrainte == OUI_PNE ) {
	      # if VERBOSE_CONTRAINTES_DE_BORNE == OUI_PNE
          printf("Contrainte de borne sup variable sur %d xmax: %e -> %e (xmin = %e) variable instancies %d a %e\n",
			            Var,Xmax0[Var],BorneSupCandidate,Xmin0[Var],VariableInstanciee,ValeurDeLaVariableInstanciee);
	      # endif
			  Borne = BORNE_SUP;
        PNE_ProbingEtablirContraineDeBornes( Pne, Var, VariableInstanciee, a0, a, b, Borne );
        if ( Pne->ContraintesDeBorneVariable != NULL ) {
          if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= MAX_CONTRAINTES_DE_BORNE_VARIABLE ) return;
        }
			} 
    }
	}
	/* Recherche d'une borne inf */
	if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
	  CreerUneContrainte = NON_PNE;
		BorneInfCandidate = ValeurDeBorneInf[Var] - MARGE_SUR_NOUVELLE_BORNE;
    if ( BorneInfCandidate > Xmin0[Var] + MARGE ) CreerUneContrainte = OUI_PNE;
		if ( CreerUneContrainte == NON_PNE && TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
      PlageInitiale = Xmax0[Var] - Xmin0[Var];
      PlageFinale = Xmax0[Var] - BorneInfCandidate;
		  if ( PlageFinale - PlageInitiale > SEUIL_DE_REDUCTION_DE_PLAGE * PlageInitiale ) CreerUneContrainte = OUI_PNE;		
    }		
	  if ( CreerUneContrainte == OUI_PNE ) {
	    /* On regarde si la variable entiere servait deja a construite une contrainte de borne */
		  a0 = -1;
      if ( ValeurDeLaVariableInstanciee == 1 ) {
		    b = -Xmin0[Var];
			  a = BorneInfCandidate - Xmin0[Var];												
		  }
		  else if ( ValeurDeLaVariableInstanciee == 0 ) {
		    b = -BorneInfCandidate;
			  a = Xmin0[Var] - BorneInfCandidate;
		  }
			else {
			  printf("Bug dans PNE_ProbingConstruireContraintesDeBornes ValeurDeLaVariableInstanciee %e\n",ValeurDeLaVariableInstanciee);
				continue;
			}
			/* Pour ne pas trop detruire le conditionnement */
			if ( fabs( a ) < PlusPetitTerme || fabs( a ) > PlusGrandTerme ) CreerUneContrainte = NON_PNE;									
			if ( CreerUneContrainte == OUI_PNE ) {			
	      # if VERBOSE_CONTRAINTES_DE_BORNE == OUI_PNE
          printf("Contrainte de borne inf variable sur %d xmin: %e -> %e (xmax = %e) variable instancies %d a %e\n",
			            Var,Xmin0[Var],BorneInfCandidate,Xmax0[Var],VariableInstanciee,ValeurDeLaVariableInstanciee);
	 	    # endif
			  Borne = BORNE_INF;
        PNE_ProbingEtablirContraineDeBornes( Pne, Var, VariableInstanciee, a0, a, b, Borne );			      	
        if ( Pne->ContraintesDeBorneVariable != NULL ) {
          if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne >= MAX_CONTRAINTES_DE_BORNE_VARIABLE ) return;
        }
			}
	  }
  }
}

return;
}

/*----------------------------------------------------------------------------*/

# endif
