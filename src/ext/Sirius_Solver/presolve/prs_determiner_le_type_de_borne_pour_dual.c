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

   FONCTION: On determine le type de borne le plus contraignant pour
	           le cout reduit de la variable.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define MARGE 1.e-5

/*----------------------------------------------------------------------------*/

char PRS_AjusterLeTypeDeBorne( char TypeBrnNative, char TypeBrnPresolve,
                               double BorneInfPresolve, double BorneInfNative,
                               double BorneSupPresolve, double BorneSupNative,
															 char ConserverLaBorneInfDuPresolve, char ConserverLaBorneSupDuPresolve,
															 char BorneInfPresolveDisponible, char BorneSupPresolveDisponible,
															 char TypeDeValeurDeBorneInf, char TypeDeValeurDeBorneSup )   
{
char TypeBrn;

TypeBrn = TypeBrnPresolve;
if ( TypeBrn == VARIABLE_FIXE || TypeBrn == VARIABLE_NON_BORNEE ) return( TypeBrn );

if ( BorneInfPresolveDisponible == OUI_PNE && BorneSupPresolveDisponible == NON_PNE ) {
  TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;
	if ( ConserverLaBorneInfDuPresolve == NON_PNE ) {
    if ( TypeBrnNative == VARIABLE_NON_BORNEE || TypeBrnNative == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeBrn = VARIABLE_NON_BORNEE;
	  else if ( TypeBrnNative == VARIABLE_BORNEE_INFERIEUREMENT ) {
			/* Des que la valeur devient implicite c'est qu'il existe une contrainte qui borne la variable */
			if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_NON_BORNEE;
		}
	}
  return( TypeBrn );
}

if ( BorneInfPresolveDisponible == NON_PNE && BorneSupPresolveDisponible == OUI_PNE ) {
 TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;
  if ( ConserverLaBorneSupDuPresolve == NON_PNE ) {
    if ( TypeBrnNative == VARIABLE_NON_BORNEE || TypeBrnNative == VARIABLE_BORNEE_INFERIEUREMENT ) TypeBrn = VARIABLE_NON_BORNEE;	
    else if ( TypeBrnNative == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			/* Des que la valeur devient implicite c'est qu'il existe une contrainte qui borne la variable */
			if ( TypeDeValeurDeBorneSup == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_NON_BORNEE;			
    }
  }
  return( TypeBrn );	
}

if ( BorneInfPresolveDisponible == NON_PNE && BorneSupPresolveDisponible == NON_PNE ) {
  TypeBrn = VARIABLE_NON_BORNEE;
  return( TypeBrn );	
}

if ( BorneInfPresolveDisponible == OUI_PNE && BorneSupPresolveDisponible == OUI_PNE ) {
  TypeBrn = VARIABLE_BORNEE_DES_DEUX_COTES;
	
	if ( ConserverLaBorneInfDuPresolve == NON_PNE ) {
	  if ( ConserverLaBorneSupDuPresolve == NON_PNE ) {
      if ( TypeBrnNative == VARIABLE_NON_BORNEE ) TypeBrn = VARIABLE_NON_BORNEE;
      else if (	TypeBrnNative == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;
				if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_NON_BORNEE;				
			}
      else if (	TypeBrnNative == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;
			  if ( TypeDeValeurDeBorneSup == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_NON_BORNEE;	  			
			}
			else {
        /* TypeBrnNative est VARIABLE_BORNEE_DES_DEUX_COTES */
				/* Des que la valeur devient implicite c'est qu'il existe une contrainte qui borne la variable */
			  if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE && TypeDeValeurDeBorneSup == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_NON_BORNEE;
				else if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;
	      else if ( TypeDeValeurDeBorneSup == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;       				
			}
    }
		else {
      /* On doit conserver la borne sup du presolve */
      if ( TypeBrnNative == VARIABLE_NON_BORNEE ) TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;
      else if (	TypeBrnNative == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;			
      else if (	TypeBrnNative == VARIABLE_BORNEE_INFERIEUREMENT ) {			
			  if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;								
			}
			else {
        /* TypeBrnNative est VARIABLE_BORNEE_DES_DEUX_COTES */
			  if ( TypeDeValeurDeBorneInf == VALEUR_IMPLICITE )	TypeBrn = VARIABLE_BORNEE_SUPERIEUREMENT;										
			}	
		}
	}
	else {
    /* On doit conserver la borne inf du presolve */
	  if ( ConserverLaBorneSupDuPresolve == NON_PNE ) {
      if ( TypeBrnNative == VARIABLE_NON_BORNEE ) TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;
      else if (	TypeBrnNative == VARIABLE_BORNEE_INFERIEUREMENT ) TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;
      else if (	TypeBrnNative == VARIABLE_BORNEE_SUPERIEUREMENT ) {			
			  if ( TypeDeValeurDeBorneSup == VALEUR_IMPLICITE ) TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;	  										
			}
			else {
        /* TypeBrnNative est VARIABLE_BORNEE_DES_DEUX_COTES */
			  if ( TypeDeValeurDeBorneSup == VALEUR_IMPLICITE )	TypeBrn = VARIABLE_BORNEE_INFERIEUREMENT;	  											
			}			  
    }
	}	
  return( TypeBrn );	
}

return( TypeBrn );
}

/*----------------------------------------------------------------------------*/

char PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( 
                     int TypeDeBorneNative, int TypeDeBornePourPresolve,
										 double BorneInfPourPresolve, double BorneInfNative,
										 double BorneSupPourPresolve, double BorneSupNative,
										 char ConserverLaBorneInfDuPresolve, char ConserverLaBorneSupDuPresolve,
										 char TypeDeValeurDeBorneInf, char TypeDeValeurDeBorneSup )   										 
{
char BorneInfPresolveDisponible; double BorneInfPresolve; char BorneSupPresolveDisponible;
double BorneSupPresolve; char TypeBrn;

if ( TypeDeBornePourPresolve == VARIABLE_FIXE || TypeDeBornePourPresolve == VARIABLE_NON_BORNEE ) {
  return( TypeDeBornePourPresolve );
}

BorneInfPresolveDisponible = NON_PNE;
BorneInfPresolve = -LINFINI_PNE;
BorneSupPresolveDisponible = NON_PNE;
BorneSupPresolve = LINFINI_PNE;		
if ( TypeDeBornePourPresolve == VARIABLE_BORNEE_INFERIEUREMENT ) {
  BorneInfPresolveDisponible = OUI_PNE;
	BorneInfPresolve = BorneInfPourPresolve;
}
else if ( TypeDeBornePourPresolve == VARIABLE_BORNEE_SUPERIEUREMENT ) {
  BorneSupPresolveDisponible = OUI_PNE;
	BorneSupPresolve = BorneSupPourPresolve;
}
else if ( TypeDeBornePourPresolve == VARIABLE_BORNEE_DES_DEUX_COTES ) {
  BorneInfPresolveDisponible = OUI_PNE;
	BorneInfPresolve = BorneInfPourPresolve;
  BorneSupPresolveDisponible = OUI_PNE;
	BorneSupPresolve = BorneSupPourPresolve;		
}
	
TypeBrn = PRS_AjusterLeTypeDeBorne( TypeDeBorneNative, TypeDeBornePourPresolve, BorneInfPresolve, BorneInfNative,
	                                  BorneSupPresolve, BorneSupNative,
															      ConserverLaBorneInfDuPresolve, ConserverLaBorneSupDuPresolve,
															      BorneInfPresolveDisponible, BorneSupPresolveDisponible,
										                TypeDeValeurDeBorneInf, TypeDeValeurDeBorneSup );

return( TypeBrn );
}

/*----------------------------------------------------------------------------*/
