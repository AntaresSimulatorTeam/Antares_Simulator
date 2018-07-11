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

   FONCTION: Normalisation d'une coupe
                
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

/*----------------------------------------------------------------------------*/

void PNE_NormaliserUnCoupe( double * Coefficient, double * SecondMembre, int NombreDeTermes, double PlusGrandCoeff )
{
int i; double Normalisation; double Sec; 

# if NORMALISER_LES_COUPES_SUR_LES_G_ET_I != OUI_PNE    
  return;
# endif

if ( PlusGrandCoeff < SEUIL_POUR_NORMALISER_LES_COUPES_SUR_LES_G_ET_I ) return;

Normalisation = 1.0 / PlusGrandCoeff;  
SPX_ArrondiEnPuissanceDe2( &Normalisation );  

for ( i = 0 ; i < NombreDeTermes ; i++ ) Coefficient[i] *= Normalisation;          
Sec = *SecondMembre;
Sec*= Normalisation;
*SecondMembre = Sec;
   
return;
}

