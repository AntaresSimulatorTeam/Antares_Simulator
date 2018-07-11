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

   FONCTION: Dans chaque contrainte on classe les coefficients dans
	           l'ordre decroissant des valeurs absolues.
						 Ca sert a calculer les min max contraintes en utilisant
						 les cliques.
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"  

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

int PNE_PartitionCoeffContraintesTriRapide( double * , int * , int , int );
void PNE_ClasserCoeffContraintesTriRapide( double * , int * , int , int );

/*----------------------------------------------------------------------------*/

int PNE_PartitionCoeffContraintesTriRapide( double * A, int * Nuvar, int Debut, int Fin )
{
int Compt; double Pivot; int i; double a; int v;
Compt = Debut;
Pivot = fabs( A[Debut] );
/* Ordre decroissant */
for ( i = Debut + 1 ; i <= Fin ; i++) {		
  if ( fabs( A[i] ) > Pivot) {
    Compt++;		
    a = A[Compt];
    A[Compt] = A[i];
		A[i] = a;
		v = Nuvar[Compt];
		Nuvar[Compt] = Nuvar[i];
		Nuvar[i] = v;
  }
}	
a = A[Compt];
A[Compt] = A[Debut];
A[Debut] = a;
v = Nuvar[Compt];
Nuvar[Compt] = Nuvar[Debut];
Nuvar[Debut] = v;
return(Compt);
}

/*----------------------------------------------------------------------------*/

void PNE_ClasserCoeffContraintesTriRapide( double * A, int * Nuvar, int Debut, int Fin )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = PNE_PartitionCoeffContraintesTriRapide( A, Nuvar, Debut, Fin );
  PNE_ClasserCoeffContraintesTriRapide( A, Nuvar, Debut, Pivot-1 );
  PNE_ClasserCoeffContraintesTriRapide( A, Nuvar, Pivot+1, Fin );
}
return;
}

/*----------------------------------------------------------------------------*/

void PNE_ClasserLesCoefficientsDesContraintesOrdreDecroissant( PROBLEME_PNE * Pne )
{
int * Mdeb; int * NbTerm; int * Nuvar; double * A; int NombreDeContraintes;
int Debut; int Fin; int Cnt;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) { 
	if ( NbTerm[Cnt] <= 0 ) continue;
	Debut = Mdeb[Cnt];
	if ( Debut < 0 ) continue;
	Fin = Debut + NbTerm[Cnt] - 1;
  PNE_ClasserCoeffContraintesTriRapide( A, Nuvar, Debut, Fin );	
}

return;
}
