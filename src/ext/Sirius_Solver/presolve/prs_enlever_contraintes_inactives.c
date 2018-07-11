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

   FONCTION: On enleve les contraintes inactives 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif
								     
/*----------------------------------------------------------------------------*/

void PRS_EnleverLesContraintesInactives( PRESOLVE * Presolve )
{
int Cnt0; int Cnt0Max; int il; 
int * Mdeb0; int * NbTerm0; double * B0; char * SensContrainte0; int * Nuvar0;
double * A0; int il0; int il0Max; int NombreDeTermesNonNuls; 
PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
  
if ( Presolve == NULL ) return;

Cnt0Max = Pne->NombreDeContraintesTrav ;

Mdeb0           = (int *)   malloc( Cnt0Max * sizeof( int ) );
NbTerm0         = (int *)   malloc( Cnt0Max * sizeof( int ) );
B0              = (double *) malloc( Cnt0Max * sizeof( double ) );
SensContrainte0             = (char *) malloc( Cnt0Max * sizeof( char ) );

Nuvar0 = (int *)   malloc( Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( int   ) );
A0     = (double *) malloc( Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( double ) );

if ( Mdeb0  == NULL || NbTerm0 == NULL || B0 == NULL || SensContrainte0 == NULL ||
     Nuvar0 == NULL || A0      == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PRS_EnleverLesContraintesInactives \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

/* Recopie des contraintes dans une zone de sauvegarde */
memcpy( (char *) Nuvar0 , (char *) Pne->NuvarTrav , Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( int   ) );
memcpy( (char *) A0     , (char *) Pne->ATrav     , Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( double ) );

for ( Cnt0 = 0 ; Cnt0 < Cnt0Max ; Cnt0++ ) {
  Mdeb0          [Cnt0] = Pne->MdebTrav          [Cnt0];
  NbTerm0        [Cnt0] = Pne->NbTermTrav        [Cnt0]; 
  B0             [Cnt0] = Pne->BTrav             [Cnt0]; 
  SensContrainte0[Cnt0] = Pne->SensContrainteTrav[Cnt0];  	
}

/* Recopie des contraintes actives */

Pne->NombreDeContraintesTrav = 0;
NombreDeTermesNonNuls       = 0;
for ( il = 0 , Cnt0 = 0 ; Cnt0 < Cnt0Max ; Cnt0++ ) {
  if ( Presolve->ContrainteInactive[Cnt0] == OUI_PNE ) {		
    continue;
  }

  Pne->MdebTrav          [Pne->NombreDeContraintesTrav] = il;
  Pne->NbTermTrav        [Pne->NombreDeContraintesTrav] = 0 /*NbTerm0[Cnt0]*/;
  Pne->BTrav             [Pne->NombreDeContraintesTrav] = B0[Cnt0];
  Pne->SensContrainteTrav[Pne->NombreDeContraintesTrav] = SensContrainte0[Cnt0];

  /* On peut se permettre de faire cela car Cnt0 est toujours superieur ou egal a Pne->NombreDeContraintesTrav */
  Pne->CorrespondanceCntPneCntEntree[Pne->NombreDeContraintesTrav] = Pne->CorrespondanceCntPneCntEntree[Cnt0];

  il0    = Mdeb0[Cnt0];
  il0Max = il0 + NbTerm0[Cnt0];
  while ( il0 < il0Max) {
    if ( A0[il0] != 0.0 ) {     
      Pne->ATrav    [il] = A0[il0];			
      Pne->NuvarTrav[il] = Nuvar0[il0];	
      Pne->NbTermTrav[Pne->NombreDeContraintesTrav]++;
      il++;  
      NombreDeTermesNonNuls++;
    }
    il0++;
  }
	
  for ( il0 = 0 ; il0 < MARGE_EN_FIN_DE_CONTRAINTE ; il0++ ) {
    Pne->ATrav[il] = 0.0;
    il++; 
  } 
		
  Pne->NombreDeContraintesTrav++;
 
}

Pne->ChainageTransposeeExploitable = NON_PNE;

free( Mdeb0 );
free( NbTerm0 );
free( B0 );
free( SensContrainte0 );
free( Nuvar0 );
free( A0 );

return;
}
