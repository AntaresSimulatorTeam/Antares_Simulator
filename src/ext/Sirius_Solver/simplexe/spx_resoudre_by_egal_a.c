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

   FONCTION: Resoudre B y = a en utilisant la forme produit de 
             l'inverse si necessaire.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_ResoudreBYegalA( PROBLEME_SPX * Spx,
                          char TypeDEntree, /* Mode de stockage du vecteur second membre */
                          double * A,       /* Second membre et solution */
                          int * IndexDesTermesNonNuls,
													int * NombreDeTermesNonNuls,
													char * TypeDeSortie, /* Mode de stockage demande pour la solution */
													
                          char CalculEnHyperCreux, /* Vaut OUI_SPX ou NON_SPX */
                          char Save,  /* Sauvegarde ou non du resultat de la triangulaire inferieure */
			                    char SecondMembreCreux /* Vaut OUI_LU ou NON_LU */
                        )
{
/* Il faut d'abord resoudre B y = a en prenant pour B la derniere base factorisee
   et eventuellement appliquer les eta vecteurs dans le cas de la forme produit de
	 l'inverse */

SPX_ResolutionDeSysteme( Spx, TypeDEntree, A, IndexDesTermesNonNuls, NombreDeTermesNonNuls,
												 TypeDeSortie, CalculEnHyperCreux, Save, SecondMembreCreux );
   
/* Si necessaire, on applique les eta vecteurs (cas: forme produit de l'inverse) */
if ( Spx->UtiliserLaLuUpdate == OUI_SPX ) return;

SPX_AppliquerLesEtaVecteurs( Spx, A, IndexDesTermesNonNuls, NombreDeTermesNonNuls, CalculEnHyperCreux, *TypeDeSortie );

/* On ne cherche pas a passer en mode plein apres application des eta vecteurs */

return;
}
  
  
