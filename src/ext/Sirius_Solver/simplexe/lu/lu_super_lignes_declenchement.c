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

   FONCTION: Declenchement eventuel de la detection des super lignes 
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/

void LU_InitPourSuperLignes( MATRICE * Matrice )	  
{

Matrice->MatricePleineDansUneSeuleSuperLigne = NON_LU;

/* On fait une RAZ des tables de hash code */
memset( (char *) Matrice->HashCodeLigne                 , 0                         ,     Matrice->Rang * sizeof( unsigned int ) );
memset( (char *) Matrice->TypeDeClassementHashCodeAFaire, ELEMENT_HASCODE_A_CLASSER , 2 * Matrice->Rang * sizeof( char          ) );

Matrice->NbScanMin                       = 0;
Matrice->NombreDePassagesDansSuperLignes = 0;
Matrice->OnDeclenche                     = NON_LU; 
Matrice->NbLignesSuperLignes             = 0;  
Matrice->NbFois                          = -1;
Matrice->Cycle                           = Matrice->Rang / 100;
Matrice->TauxPrec                        = 100.;
Matrice->NbScanSuperLignes               = 0;
Matrice->LaMatriceEstPleine              = NON_LU;
  
Matrice->SeuilNombreDeSuperLigneAGrouper = SEUIL_NB_SUPER_LIGNES_A_GROUPER;
Matrice->SeuilNombreDeLignesAGrouper     = SEUIL_1_NB_LIGNES_A_GROUPER;

return;
}

/*--------------------------------------------------------------------------------------------------*/
void LU_DeclenchementEventuelRechercheSuperLignes( MATRICE * Matrice )	  
{
int Kp; double Taux; char SuperLignesReamenagees; char ParVariationDeTaux; 
   
Kp = Matrice->Kp;

Taux = (float)( Matrice->NombreDeTermes )/(float)( (float)(Matrice->Rang - Kp) * (float)(Matrice->Rang - Kp) );
/*
printf("Kp %d NbFillIn %d Rang %d  NombreDeTermes %d taux remplissage %f \n",
Kp,Matrice->NbFillIn,Matrice->Rang,Matrice->NombreDeTermes,
Taux );
fflush(stdout);
*/

if ( ( Taux > 0.1 || Kp > 0.8 * Matrice->Rang ) && Matrice->NbFois == -1 ) {
  Matrice->OnDeclenche = OUI_LU;
      
  Matrice->CycleDeBase = ( Matrice->Rang - Kp ) / 100;
  Matrice->SeuilDeVariationDeBase = 0.1/*0.05*/;

  Matrice->Cycle = Matrice->CycleDeBase;
  Matrice->SeuilDeVariation = Matrice->SeuilDeVariationDeBase;
      
  Matrice->NbFois = Matrice->Cycle;
      
  /*printf("Declenchement superlignes Kp = %d  Rang = %d\n",Kp,Matrice->Rang);*/
           
}

if ( Matrice->LaMatriceEstPleine == OUI_LU && Matrice->OnDeclenche == OUI_LU ) {

  /* printf("Kp = %d on met tout dans une seule super ligne\n",Kp); */
	
  LU_MettreTouteLaMatriceDansUneSuperLigne( Matrice ); 
  Matrice->MatricePleineDansUneSeuleSuperLigne = OUI_LU; 
  Matrice->OnDeclenche = NON_LU;
  Matrice->NbFois = Matrice->Rang;
}
   
if ( Matrice->LaMatriceEstPleine == OUI_LU && Matrice->OnDeclenche == OUI_LU ) {
      
  printf("Kp = %d la matrice est pleine, on relance une recherche de super lignes car on devrait alors tout fusionner\n",Kp);
      
  LU_DetectionSuperLignes( Matrice , &SuperLignesReamenagees );
  Matrice->OnDeclenche = NON_LU;
      
  /* Verification. Il ne doit avoir qu'une seule super ligne rempli grace aux fusion */     
  {
    int j; int nbsl; 
    nbsl = 0;
    for ( j = 0 ; j < Matrice->NombreDeSuperLignes ; j++ ) {
      if ( Matrice->SuperLigne[j] == NULL ) continue;
      nbsl++;
    }
    /*	printf("############### NombreDeSuperLignes non vides %d  Kp %d\n", nbsl, Kp); */
    if ( nbsl != 1 ) {
      printf(" BUG, il doit y avoir 1 et 1 seul super ligne car la matrice est pleine\n");
      exit(0);
    }
  }
      
}
   
if ( Matrice->OnDeclenche == OUI_LU ) {
   
  /* printf(" Kp = %d  Taux = %f\n",Kp,Taux); */
  
  if ( Matrice->NbFois >= Matrice->Cycle || Taux > Matrice->TauxPrec + Matrice->SeuilDeVariation ) {
    if (  Taux > Matrice->TauxPrec + Matrice->SeuilDeVariation ) ParVariationDeTaux = OUI_LU;
    else ParVariationDeTaux = NON_LU;
    Matrice->TauxPrec = Taux;		
    LU_DetectionSuperLignes( Matrice , &SuperLignesReamenagees );
    /* S'il n'y a pas eu de reamenagement de lignes, on diminue la frequence de recherche */
    if ( SuperLignesReamenagees == NON_LU && 0 ) {
      if ( ParVariationDeTaux == OUI_LU ) {
        Matrice->SeuilDeVariation+= Matrice->SeuilDeVariationDeBase;
        if ( Matrice->TauxPrec + Matrice->SeuilDeVariation > 1.0 ) {
					/* Pour ne pas bloquer on baisse un peu SeuilDeVariation */
					Matrice->SeuilDeVariation = 0.5 * ( 1 - Taux ); 
        }
      }
      else {
        Matrice->Cycle+= Matrice->CycleDeBase;
        /* Pour eviter de bloquer */
        if ( Matrice->Cycle > Matrice->Rang - Kp )  Matrice->Cycle = ceil ( 0.5 * ( Matrice->Rang - Kp ) );
      }
    }
    else {
      /*
      if ( ParVariationDeTaux == OUI_LU ) {
        Matrice->SeuilDeVariation-= Matrice->SeuilDeVariationDeBase;
        if ( Matrice->SeuilDeVariation < Matrice->SeuilDeVariationDeBase ) Matrice->SeuilDeVariation = Matrice->SeuilDeVariationDeBase;
      }
      else {
        Matrice->Cycle-= Matrice->CycleDeBase;
        if ( Matrice->Cycle < Matrice->CycleDeBase ) Matrice->Cycle = Matrice->CycleDeBase;
      }
      */
    }
    /*
    printf("  SuperLignes Kp = %d  NombreDeSuperLignes = %d Taux = %f Cycle %d\n",Kp,Matrice->NombreDeSuperLignes,Taux,Matrice->Cycle);
    */
    Matrice->NbFois = 0;
  }
  else {
    Matrice->NbFois++;
  } 
}
  
return;
}

































































