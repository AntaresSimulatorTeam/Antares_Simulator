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

   FONCTION: Fonction de tirage d'un nombre pseudo aleatoire entre 
             0 et 1
                  
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
   
# define PI            3.141592653
# define PNE_RAND_MAX  1.

/********************************************************************/

double PNE_SRand( double Graine )
{
double A1;

A1 = Graine;
return( PNE_Rand( A1 ) );

}

/********************************************************************/

double PNE_Rand( double A1 )
{
                                                                   
A1 = A1 + PI;                                                      
A1 = pow( A1, 5. );                                                          
A1 = A1 - floor( A1 );                                                    
A1 = A1 * PNE_RAND_MAX;    

if      ( A1 > PNE_RAND_MAX ) A1 = PNE_RAND_MAX;
else if ( A1 < 0.           ) A1 = 0.;

return( A1 );

}


