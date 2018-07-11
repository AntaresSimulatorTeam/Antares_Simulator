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

   FONCTION: On ordonne les stockage des matrices dans l'ordre croissant
	           des indices.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"
 
# include "spx_fonctions.h"

int SPX_PartitionOrdreCroissant( double * , int * , int , int );
void SPX_ClasserVecteurDansOrdreCroissant( double * , int * , int , int );

/*----------------------------------------------------------------------------*/

int SPX_PartitionOrdreCroissant( double * A, int * NumColonneOuNumLigne, int Debut, int Fin )
{
int Compt; int Pivot; int i; double a; int v;
Compt = Debut;
Pivot = NumColonneOuNumLigne[Debut];
/* Ordre croissant */
for ( i = Debut + 1 ; i <= Fin ; i++) {		
  if ( NumColonneOuNumLigne[i] < Pivot) {
    Compt++;		
    a = A[Compt];
    A[Compt] = A[i];
		A[i] = a;
		v = NumColonneOuNumLigne[Compt];
		NumColonneOuNumLigne[Compt] = NumColonneOuNumLigne[i];
		NumColonneOuNumLigne[i] = v;
  }
}	
a = A[Compt];
A[Compt] = A[Debut];
A[Debut] = a;
v = NumColonneOuNumLigne[Compt];
NumColonneOuNumLigne[Compt] = NumColonneOuNumLigne[Debut];
NumColonneOuNumLigne[Debut] = v;
return(Compt);
}

/*----------------------------------------------------------------------------*/

void SPX_ClasserVecteurDansOrdreCroissant( double * A, int * NumColonneOuNumLigne, int Debut, int Fin )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = SPX_PartitionOrdreCroissant( A, NumColonneOuNumLigne, Debut, Fin );
  SPX_ClasserVecteurDansOrdreCroissant( A, NumColonneOuNumLigne, Debut, Pivot-1 );
  SPX_ClasserVecteurDansOrdreCroissant( A, NumColonneOuNumLigne, Pivot+1, Fin );
}
return;
}

/*----------------------------------------------------------------------------*/

void SPX_OrdonnerMatriceDesContraintes( int NombreDeVecteurs, int * IndexDebut, int * NbTermes,
                                        int * NumColonneOuNumLigne, double * A )
{
int Vecteur; int Debut; int Fin; 
for ( Vecteur = 0 ; Vecteur < NombreDeVecteurs ; Vecteur++ ) { 
	if ( NbTermes[Vecteur] <= 0 ) continue;
	Debut = IndexDebut[Vecteur];
	if ( Debut < 0 ) continue;
	Fin = Debut + NbTermes[Vecteur] - 1;
  SPX_ClasserVecteurDansOrdreCroissant( A, NumColonneOuNumLigne, Debut, Fin );	
}

return;
}

