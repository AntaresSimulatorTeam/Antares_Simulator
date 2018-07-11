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

   FONCTION: On etudie les domaines de variation des variables entieres
	           dans le but d'en fixer.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_ProbingNodePresolveAlloc( PROBLEME_PNE * Pne, char * CodeRet )   
{
int il; int NombreDeVariables; int NombreDeContraintes; char * Buffer;

PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve;

*CodeRet = OUI_PNE;

ProbingOuNodePresolve = (PROBING_OU_NODE_PRESOLVE *) malloc( sizeof( PROBING_OU_NODE_PRESOLVE ) );
if ( ProbingOuNodePresolve == NULL ) {
  *CodeRet = NON_PNE;
  return; 
}
Pne->ProbingOuNodePresolve = ProbingOuNodePresolve; 
NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
il = 0;

/* Zone de travail */
il += NombreDeVariables * sizeof( char );   /* BorneInfConnue */
il += NombreDeVariables * sizeof( char );   /* BorneSupConnue */
il += NombreDeVariables * sizeof( double ); /* ValeurDeBorneInf */
il += NombreDeVariables * sizeof( double ); /* ValeurDeBorneSup */
il += NombreDeContraintes * sizeof( double ); /* Bmin */
il += NombreDeContraintes * sizeof( double ); /* Bmax */
/* Zone de sauvegarde */
il += NombreDeVariables * sizeof( char ); /* BorneInfConnueSv */
il += NombreDeVariables * sizeof( char ); /* BorneSupConnueSv */
il += NombreDeVariables * sizeof( double ); /* ValeurDeBorneInfSv */
il += NombreDeVariables * sizeof( double ); /* ValeurDeBorneSupSv */
il += NombreDeContraintes * sizeof( double ); /* BminSv */
il += NombreDeContraintes * sizeof( double ); /* BmaxSv */

il += NombreDeVariables * sizeof( int );/* NumeroDeVariableModifiee */
il += NombreDeVariables * sizeof( char );/* VariableModifiee */

il += NombreDeContraintes * sizeof( int );/* NumeroDeContrainteModifiee */
il += NombreDeContraintes * sizeof( int );/* NbFoisContrainteModifiee */

il += NombreDeContraintes * sizeof( int ); /* NumeroDeContrainteAAnalyser */
il += NombreDeContraintes * sizeof( char ); /* ContrainteAAnalyser */

/*il += NombreDeContraintes * sizeof( int );*/ /* IndexContrainteAAnalyser */ 
/*il += NombreDeContraintes * sizeof( int );*/ /* Next_NumeroDeContrainteAAnalyser */ 
/*il += NombreDeContraintes * sizeof( int );*/ /* Next_IndexContrainteAAnalyser */ 

il += NombreDeContraintes * sizeof( char );   /* BminValide */
il += NombreDeContraintes * sizeof( char );   /* BmaxValide */

il += NombreDeVariables * sizeof( int ); /* NumeroDesVariablesFixees */

il += NombreDeContraintes * sizeof( int ); /* NumeroDeCoupeDeProbing */

il += NombreDeContraintes * sizeof( int ); /* NumCntCoupesDeProbing */
il += NombreDeContraintes * sizeof( char ); /* FlagCntCoupesDeProbing */

il += NombreDeVariables * sizeof( int ); /* NumVarAInstancier */
il += NombreDeVariables * sizeof( char ); /* FlagVarAInstancier */

Buffer = (char *) malloc( il );
if ( Buffer == NULL ) {
  free( Pne->ProbingOuNodePresolve );
	Pne->ProbingOuNodePresolve = NULL;
  *CodeRet = NON_PNE;
  return;
}  
ProbingOuNodePresolve->Buffer = Buffer;

il = 0;
/* Zone de travail */
ProbingOuNodePresolve->BorneInfConnue = (char *) (&Buffer[il]);
il += NombreDeVariables * sizeof( char );
ProbingOuNodePresolve->BorneSupConnue = (char *) (&Buffer[il]);  
il += NombreDeVariables * sizeof( char );
ProbingOuNodePresolve->ValeurDeBorneInf = (double *) (&Buffer[il]);
il += NombreDeVariables * sizeof( double );
ProbingOuNodePresolve->ValeurDeBorneSup = (double *) (&Buffer[il]);
il += NombreDeVariables * sizeof( double );   
ProbingOuNodePresolve->Bmin = (double *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( double );   
ProbingOuNodePresolve->Bmax = (double *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( double );

/* Zone de sauvegarde */
ProbingOuNodePresolve->BorneInfConnueSv = (char *) (&Buffer[il]);
il += NombreDeVariables * sizeof( char );
ProbingOuNodePresolve->BorneSupConnueSv = (char *) (&Buffer[il]);
il += NombreDeVariables * sizeof( char );
ProbingOuNodePresolve->ValeurDeBorneInfSv = (double *) (&Buffer[il]);
il += NombreDeVariables * sizeof( double );
ProbingOuNodePresolve->ValeurDeBorneSupSv = (double *) (&Buffer[il]);
il += NombreDeVariables * sizeof( double );
ProbingOuNodePresolve->BminSv = (double *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( double );
ProbingOuNodePresolve->BmaxSv = (double *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( double );

ProbingOuNodePresolve->NumeroDeVariableModifiee = (int *) (&Buffer[il]);
il += NombreDeVariables * sizeof( int );
ProbingOuNodePresolve->VariableModifiee = (char *) (&Buffer[il]);
il += NombreDeVariables * sizeof( char );
ProbingOuNodePresolve->NumeroDeContrainteModifiee = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );
ProbingOuNodePresolve->NbFoisContrainteModifiee = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );

ProbingOuNodePresolve->NumeroDeContrainteAAnalyser = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );
ProbingOuNodePresolve->ContrainteAAnalyser = (char *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( char );

/*
ProbingOuNodePresolve->IndexContrainteAAnalyser = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );
ProbingOuNodePresolve->Next_NumeroDeContrainteAAnalyser = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );
ProbingOuNodePresolve->Next_IndexContrainteAAnalyser = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );
*/

ProbingOuNodePresolve->BminValide = (char *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( char );   
ProbingOuNodePresolve->BmaxValide = (char *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( char );

ProbingOuNodePresolve->NumeroDesVariablesFixees = (int *) (&Buffer[il]);
il += NombreDeVariables * sizeof( int );

ProbingOuNodePresolve->NumeroDeCoupeDeProbing = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int );

ProbingOuNodePresolve->NumCntCoupesDeProbing = (int *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( int ); 
ProbingOuNodePresolve->FlagCntCoupesDeProbing = (char *) (&Buffer[il]);
il += NombreDeContraintes * sizeof( char ); 

ProbingOuNodePresolve->NumVarAInstancier = (int *) (&Buffer[il]);
il += NombreDeVariables * sizeof( int ); 
ProbingOuNodePresolve->FlagVarAInstancier = (char *) (&Buffer[il]);
il += NombreDeVariables * sizeof( char );

memset( (char *) ProbingOuNodePresolve->BminValide, VALIDITE_A_DETERMINER, NombreDeContraintes * sizeof( char ) );
memset( (char *) ProbingOuNodePresolve->BmaxValide, VALIDITE_A_DETERMINER, NombreDeContraintes * sizeof( char ) );

Pne->ProbingOuNodePresolve->Faisabilite = OUI_PNE;

ProbingOuNodePresolve->NbCntCoupesDeProbing = 0;
memset( (char *) ProbingOuNodePresolve->FlagCntCoupesDeProbing, 0, NombreDeContraintes * sizeof( char ) );

return;
}

/*----------------------------------------------------------------------------*/

