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

   FONCTION: On insere une contrainte dans l'ensemble des coupes d'un 
             probleme relaxe. Les contraintes sont toujours de type <=.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void PNE_InsererUneContrainte( PROBLEME_PNE * Pne,
                               int      NombreDeTermes, 
                               double * Coefficient, 
                               int   *  IndiceDeLaVariable, 
                               double   SecondMembre,
                               char     ContrainteSaturee,
															 char     TypeDeCoupe )
{
int i; int il; int DerniereContrainte; int * Mdeb; int * NbTerm;
double * A; int * Nuvar;

if ( Pne->Coupes.NombreDeContraintesAllouees <= 0 ) PNE_AllocCoupes( Pne );

DerniereContrainte = Pne->Coupes.NombreDeContraintes - 1;

Mdeb   = Pne->Coupes.Mdeb;
NbTerm = Pne->Coupes.NbTerm;
Nuvar  = Pne->Coupes.Nuvar;
A      = Pne->Coupes.A;

il = 0;
if ( DerniereContrainte >= 0 ) {
  il = Mdeb[DerniereContrainte] + NbTerm[DerniereContrainte];

  if ( il >= Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes ) {
    PNE_AugmenterLaTailleDeLaMatriceDesCoupes( Pne );
    Nuvar = Pne->Coupes.Nuvar;
    A     = Pne->Coupes.A;
  }

}

Mdeb  [Pne->Coupes.NombreDeContraintes] = il;
NbTerm[Pne->Coupes.NombreDeContraintes] = NombreDeTermes;

/* La Contrainte */
for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  A    [il] = Coefficient[i];
  Nuvar[il] = IndiceDeLaVariable[i];  
  il++;
  if ( il >= Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes ) {
    PNE_AugmenterLaTailleDeLaMatriceDesCoupes( Pne );
    Nuvar = Pne->Coupes.Nuvar;
    A     = Pne->Coupes.A;    
  }
}

/* Second membre */
Pne->Coupes.B[Pne->Coupes.NombreDeContraintes] = SecondMembre;
Pne->Coupes.TypeDeCoupe[Pne->Coupes.NombreDeContraintes] = TypeDeCoupe;

/* Etat de saturation */
if ( ContrainteSaturee == OUI_PNE ) Pne->Coupes.PositionDeLaVariableDEcart[Pne->Coupes.NombreDeContraintes] = HORS_BASE_SUR_BORNE_INF;
else 				    Pne->Coupes.PositionDeLaVariableDEcart[Pne->Coupes.NombreDeContraintes] = EN_BASE;

Pne->Coupes.NombreDeContraintes++;
if ( Pne->Coupes.NombreDeContraintes >= Pne->Coupes.NombreDeContraintesAllouees ) PNE_AugmenterLeNombreDeCoupes( Pne );

return;
}
		         


