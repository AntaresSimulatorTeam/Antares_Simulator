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

   FONCTION:   Calcul de pi = Cb * B_MOINS_1 c'est a dire              
               resolution de u B = c

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/
												
void SPX_ResoudreUBEgalC( PROBLEME_SPX * Spx,
                          char TypeDEntree, /* Mode de stockage du vecteur second membre */
                          double * U, /* Second membre et resultat */
                          int * IndexDesTermesNonNuls,
													int * NombreDeTermesNonNuls,
													char * TypeDeSortie, 
                          char CalculEnHyperCreux /* Vaut OUI_SPX ou NON_SPX */
												)
{ 
char SecondMembreCreux;     

if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
  SPX_AppliquerLesEtaVecteursTransposee( Spx, U, IndexDesTermesNonNuls, NombreDeTermesNonNuls,
	                                       CalculEnHyperCreux, TypeDEntree );	
}

/* Terminer par la resolution avec la derniere base factorisee */

SecondMembreCreux = NON_LU;
SPX_ResolutionDeSystemeTransposee( Spx, TypeDEntree, U, IndexDesTermesNonNuls, NombreDeTermesNonNuls,																		
                                   TypeDeSortie, CalculEnHyperCreux, SecondMembreCreux );  
																	 
return;
}
