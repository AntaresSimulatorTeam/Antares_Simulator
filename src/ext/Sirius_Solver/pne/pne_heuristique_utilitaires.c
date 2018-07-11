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

   FONCTION: Utilitaires de creation de matrice de contraintes pour les
	           heuristiques.
						 Attention, lorsque ce module est appele les variables
						 UminTrav UmaxTrav et TypeDeBorneTrav on ete remises a leur
						 valeur du noeud racine. Donc leurs valeurs n'inclut pas
						 les instanciations faites en amont du noeud a partir duquel
						 on demarre.
						 Par contre les structures du simplexe se trouvent dans l'etat
						 du dernier noeud resolu (bornes et types de bornes).						 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define EPS_COUT_OPT     1.e-4
# define MARGE_OPTIMALITE 1.e-6

/*----------------------------------------------------------------------------*/
void PNE_HeuristiqueLibererMatriceDeContraintes( MATRICE_DE_CONTRAINTES * Contraintes ) 
{
if ( Contraintes == NULL ) return;
free( Contraintes->IndexDebut );
free( Contraintes->NombreDeTermes );
free( Contraintes->SecondMembre );
free( Contraintes->Sens );
free( Contraintes->Colonne );
free( Contraintes->Coefficient );
free( Contraintes );
return;
}
/*----------------------------------------------------------------------------*/

MATRICE_DE_CONTRAINTES * PNE_HeuristiqueConstruireMatriceDeContraintes( PROBLEME_PNE * Pne ) 
{
int NombreDeContraintesNatives; int NbTermesNecessaires; int * IndexDebut; int * NombreDeTermes;
int NombreDeVariablesNatives; int il; int NbTermesNatif; int NombreDeContraintes;
int NbContraintesNecessaires; double * SecondMembre; char * Sens;  int * Colonne; double * Coefficient;
MATRICE_DE_CONTRAINTES * Contraintes;
# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == NON_PNE
  double Marge; int Nb; double * L; int Var; 
# endif
# if UTILISER_LES_COUPES == OUI_PNE
  int Cnt; int ilC; int ilCmax;
# endif

Contraintes = NULL;
/* Allocation */
Contraintes = (MATRICE_DE_CONTRAINTES *) malloc( sizeof( MATRICE_DE_CONTRAINTES ) );
if ( Contraintes == NULL ) return( NULL );

NombreDeContraintesNatives = Pne->NombreDeContraintesTrav;
NombreDeVariablesNatives = Pne->NombreDeVariablesTrav;
NbContraintesNecessaires = NombreDeContraintesNatives;
if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
  /* Pour la borne sur le cout */
	NbContraintesNecessaires++;
}
# if UTILISER_LES_COUPES == OUI_PNE
  NbContraintesNecessaires += Pne->Coupes.NombreDeContraintes;
# endif

NbTermesNatif = Pne->TailleAlloueePourLaMatriceDesContraintes;

NbTermesNecessaires = NbTermesNatif;

# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == OUI_PNE
  if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
    /* Pour la borne sur le cout */
	  NbTermesNecessaires += NombreDeVariablesNatives;
  }
# endif

# if UTILISER_LES_COUPES == OUI_PNE
  /* Les coupes */
  for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) NbTermesNecessaires += Pne->Coupes.Mdeb[Cnt] + Pne->Coupes.NbTerm[Cnt];
# endif

IndexDebut = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
NombreDeTermes = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
SecondMembre = (double *) malloc( NbContraintesNecessaires * sizeof( double ) );
Sens = (char *) malloc( NbContraintesNecessaires * sizeof( char ) );
Colonne = (int *) malloc( NbTermesNecessaires * sizeof( int ) );
Coefficient = (double *) malloc( NbTermesNecessaires * sizeof( double ) );
if ( IndexDebut == NULL || NombreDeTermes == NULL || SecondMembre == NULL || Sens == NULL || Colonne == NULL || Coefficient == NULL ) {
  free( IndexDebut ); free( NombreDeTermes ); free( SecondMembre ); free( Sens ); free( Colonne ); free( Coefficient );
	free( Contraintes );
	return( NULL );
}
Contraintes->IndexDebut = IndexDebut;
Contraintes->NombreDeTermes = NombreDeTermes;
Contraintes->SecondMembre = SecondMembre;
Contraintes->Sens = Sens;
Contraintes->Colonne = Colonne;
Contraintes->Coefficient = Coefficient;

/* Recopie de la matrice des contraintes natives */
/* Attention on suppose que les contraintes sont rangees dans l'ordre */
memcpy( (char *) IndexDebut, (char *) Pne->MdebTrav, NombreDeContraintesNatives * sizeof( int ) );
memcpy( (char *) NombreDeTermes, (char *) Pne->NbTermTrav, NombreDeContraintesNatives * sizeof( int ) );
memcpy( (char *) SecondMembre, (char *) Pne->BTrav, NombreDeContraintesNatives * sizeof( double ) );
memcpy( (char *) Sens, (char *) Pne->SensContrainteTrav, NombreDeContraintesNatives * sizeof( char ) );
memcpy( (char *) Colonne, (char *) Pne->NuvarTrav, NbTermesNatif * sizeof( int ) );
memcpy( (char *) Coefficient, (char *) Pne->ATrav, NbTermesNatif * sizeof( double ) );

NombreDeContraintes = NombreDeContraintesNatives;
il = NbTermesNatif;

# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == OUI_PNE
  /* creation de la contrainte de cout max */
  if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
    L = Pne->LTrav;
	  IndexDebut[NombreDeContraintes] = il;
	  Nb = 0;
    for ( Var = 0 ; Var < NombreDeVariablesNatives ; Var++ ) {
      if ( L[Var] != 0.0 ) {
        Colonne[il] = Var;
        Coefficient[il] = L[Var];
			  il++; Nb++;
		  }
	  }
    NombreDeTermes[NombreDeContraintes] = Nb;

	  Marge = EPS_COUT_OPT * fabs( Pne->CoutOpt );

    SecondMembre[NombreDeContraintes] = Pne->CoutOpt - Marge - MARGE_OPTIMALITE;
	  Sens[NombreDeContraintes] = '<';
    NombreDeContraintes++;	
  }
# endif

# if UTILISER_LES_COUPES == OUI_PNE
  /* Les coupes */
  for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) {
	  ilC = Pne->Coupes.Mdeb[Cnt] ;
		ilCmax = ilC + Pne->Coupes.NbTerm[Cnt];
		Nb = 0;
	  IndexDebut[NombreDeContraintes] = il;		
    while ( ilC < ilCmax ) {
      Colonne[il] = Pne->Coupes.Nuvar[ilC];
      Coefficient[il] = Pne->Coupes.A[ilC];      
      ilC++; il++; Nb++;
		}
    NombreDeTermes[NombreDeContraintes] = Nb;
    SecondMembre[NombreDeContraintes] = Pne->Coupes.B[Cnt];
	  Sens[NombreDeContraintes] = '<';
    NombreDeContraintes++;			
	}	
# endif

Contraintes->NombreDeContraintes = NombreDeContraintes;

return( Contraintes );
}

/*----------------------------------------------------------------------------*/
/* On calcule le nombre de variables non fixes et le nombre de contraintes
   qui en en resulte lorque qu'on utilise une heuristique qui consiste a fixer
	 certaines variables */
char PNE_HeuristiqueEvaluerTailleDuProbleme( PROBLEME_PNE * Pne ) 
{
int NbCntRestantes; int NbVarRestantes; int NbNTermesNonNulsRestants; int Cnt;
int il; int ilMax; int * Mdeb; int * NbTerm; int * TypeDeBorne; double * Xmin;
double * Xmax; int Var; int Nbt; int * NuVar; char OK; int NbVarEntieresRestantes;
int * NumerosDesVariablesEntieresTrav;

NbCntRestantes = 0;
NbVarRestantes = 0;
NbVarEntieresRestantes = 0;
NbNTermesNonNulsRestants = 0;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
NuVar = Pne->NuvarTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
NumerosDesVariablesEntieresTrav = Pne->NumerosDesVariablesEntieresTrav;

for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) NbVarRestantes++;
}

for ( il = 0 ; il < Pne->NombreDeVariablesEntieresTrav ; il++ ) {
  Var = NumerosDesVariablesEntieresTrav[il];
  if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) NbVarEntieresRestantes++;
}

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
	Nbt = 0;
	/* On ne compte pas les contraintes a 1 terme car il y a beaucoup de chances pour qu'elles
	   ne soient pas penalisantes */
  while ( il < ilMax ) {
	  Var = NuVar[il];
	  if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) Nbt++;
    il++;
  }

	if ( Nbt > 1 ) {
	  NbNTermesNonNulsRestants += Nbt;
		NbCntRestantes++;
	}
	
}

OK = NON_PNE;
ilMax = Mdeb[Pne->NombreDeContraintesTrav-1] + NbTerm[Pne->NombreDeContraintesTrav-1];
if ( NbNTermesNonNulsRestants < 0.1 * ilMax || 1) {
  OK = OUI_PNE;
	/*printf("On accepte l'heuristique NbNTermesNonNulsRestants %d seuil %d\n",NbNTermesNonNulsRestants, (int) (0.10 * ilMax) );*/
}
else if ( NbCntRestantes < 0.5 * Pne->NombreDeContraintesTrav ) {
  OK = OUI_PNE;
	/*printf("On accepte l'heuristique NbCntRestantes %d seuil %d\n",NbCntRestantes,(int) ( 0.5 * Pne->NombreDeContraintesTrav ));*/
}
else if ( ( NbVarRestantes < (int) ( 0.5 * Pne->NombreDeVariablesNonFixes ) && 
            NbVarEntieresRestantes  < (int) ( 0.1 * Pne->NombreDeVariablesEntieresNonFixes ) ) ||
					( NbVarRestantes <= 100 && NbVarEntieresRestantes <= 10 ) ) {
  OK = OUI_PNE;
	/*
	printf("On accepte l'heuristique NbVarRestantes %d seuil %d NbVarEntieresRestantes %d seuil %d\n",
	        NbVarRestantes,(int) ( 0.5 * Pne->NombreDeVariablesNonFixes ),
					NbVarEntieresRestantes,(int) ( 0.1 * Pne->NombreDeVariablesEntieresNonFixes ) );
	*/
}
if ( OK == NON_PNE ) {
  /*
  printf("NbNTermesNonNulsRestants %d NbVarRestantes %d NbVarEntieresRestantes %d\n",NbNTermesNonNulsRestants,NbVarRestantes,NbVarEntieresRestantes);
  printf("Refus lancement heuristique\n");
	*/
}

return( OK );
}
