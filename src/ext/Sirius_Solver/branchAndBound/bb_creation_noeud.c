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

   FONCTION: Allocation / Desallocation  
  
   Allocation d'un noeud: il renseigne le champ adresse de l'antecedent 
   mais pas les adresses des noeuds suivants.
   Si le noeud antecedent est a rejeter ou terminal, le fils n'est pas cree et on renvoie Noeud = 0 
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------------*/

NOEUD * BB_AllouerUnNoeud(  BB * Bb,
			    NOEUD * NoeudAntecedent, 
          int    ProfondeurDuNoeud,
			    int    FilsACreer, 
			    int    ValeurDInstanciation,
			    int    NombreDeVariablesAInstancier,
			    int *  NumerosDesVariablesAInstancier,			    
          double  MinorantPredit )
{
NOEUD * Noeud; int i; int Marge; int Var;  

Bb->NbNoeudsOuverts++;

if( NoeudAntecedent != 0 ) {
  if ( NoeudAntecedent->StatutDuNoeud == A_REJETER ) {
    BB_DesallocationPartielleDUnNoeud( NoeudAntecedent );      
    return( 0 );
  }
  if ( NoeudAntecedent->NoeudTerminal == OUI ) {
    BB_DesallocationPartielleDUnNoeud( NoeudAntecedent );        
    return( 0 );
  }  
}

Noeud = (NOEUD *) malloc( sizeof(NOEUD) );
if ( Noeud == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n"); fflush(stdout);
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}

Noeud->ProfondeurDuNoeud  = ProfondeurDuNoeud;
Noeud->StatutDuNoeud = A_EVALUER;
Noeud->NoeudTerminal = NON;

Noeud->NombreDeVariablesEntieresInstanciees = 0;

Noeud->NbValeursFractionnairesApresResolution = Bb->NombreDeVariablesEntieresDuProbleme
					                                    - Noeud->NombreDeVariablesEntieresInstanciees;
if( NoeudAntecedent == 0 ) {
  /* C'est le noeud racine */
  Noeud->BaseFournie = NON;
  Noeud->IndiceDeLaNouvelleVariableInstanciee = -1;
  /* Taille des tableaux a allouer */
  i = 1;  
}
else {
  i = NoeudAntecedent->NombreDeVariablesEntieresInstanciees;
  /* Taille des tableaux a allouer */
  i+= NombreDeVariablesAInstancier; 
}

Noeud->ValeursDesVariablesEntieresInstanciees = (char *) malloc( i * sizeof(char) );
Noeud->IndicesDesVariablesEntieresInstanciees = (int *) malloc( i * sizeof(int) );

if ( Noeud->ValeursDesVariablesEntieresInstanciees == NULL || Noeud->IndicesDesVariablesEntieresInstanciees == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}

/* Pour pouvoir travailler avec la forme standard, on surdimensionne le vecteur avec le nombre de contraintes (inutile maintenant) */
Noeud->PositionDeLaVariable = (int *) malloc( ( Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ) * sizeof(int) );
if ( Noeud->PositionDeLaVariable == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}

Noeud->NbVarDeBaseComplementairesSansCoupes = 0;
Noeud->ComplementDeLaBaseSansCoupes   = NULL;
Noeud->PositionDeLaVariableSansCoupes = NULL;
if ( NoeudAntecedent == 0 ) {
  /* Cas du noeud racine, il faut quand-meme allouer la memoire pour ces tables */
  Noeud->TailleComplementDeBase = Bb->NombreDeContraintesDuProbleme;
  Noeud->ComplementDeLaBase             = (int *) malloc( Noeud->TailleComplementDeBase * sizeof(int) );
  Noeud->ComplementDeLaBaseSansCoupes   = (int *) malloc( Noeud->TailleComplementDeBase * sizeof(int) );
  Noeud->PositionDeLaVariableSansCoupes = (int *) malloc( Bb->NombreDeVariablesDuProbleme * sizeof(int) );  
  if ( Noeud->ComplementDeLaBase == NULL || Noeud->ComplementDeLaBaseSansCoupes == NULL ||
       Noeud->PositionDeLaVariableSansCoupes == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
}

Noeud->CoupesGenereesAuNoeud = 0 ; 
Noeud->NombreDeCoupesGenereesAuNoeud = 0;
Noeud->NombreDeG = 0;
Noeud->NombreDeK = 0;
Noeud->NombreDeI = 0;

Noeud->NombreDeCoupesExaminees  = 0;
Noeud->NumeroDesCoupesExaminees = NULL;
Noeud->LaCoupeEstSaturee        = NULL;

Noeud->NombreDeCoupesViolees  = 0;
Noeud->NumeroDesCoupesViolees = NULL; 

Noeud->NumeroDesCoupeAjouteeAuProblemeCourant = NULL;

Noeud->MinorantDuCritereAuNoeud    = PLUS_LINFINI;
Noeud->MinorantPredit              = MinorantPredit;
Noeud->LaSolutionRelaxeeEstEntiere = NON;

Noeud->NoeudAntecedent = NoeudAntecedent;

Noeud->NoeudSuivantGauche = 0;
Noeud->NoeudSuivantDroit  = 0;

Noeud->NbVarDeBaseComplementaires = 0; 

Noeud->BaseSimplexeDuNoeud = NULL;

/* Initialisation des tables des bornes au noeud */

if( NoeudAntecedent == 0 ) {
  Noeud->NombreDeBornesModifiees = 0;
  Noeud->NumeroDeLaVariableModifiee = NULL;
  Noeud->TypeDeBorneModifiee = NULL;
  Noeud->NouvelleValeurDeBorne = NULL;
}
else {
  i = NoeudAntecedent->NombreDeBornesModifiees;	
  Noeud->NombreDeBornesModifiees = i;
  Noeud->NumeroDeLaVariableModifiee = NULL;
  Noeud->TypeDeBorneModifiee = NULL;
  Noeud->NouvelleValeurDeBorne = NULL;
	if ( i > 0 ) {	
    Noeud->NumeroDeLaVariableModifiee = (int *) malloc( i * sizeof( int ) );
    Noeud->TypeDeBorneModifiee = (char *) malloc( i * sizeof( char ) );
    Noeud->NouvelleValeurDeBorne = (double *) malloc( i * sizeof( double ) );
    if ( Noeud->NumeroDeLaVariableModifiee == NULL || Noeud->TypeDeBorneModifiee == NULL || Noeud->NouvelleValeurDeBorne == NULL ) { 
      printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
      Bb->AnomalieDetectee = OUI;
      longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
    }
    memcpy( (char *) Noeud->NumeroDeLaVariableModifiee, (char *) NoeudAntecedent->NumeroDeLaVariableModifiee, i * sizeof( int ) );  
    memcpy( (char *) Noeud->TypeDeBorneModifiee, (char *) NoeudAntecedent->TypeDeBorneModifiee, i * sizeof( char ) );  
    memcpy( (char *) Noeud->NouvelleValeurDeBorne, (char *) NoeudAntecedent->NouvelleValeurDeBorne, i * sizeof( double ) );
	}
}

if( NoeudAntecedent == 0 ) {
  /* Car c'est le noeud racine */
  return( Noeud ); 
}

/* Recopie de la liste des variables fixees du pere et incrementation avec les nouvelles variables */

memcpy( (char *) Noeud->ValeursDesVariablesEntieresInstanciees , (char *) NoeudAntecedent->ValeursDesVariablesEntieresInstanciees , 
         NoeudAntecedent->NombreDeVariablesEntieresInstanciees * sizeof( char ) );  

memcpy( (char *) Noeud->IndicesDesVariablesEntieresInstanciees , (char *) NoeudAntecedent->IndicesDesVariablesEntieresInstanciees , 
         NoeudAntecedent->NombreDeVariablesEntieresInstanciees * sizeof( int ) );  

Noeud->NombreDeVariablesEntieresInstanciees = NoeudAntecedent->NombreDeVariablesEntieresInstanciees;
	 
if( ValeurDInstanciation == 0 ) {
  for ( i = 0 ; i < NombreDeVariablesAInstancier ; i++ ) {
    Var = NumerosDesVariablesAInstancier[i];
    Noeud->ValeursDesVariablesEntieresInstanciees[ Noeud->NombreDeVariablesEntieresInstanciees ] = '0';
    Noeud->IndicesDesVariablesEntieresInstanciees[ Noeud->NombreDeVariablesEntieresInstanciees ] = Var;
    Noeud->NombreDeVariablesEntieresInstanciees++;
  }
}
else {
  for ( i = 0 ; i < NombreDeVariablesAInstancier ; i++ ) {
    Var = NumerosDesVariablesAInstancier[i];
    Noeud->ValeursDesVariablesEntieresInstanciees[ Noeud->NombreDeVariablesEntieresInstanciees ] = '1';
    Noeud->IndicesDesVariablesEntieresInstanciees[ Noeud->NombreDeVariablesEntieresInstanciees ] = Var;
    Noeud->NombreDeVariablesEntieresInstanciees++;
  }
}

if      ( FilsACreer == FILS_GAUCHE ) NoeudAntecedent->NoeudSuivantGauche = Noeud; 
else if ( FilsACreer == FILS_DROIT  ) NoeudAntecedent->NoeudSuivantDroit  = Noeud; 
else {
  printf("BB_AllouerUnNoeud: type de fils a creer inconnu %d\n",FilsACreer);
  exit(0);
}

/* Pour le simplexe */

Noeud->BaseFournie = OUI;
/* Meme s'il y a plusieurs variables instanciees on ne met que la premiere valeur car cela ne sert
   qu'a differencier le noeud racine des autres au niveau de l'appel du simplexe */
Noeud->IndiceDeLaNouvelleVariableInstanciee = NumerosDesVariablesAInstancier[0];

/* Recopie de la base du pere */
Marge = (int)(0.1 * Bb->NombreDeContraintesDuProbleme);
if ( Marge <= 0 ) Marge = 10;

Noeud->NombreDeCoupesExaminees = Bb->NombreDeCoupesAjoutees;
if ( Bb->NombreDeCoupesAjoutees > 0 ) {
  Noeud->NumeroDesCoupesExaminees = (int *) malloc( Noeud->NombreDeCoupesExaminees * sizeof(int) );
  Noeud->LaCoupeEstSaturee        = (char *) malloc( Noeud->NombreDeCoupesExaminees * sizeof(char) );  
  if ( Noeud->NumeroDesCoupesExaminees == NULL || Noeud->LaCoupeEstSaturee == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
}
 
if ( Bb->BasesFilsDisponibles == NON_PNE ) {

  memcpy( (char * ) Noeud->PositionDeLaVariable , (char * ) NoeudAntecedent->PositionDeLaVariable , 
                    ( Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ) * sizeof( int ) );

  Noeud->TailleComplementDeBase = NoeudAntecedent->NbVarDeBaseComplementaires + Marge;
  /* Il faut dimensionner aux nombre de contraintes car le simplexe peut y ajouter des valeurs dans la limite du nombre 
     de contraintes => Noeud->TailleComplementDeBase est inutile */

  Noeud->ComplementDeLaBase = (int *) malloc( Bb->NombreDeContraintesDuProbleme /*Noeud->TailleComplementDeBase*/ * sizeof(int) );  
  if ( Noeud->ComplementDeLaBase == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }

  Noeud->NbVarDeBaseComplementaires = NoeudAntecedent->NbVarDeBaseComplementaires;
  memcpy( (char * ) Noeud->ComplementDeLaBase , (char * ) NoeudAntecedent->ComplementDeLaBase , NoeudAntecedent->NbVarDeBaseComplementaires * sizeof( int ) );

  for ( i = 0 ; i < Bb->NombreDeCoupesAjoutees ; i++ ) {
    Noeud->NumeroDesCoupesExaminees[i] = Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[i];     
    Noeud->LaCoupeEstSaturee       [i] = Bb->CoupeSaturee[i];  
  }

}
/* Preparation de la base de depart */
if ( Bb->BasesFilsDisponibles == OUI_PNE ) {

  if ( ValeurDInstanciation == 0 ) {

    memcpy( (char * ) Noeud->PositionDeLaVariable , (char * ) Bb->PositionDeLaVariableAEntierInf , 
                      ( Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ) * sizeof( int ) );

    Noeud->TailleComplementDeBase = Bb->NbVarDeBaseComplementairesAEntierInf + Marge;												      
    /* Il faut dimensionner aux nombre de contraintes car le simplexe peut y ajouter des valeurs dans la limite du nombre 
       de contraintes => Noeud->TailleComplementDeBase est inutile */
       
    Noeud->ComplementDeLaBase = (int *) malloc( Bb->NombreDeContraintesDuProbleme /*Noeud->TailleComplementDeBase*/ * sizeof(int) );
    if ( Noeud->ComplementDeLaBase == NULL ) { 
      printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
      Bb->AnomalieDetectee = OUI;
      longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
    }

    Noeud->NbVarDeBaseComplementaires = Bb->NbVarDeBaseComplementairesAEntierInf;
    memcpy( (char * ) Noeud->ComplementDeLaBase , (char * ) Bb->ComplementDeLaBaseAEntierInf , 
                      Noeud->NbVarDeBaseComplementaires * sizeof( int ) );
		      
    for ( i = 0 ; i < Bb->NombreDeCoupesAjoutees ; i++ ) {
      Noeud->NumeroDesCoupesExaminees[i] = Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[i];     
      Noeud->LaCoupeEstSaturee       [i] = Bb->CoupeSatureeAEntierInf[i];
    }

  }
  else {

    memcpy( (char * ) Noeud->PositionDeLaVariable , (char * ) Bb->PositionDeLaVariableAEntierSup , 
                      ( Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ) * sizeof( int ) );

    Noeud->TailleComplementDeBase = Bb->NbVarDeBaseComplementairesAEntierSup + Marge;
    /* Il faut dimensionner aux nombre de contraintes car le simplexe peut y ajouter des valeurs dans la limite du nombre 
       de contraintes => Noeud->TailleComplementDeBase est inutile */

    Noeud->ComplementDeLaBase = (int *) malloc( Bb->NombreDeContraintesDuProbleme /*Noeud->TailleComplementDeBase*/ * sizeof(int) );
    if ( Noeud->ComplementDeLaBase == NULL ) { 
      printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_AllouerUnNoeud \n");
      Bb->AnomalieDetectee = OUI;
      longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
    }

    Noeud->NbVarDeBaseComplementaires = Bb->NbVarDeBaseComplementairesAEntierSup;
    memcpy( (char * ) Noeud->ComplementDeLaBase , (char * ) Bb->ComplementDeLaBaseAEntierSup , 
                      Noeud->NbVarDeBaseComplementaires * sizeof( int ) );

    for ( i = 0 ; i < Bb->NombreDeCoupesAjoutees ; i++ ) {
      Noeud->NumeroDesCoupesExaminees[i] = Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[i];     
      Noeud->LaCoupeEstSaturee       [i] = Bb->CoupeSatureeAEntierSup[i];
    }

  }
  
}

/* Si les fils ont été crées on peut désallouer certaintes parties de la structure du noeud père */
if ( NoeudAntecedent->NoeudSuivantGauche != 0 && NoeudAntecedent->NoeudSuivantDroit != 0 ) {
  if ( NoeudAntecedent != Bb->NoeudRacine ) {
    /* On ne desalloue pas partiellement le noeud racine car on peut avoir besoin de sa base */
    BB_DesallocationPartielleDUnNoeud( NoeudAntecedent );
  }
}

return( Noeud );
}

/*-----------------------------------------------------------------------------------------------*/

void BB_DesallouerUnNoeud( BB * Bb, NOEUD * Noeud )    
{ 
int i; COUPE ** Coupe; 
					    
if( Noeud == 0 ) return;

/*
if( Noeud == Bb->NoeudRacine ) { printf(" Elimination du noeud racine \n"); }
*/

/*  
if( Noeud == Bb->NoeudEnExamen ) { 
  #if VERBOSE_BB
    printf(" Tentative elimination du noeud en examen qui est %x \n",Bb->NoeudEnExamen); 
  #endif
  return;
}
*/

for ( i = 0 ; i < Bb->NbNoeuds1_PNE_BalayageEnProfondeur ; i++ ) {	      
 if( Bb->Liste1_PNE_BalayageEnProfondeur[i] == Noeud ) {  
   Bb->Liste1_PNE_BalayageEnProfondeur[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur ; i++ ) {	      
 if( Bb->Liste1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur[i] == Noeud ) {  
   Bb->Liste1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur ; i++ ) {	      
 if( Bb->Liste2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur[i] == Noeud ) {  
   Bb->Liste2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds1_PNE_BalayageEnLargeur ; i++ ) {	      
 if( Bb->Liste1_PNE_BalayageEnLargeur[i] == Noeud ) {  
   Bb->Liste1_PNE_BalayageEnLargeur[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds2_PNE_BalayageEnLargeur ; i++ ) {	      
 if( Bb->Liste2_PNE_BalayageEnLargeur[i] == Noeud ) {  
   Bb->Liste2_PNE_BalayageEnLargeur[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux ; i++ ) {	      
 if( Bb->Liste1_PNE_EliminerLesNoeudsSousOptimaux[i] == Noeud ) {  
   Bb->Liste1_PNE_EliminerLesNoeudsSousOptimaux[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux ; i++ ) {	      
 if( Bb->Liste2_PNE_EliminerLesNoeudsSousOptimaux[i] == Noeud ) {  
   Bb->Liste2_PNE_EliminerLesNoeudsSousOptimaux[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud ; i++ ) {	      
 if( Bb->Liste1_PNE_SupprimerTousLesDescendantsDUnNoeud[i] == Noeud ) {  
   Bb->Liste1_PNE_SupprimerTousLesDescendantsDUnNoeud[i] = 0;
   break;
 }
}

for ( i = 0 ; i < Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud ; i++ ) {	      
 if( Bb->Liste2_PNE_SupprimerTousLesDescendantsDUnNoeud[i] == Noeud ) {  
   Bb->Liste2_PNE_SupprimerTousLesDescendantsDUnNoeud[i] = 0;
   break;
 }
}

BB_DesallocationPartielleDUnNoeud( Noeud );    

/* Liberation des coupes */

if( Noeud->NombreDeCoupesGenereesAuNoeud > 0 ) {
  Coupe = Noeud->CoupesGenereesAuNoeud; /* Pointeur sur le tableau de pointeurs sur les coupes */  
  for ( i = 0 ; i < Noeud->NombreDeCoupesGenereesAuNoeud; i++ ) {
    free( Coupe[i]->Coefficient ); 
    free( Coupe[i]->IndiceDeLaVariable );
    free( Coupe[i] );
  }
}

free( Noeud->NumeroDesCoupeAjouteeAuProblemeCourant ); 
free( Noeud->CoupesGenereesAuNoeud ); 
free( Noeud->NumeroDesCoupesExaminees ); 
free( Noeud->LaCoupeEstSaturee );
free( Noeud->NumeroDesCoupesViolees ); 

free( Noeud ); 

return;
}

/*-----------------------------------------------------------------------------------------------*/

void BB_DesallocationPartielleDUnNoeud( NOEUD * Noeud )    
{
	
free( Noeud->ValeursDesVariablesEntieresInstanciees ); 
free( Noeud->IndicesDesVariablesEntieresInstanciees ); 

Noeud->ValeursDesVariablesEntieresInstanciees = NULL;
Noeud->IndicesDesVariablesEntieresInstanciees = NULL;

free( Noeud->PositionDeLaVariable ); 
free( Noeud->ComplementDeLaBase );

Noeud->PositionDeLaVariable = NULL;
Noeud->ComplementDeLaBase   = NULL;

free( Noeud->PositionDeLaVariableSansCoupes ); 
free( Noeud->ComplementDeLaBaseSansCoupes );

Noeud->PositionDeLaVariableSansCoupes = NULL;
Noeud->ComplementDeLaBaseSansCoupes   = NULL;

free( Noeud->NumeroDeLaVariableModifiee ); 
free( Noeud->TypeDeBorneModifiee ); 
free( Noeud->NouvelleValeurDeBorne );

Noeud->NumeroDeLaVariableModifiee = NULL;
Noeud->TypeDeBorneModifiee = NULL;
Noeud->NouvelleValeurDeBorne = NULL;
Noeud->NombreDeBornesModifiees = 0;

if ( Noeud->BaseSimplexeDuNoeud != NULL ) {
  free( Noeud->BaseSimplexeDuNoeud->PositionDeLaVariable );
  free( Noeud->BaseSimplexeDuNoeud->InDualFramework );
  free( Noeud->BaseSimplexeDuNoeud->ContrainteDeLaVariableEnBase );
  free( Noeud->BaseSimplexeDuNoeud->DualPoids );
  free( Noeud->BaseSimplexeDuNoeud->VariableEnBaseDeLaContrainte );
  free( Noeud->BaseSimplexeDuNoeud );
  Noeud->BaseSimplexeDuNoeud = NULL;
}

return;
}

