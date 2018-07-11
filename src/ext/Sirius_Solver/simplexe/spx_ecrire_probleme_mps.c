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

   FONCTION: Ecrire du probleme au format MPS utilise QUE POUR FAIRE
	           DE LA MISE AU POINT
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        
# include "bb_fonctions.h"

# include "pne_define.h"		        

/*----------------------------------------------------------------------------*/

void SPX_EcrireProblemeAuFormatMPS( PROBLEME_SIMPLEXE Probleme ) 
{
SPX_EcrireJeuDeDonneesLineaireAuFormatMPS(Probleme.NombreDeVariables,Probleme.TypeDeVariable,Probleme.Xmax,Probleme.Xmin,Probleme.CoutLineaire,
		                                  Probleme.NombreDeContraintes,Probleme.SecondMembre,Probleme.Sens,Probleme.IndicesDebutDeLigne,
										  Probleme.NombreDeTermesDesLignes,Probleme.CoefficientsDeLaMatriceDesContraintes,Probleme.IndicesColonnes);
}


void SPX_EcrireProblemeSpxAuFormatMPS( PROBLEME_SPX * Spx )
{
	int i;
	int * TypeDeBorne;
	char * Sens;
	Sens = (char*) malloc( Spx->NombreDeContraintes * sizeof( char ) ); 
	for (i=0; i<Spx->NombreDeContraintes; i++) Sens[i] = '='; // uniquement des contraintes d'égalité
	TypeDeBorne = (int*) malloc ( Spx->NombreDeVariables * sizeof (int) );
	for (i=0; i<Spx->NombreDeVariables; i++) TypeDeBorne[i] = Spx->TypeDeVariable[i]; // conversion des types de borne

	SPX_EcrireJeuDeDonneesLineaireAuFormatMPS(Spx->NombreDeVariables,TypeDeBorne,Spx->Xmax,Spx->Xmin,Spx->C,
		                                      Spx->NombreDeContraintes,Spx->B,Sens,Spx->Mdeb,Spx->NbTerm,Spx->A,Spx->Indcol);

	free ( TypeDeBorne );
	free ( Sens );
}


void SPX_EcrireJeuDeDonneesLineaireAuFormatMPS(int NombreDeVariables, int * TypeDeBorneDeLaVariable, 
											   double * Xmax, double * Xmin, double * CoutLineaire, int NombreDeContraintes,   
											   double * SecondMembre, char * Sens, int * IndicesDebutDeLigne, 
											   int * NombreDeTermesDesLignes,	double * CoefficientsDeLaMatriceDesContraintes, 
											   int * IndicesColonnes)
{

FILE * Flot;
int Cnt; int Var; int il; int ilk; int ilMax; char * Nombre;
int * Cder; int * Cdeb; int * NumeroDeContrainte; int * Csui;

/* Chainage de la transposee */
for ( ilMax = -1 , Cnt = 0 ; Cnt < NombreDeContraintes; Cnt++ ) {
  if ( ( IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1 ) > ilMax ) {
    ilMax = IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1;
  }
}
ilMax+= NombreDeContraintes; /* Marge */

Cder               = (int *) malloc( NombreDeVariables * sizeof( int ) );
Cdeb               = (int *) malloc( NombreDeVariables * sizeof( int ) );
NumeroDeContrainte = (int *) malloc( ilMax             * sizeof( int ) );
Csui               = (int *) malloc( ilMax             * sizeof( int ) );
Nombre             = (char *) malloc( 1024 );
if ( Cder == NULL || Cdeb == NULL || NumeroDeContrainte == NULL || Csui == NULL || Nombre == NULL ) {
  printf("Memoire insuffisante pour ecrire le probleme\n");
	exit(0);
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) Cdeb[Var] = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = IndicesDebutDeLigne[Cnt];
  ilMax = il + NombreDeTermesDesLignes[Cnt];
  while ( il < ilMax ) {
    Var = IndicesColonnes[il];
    if ( Cdeb[Var] < 0 ) {
      Cdeb              [Var] = il;
      NumeroDeContrainte[il]  = Cnt;
      Csui              [il]  = -1;
      Cder              [Var] = il;
    }
    else {
      ilk                     = Cder[Var];
      Csui              [ilk] = il;
      NumeroDeContrainte[il]  = Cnt;
      Csui              [il]  = -1;
      Cder              [Var] = il;
    }
    il++;
  }
}
free( Cder );
/* Fin chainage de la transposee */

/* Fichier qui contiendra le jeu de donnees */
printf("***************************************************************************\n");
printf("*** Vous avez demande la creation d'un fichier contenant la description ***\n");
printf("*** du probleme en cours de resolution. Le fichier de donnees se trouve ***\n");
printf("*** dans le repertoire d'execution. Il s'appelle:                       ***\n");
printf("***                                                                     ***\n");
printf("***                 Donnees_Probleme_Solveur.mps                        ***\n");
printf("***                                                                     ***\n");
printf("*** Si un fichier de ce nom existait deja, il sera ecrase par avec les  ***\n");
printf("*** nouvelles donnees.                                                  ***\n");
printf("***************************************************************************\n");

Flot = fopen( "Donnees_Probleme_Solveur.mps", "w" ); 
if( Flot == NULL ) {
  printf("Erreur ouverture du fichier pour l'ecriture du jeu de donnees \n");
  return;
}
	
/* Ecrire du titre */
fprintf(Flot,"* Number of variables:   %d\n",NombreDeVariables);
fprintf(Flot,"* Number of constraints: %d\n",NombreDeContraintes);

/*
 Les champs du format MPS
Champ1 :  2- 3
Champ2 :  5-12
Champ3 : 15-22
Champ4 : 25-36
Champ5 : 40-47
Champ6 : 50-61
*/

/* NAME */
fprintf(Flot,"NAME          Pb Solve\n");

/* ROWS */
fprintf(Flot,"ROWS\n");
/*
In this section all the row labels are defined, as well as the row type. The row
type is entered in field 1 (in column 2 or 3) and the row label is entered in
field 2 (columns 5-12). Row type:
E : egalité
L : inferieur ou egal
G : superieur ou egal
N : objectif
N : free ??
*/
/* Objectif */
fprintf(Flot," N  OBJECTIF\n");
/* Ecriture de toutes les contraintes */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( Sens[Cnt] == '=' ) {
    fprintf(Flot," E  R%07d\n",Cnt);
  }
  else if (  Sens[Cnt] == '<' ) {
    fprintf(Flot," L  R%07d\n",Cnt);
  }
  else if (  Sens[Cnt] == '>' ) {
    fprintf(Flot," G  R%07d\n",Cnt);
  }
  else {
    fprintf(Flot,"SPX_EcrireJeuDeDonneesLineaireAuFormatMPS : le sens de la contrainte %c ne fait pas partie des sens reconnus\n",
            Sens[Cnt]);
    exit(0);
  } 
}

/* COLUMNS */
fprintf(Flot,"COLUMNS\n");
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( CoutLineaire[Var] != 0.0 ) {
    sprintf(Nombre,"%-.10lf",CoutLineaire[Var]);
    /*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
    fprintf(Flot,"    C%07d  OBJECTIF  %s\n",Var,Nombre);
  }
  il = Cdeb[Var];
  while ( il >= 0 ) {
    sprintf(Nombre,"%-.10lf",CoefficientsDeLaMatriceDesContraintes[il]);
    /*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
    fprintf(Flot,"    C%07d  R%07d  %s\n",Var,NumeroDeContrainte[il],Nombre);
    il = Csui[il];
  }
}

/* RHS */
fprintf(Flot,"RHS\n");
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SecondMembre[Cnt] != 0.0 ) {
    sprintf(Nombre,"%-.9lf",SecondMembre[Cnt]);
    /*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
    fprintf(Flot,"    RHSVAL    R%07d  %s\n",Cnt,Nombre);
  }
}

/* BOUNDS */
fprintf(Flot,"BOUNDS\n");
/*
 Field 1 (columns 2-3) specifies the type of bound:
 LO lower bound
 UP upper bound
 LI lower bound integer variable
 UI upper bound integer variable
 BV binary variable
 FX fixed variable
 FR free
 MI lower bound - infini
 PL upper bound + infini
*/
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  /* Par defaut la variable est PL i.e;. comprise entre 0 et + l'infini */
	if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE ) {
		sprintf(Nombre,"%-.9lf",Xmin[Var]);
		/*Nombre[12] = '\0';*/ /* <- On prefere ne pas ajouter de troncature */
		fprintf(Flot," FX BNDVALUE  C%07d  %s\n",Var,Nombre);
	}
	else if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
		if ( Xmin[Var] != 0.0 ) {
			sprintf(Nombre,"%-.9lf",Xmin[Var]);
			/*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
			fprintf(Flot," LO BNDVALUE  C%07d  %s\n",Var,Nombre);
		}
		sprintf(Nombre,"%-.9lf",Xmax[Var]);
		/*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
		fprintf(Flot," UP BNDVALUE  C%07d  %s\n",Var,Nombre);
	}
	else if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
		if ( Xmin[Var] != 0.0 ) {
			sprintf(Nombre,"%-.9lf",Xmin[Var]);
			/*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
			fprintf(Flot," LO BNDVALUE  C%07d  %s\n",Var,Nombre);
		}
	}
	else if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
		fprintf(Flot," MI BNDVALUE  C%07d\n",Var);
		if ( Xmax[Var] != 0.0 ) {
			sprintf(Nombre,"%-.9lf",Xmax[Var]);
			/*Nombre[12] = '\0';*/ /* <- On prefere ne pas faire de troncature car il y a un risque de changer un tout petit peu le pb */		
			fprintf(Flot," UP BNDVALUE  C%07d  %s\n",Var,Nombre);
		}
	}
	else if ( (int)TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE ) {
		fprintf(Flot," FR BNDVALUE  C%07d\n",Var);
	}
}

/* ENDDATA */
fprintf(Flot,"ENDATA\n");

free ( Cdeb );
free ( NumeroDeContrainte );
free ( Csui );
free ( Nombre );

fclose( Flot );

return;
}
