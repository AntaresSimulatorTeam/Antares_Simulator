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

   FONCTION: Resolution d'un sous systeme issu de la decomposition
	           Dumalge-Mendelson
                
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
			
# include "lu_define.h"
# include "lu_fonctions.h"

# if DUMALGE_MENDELSON_PNE == OUI_PNE

# include "btf.h"
# include "btf_internal.h"

# include "cs.h"   

/*----------------------------------------------------------------------------*/

void * PNE_DumalgeFactoriserMatrice( PROBLEME_PNE * Pne,
                                          void ** MatriceAFactoriser,
                                          int NbVarDuProblemeReduit,
																					int NbCntDuProblemeReduit,																					
 	                                        int IndexCCdeb,
																				 	int IndexCCfin,					
																					int IndexRRdeb,
																					int IndexRRfin,																																											
                                          void * Csd_in,
																					void * A_in,
																					int * Ligne,   
																					int * Colonne,
																				  char * CodeRet,
																					char FactorisationOK,
																					char MatriceSinguliere  
		                                 )
{   
int ic; int icMax; int icNew; csi ccDeb; csi ccFin; csi rrDeb; csi rrFin;
int i; int j; int l; csd * Csd; cs * A; int c;
MATRICE_A_FACTORISER * Matrice; MATRICE * MatriceFactorisee; 

*CodeRet = FactorisationOK;

Csd = (csd *) Csd_in;
A = (cs *) A_in;

ccDeb = (csi) IndexCCdeb;
ccFin = (csi) IndexCCfin;
rrDeb = (csi) IndexRRdeb;
rrFin = (csi) IndexRRfin;

for ( j = 0 , i = ccDeb ; i < ccFin ; i++ ) {
	/* Csd->q[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->q[i];
  Colonne[l] = j;
	j++;
}

for ( j = 0 , i = rrDeb ; i < rrFin ; i++ ) {
	/* Csd->p[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->p[i];
  Ligne[l] = j;
	j++;  
}

Matrice = (MATRICE_A_FACTORISER *) malloc( sizeof( MATRICE_A_FACTORISER ) );
Matrice->NombreDeColonnes = NbVarDuProblemeReduit;
Matrice->IndexDebutDesColonnes = (int *) malloc( NbVarDuProblemeReduit * sizeof( int ) );
Matrice->NbTermesDesColonnes   = (int *) malloc( NbVarDuProblemeReduit * sizeof( int ) );
Matrice->ValeurDesTermesDeLaMatrice = (double *) malloc( A->nzmax * sizeof( double ) );
Matrice->IndicesDeLigne             = (int *)    malloc( A->nzmax * sizeof( int    ) );

*MatriceAFactoriser = (void *) Matrice;
   
icNew = 0;
for ( j = 0 , i = ccDeb ; i < ccFin ; i++ ) {
  Matrice->IndexDebutDesColonnes[j] = icNew;
  Matrice->NbTermesDesColonnes  [j] = 0;
	c = Csd->q[i];
	ic = A->p[c];
	icMax = A->p[c+1];
  /*printf("\nMatrice: c %d j %d  i %d\n",c,j,i);*/
	while ( ic < icMax ) {	  
		if ( Ligne[A->i[ic]] >= 0 ) {		  
      Matrice->ValeurDesTermesDeLaMatrice[icNew] = A->x[ic];   
      Matrice->IndicesDeLigne[icNew] = Ligne[A->i[ic]];
      Matrice->NbTermesDesColonnes[j]++;
			/*
	    printf("ValeurDesTermesDeLaMatrice %e IndicesDeLigne %d  A->i %d\n",
			        Matrice->ValeurDesTermesDeLaMatrice[icNew],Matrice->IndicesDeLigne[icNew],A->i[ic]);
			*/				
      icNew++;
		}
		ic++;
	}
	j++;
}
		 
printf("DEBUT DE FACTORISATION\n");
Matrice->ContexteDeLaFactorisation  = LU_GENERAL;
Matrice->UtiliserLesSuperLignes     = NON_LU;
Matrice->FaireScalingDeLaMatrice    = OUI_LU;

Matrice->UtiliserLesValeursDePivotNulParDefaut = NON_LU;
Matrice->ValeurDuPivotMin        = 1.e-10;     
Matrice->ValeurDuPivotMinExtreme = 1.e-10;

Matrice->SeuilPivotMarkowitzParDefaut = OUI_LU;
Matrice->FaireDuPivotageDiagonal      = NON_LU;
Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU;
Matrice->LaMatriceEstSymetrique       = NON_LU;

MatriceFactorisee = LU_Factorisation( Matrice );

printf("FIN DE FACTORISATION \n");

if ( MatriceFactorisee == NULL ) {
  printf("PB factorisation  \n");
	return( NULL );
}

if ( Matrice->ProblemeDeFactorisation == MATRICE_SINGULIERE ) {
   printf("Matrice singuliere \n");	 
	 *CodeRet = MatriceSinguliere;
	return( (void *) MatriceFactorisee );
}
  
return( (void *) MatriceFactorisee );

}

# endif


