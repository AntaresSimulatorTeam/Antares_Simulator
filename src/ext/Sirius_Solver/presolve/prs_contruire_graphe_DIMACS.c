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

# ifdef ON_COMPILE

/***********************************************************************

   FONCTION: Construction du graphe au format DIMACS					 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PRS_ConstruireLeGrapheAuFormatDIMACS( PRESOLVE * Presolve )    
{
int NombreDeVariables; int NombreDeContraintes; int * Mdeb; int * NbTerm;
int * Nuvar; double * A; double * CoutLineaire; int Cnt; int i; double Ai;
int * Cdeb; int * Csui; int * NumContrainte; int DerniereCouleurDeLaMatrice;
char * SensContrainte; double * B; int Var; int NombreDArcs; int PremierCouleurDeLaMatrice;
int * TypeDeVariable; int * TypeDeBorne; int DerniereCouleurDesContraintes;
double * Xmin; double * Xmax; int PremiereCouleurDeContraintes; int Cn;
char * ContrainteInactive; int il; int ilMax; char Sens; double Bi;
int PremiereCouleurDesContraintes; int DerniereCouleurDesVariables;
int PremiereCouleurDesVariables; char * SensContrainteDeLaCouleur;
double * ValeurDeLaCouleur; int * VertexDeLaVariable; int * VertexDeLaContrainte;
int * VertexDuTerme; int C; int NombreDeCouleurs; int * CouleurDuNoeud;
PROBLEME_PNE * Pne; FILE * Flot; int NombreDeNoeuds; int NbTermesMatrice;

printf("PRS_ConstruireLeGrapheAuFormatDIMACS\n");

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

Flot = fopen( "Graphe_DIMACS_Du_Probleme", "w" ); 
if( Flot == NULL ) {
  printf("Erreur ouverture du fichier pour l'ecriture du jeu de donnees \n");
  exit(0);
}

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
CoutLineaire = Pne->LTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
ContrainteInactive = Presolve->ContrainteInactive;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

NbTermesMatrice = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
  il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
		if ( A[il] != 0.0 ) NbTermesMatrice++;
    il++;
	}
}

NombreDeNoeuds = NombreDeVariables + NombreDeContraintes + NbTermesMatrice;

VertexDeLaVariable = (int *) malloc( NombreDeVariables * sizeof( int ) );
VertexDeLaContrainte = (int *) malloc( NombreDeContraintes * sizeof( int ) );
VertexDuTerme = (int *) malloc( NbTermesMatrice * sizeof( int ) );
CouleurDuNoeud = (int *) malloc( NombreDeNoeuds * sizeof( int ) );

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) VertexDeLaVariable[Var] = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) VertexDeLaContrainte[Cnt] = -1;
for ( il = 0 ; il < NbTermesMatrice ; il++ ) VertexDuTerme[il] = -1;

for ( i = 0 ; i < NombreDeNoeuds ; i++ ) CouleurDuNoeud[i] = -1;

NombreDeCouleurs = NombreDeNoeuds;

SensContrainteDeLaCouleur = (char *) malloc( NombreDeCouleurs * sizeof( char ) );
ValeurDeLaCouleur = (double *) malloc( NombreDeCouleurs * sizeof( double ) );

for ( C = 0 ; C < NombreDeCouleurs ; C++ ) {
  ValeurDeLaCouleur[C] = -1;
  SensContrainteDeLaCouleur[C] = '=';
}		

/* Toutes les variables entieres ont la meme couleur */
NombreDeNoeuds = 0;
NombreDeCouleurs = 0;
PremiereCouleurDesVariables = NombreDeCouleurs;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeVariable[Var] != ENTIER ) continue;
  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;
	VertexDeLaVariable[Var] = NombreDeNoeuds;
	CouleurDuNoeud[NombreDeNoeuds] = NombreDeCouleurs;
	NombreDeNoeuds++;
}
/* Les variables continues */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  /* On pourrait regrouper les variables continues qui ont les memes bornes et le meme cout */
  if ( TypeDeVariable[Var] == ENTIER ) continue;
  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;
	VertexDeLaVariable[Var] = NombreDeNoeuds;
	CouleurDuNoeud[NombreDeNoeuds] = NombreDeCouleurs;
	NombreDeNoeuds++;
	NombreDeCouleurs++;
}
DerniereCouleurDesVariables = NombreDeCouleurs - 1;
PremiereCouleurDesContraintes = NombreDeCouleurs;
/* Le second membre */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
	Bi = B[Cnt];
	Sens = SensContrainte[Cnt];
	Cn = -1;
	for ( C = PremiereCouleurDeContraintes ; C < NombreDeCouleurs ; C++ ) {
    if ( Bi == ValeurDeLaCouleur[C] ) {
		  if ( Sens == SensContrainteDeLaCouleur[C] ) {
			  Cn = C;
				break;
			}
		}
	}
	if ( Cn == -1 ) {
    /* Nouvelle couleur */
		Cn = NombreDeCouleurs;
	  ValeurDeLaCouleur[Cn] = Bi;
	  SensContrainteDeLaCouleur[Cn] = Sens;			
		NombreDeCouleurs++;
	}
	VertexDeLaContrainte[Cnt] = NombreDeNoeuds;
	CouleurDuNoeud[NombreDeNoeuds] = Cn;
	NombreDeNoeuds++;  
}
DerniereCouleurDesContraintes = NombreDeCouleurs - 1;
PremierCouleurDeLaMatrice = NombreDeCouleurs;
NombreDArcs = 0;
/* Les lignes de la matrice */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
  il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
		Ai = A[il];
		if ( Ai != 0.0 && TypeDeBorne[Nuvar[il]] != VARIABLE_FIXE ) {
	    /* Recherche de la couleur */
		  Cn = -1;
		  for ( C = PremierCouleurDeLaMatrice ; C < NombreDeCouleurs ; C++ ) {
        if ( Ai == ValeurDeLaCouleur[C] ) {
			    Cn = C;
					break;
			  }
		  }
		  if ( Cn == -1 ) {
        /* Nouvelle couleur */
				Cn = NombreDeCouleurs;
			  ValeurDeLaCouleur[Cn] = Ai;
			  NombreDeCouleurs++;
		  }
      VertexDuTerme[il] = NombreDeNoeuds;
			/*printf("VertexDuTerme[il] %d\n",VertexDuTerme[il]);*/
		  CouleurDuNoeud[NombreDeNoeuds] = Cn;
		  NombreDeNoeuds++;
			/* Un arc entre la variable et ce noeud et un arc ent ce noeud et la contrainte */
			NombreDArcs += 2;
	  }
		il++;
	}
}
DerniereCouleurDeLaMatrice = NombreDeCouleurs - 1;

/* Ecrire du titre */
fprintf(Flot,"p edge %d %d\n",NombreDeNoeuds,NombreDArcs);

for ( i = 0 ; i < NombreDeNoeuds ; i++ ) {
  fprintf(Flot,"n %d %d\n",i+1,CouleurDuNoeud[i]+1);
}

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
  il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
		Ai = A[il];
		if ( Ai != 0.0 && TypeDeBorne[Nuvar[il]] != VARIABLE_FIXE ) {
			fprintf(Flot,"e %d %d\n",VertexDeLaVariable[Nuvar[il]]+1,VertexDuTerme[il]+1);
			fprintf(Flot,"e %d %d\n",VertexDuTerme[il]+1,VertexDeLaContrainte[Cnt]+1);			
	  }
		il++;
	}
}

fclose( Flot );

free( VertexDeLaVariable );
free( VertexDeLaContrainte );
free( VertexDuTerme );
free( CouleurDuNoeud );
free( SensContrainteDeLaCouleur );
free( ValeurDeLaCouleur );

return;
}

# endif
