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

   FONCTION: Creation de la matrice du point interieur.
        
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# include "lu_constantes_externes.h"
# include "lu_definition_arguments.h"

# include "lu_define.h"
# include "lu_fonctions.h"

/*-------------------------------------------------------------------------*/
/*     Allocation de la matrice, calcul des termes, stockage des termes    */
 
void PI_Cremat( PROBLEME_PI * Pi )
{

PI_CrematSystemeAugmente( Pi );
		  
return;
}

/*-------------------------------------------------------------------------*/
/*  Calcul de termes de la matrice a chaque iteration puis factorisation   */

void PI_Crebis( PROBLEME_PI * Pi )
{

PI_CrebisSystemeAugmente( Pi );

return;
}




