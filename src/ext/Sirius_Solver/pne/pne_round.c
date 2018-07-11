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

   FONCTION: Pour réduire la précision des coefficients utilisés dans les calculs 
                
   AUTEUR: N. Lhuiller

************************************************************************/

# include "pne_sys.h"  
# include "pne_fonctions.h"

/* Pour réduire la précision des coefficients utilisés dans les calculs */
double PNE_Round(double x, double prec) {

       if (x > 0)
             x = floor(x * prec + 0.5) / prec;
       else if (x < 0)
             x = ceil(x * prec - 0.5) / prec;
						 
       return x;
}
