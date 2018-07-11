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

   FONCTION: Preparation des coupes a passer a la resolution du 
   probleme en un noeud donne.
            
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

#define NOMBRE_MAX_DE_COUPES_HORS_BRANCHE   100000 /* Pas de limite */ /*10*/ /*100*/

#define NOMBRE_MAX_DE_COUPES_VIOLEES_AJOUTEES 100000 /* Pas de limite */ /*100*/ /*50*/ 

#define NOMBRE_MAX_DE_TERMES_DES_COUPES_INSEREES 500000

#define NOMBRE_MAX_DE_COUPES_DE_CODE_MOINS_1  100000 /* Pas de limite */ /*100*/ /*50*/

#define NOMBRE_MAX_DE_COUPES_PAR_PROBLEME_RELAXE  500 

void BB_EchangerViolation( double * , int * , int , int );
int BB_PartitionTriRapideViolations( double * , int * , int , int );
void BB_TriRapideViolations( double * , int * , int , int );

/*----------------------------------------------------------------------------*/
void BB_EchangerViolation( double * Tableau1 , int * Tableau2 , int i, int j )
{
double X; int k;
X  = Tableau1[i];  
k  = Tableau2[i];
Tableau1[i] = Tableau1[j];
Tableau2[i] = Tableau2[j];
Tableau1[j] = X;
Tableau2[j] = k;
return;
}
/*----------------------------------------------------------------------------*/
int BB_PartitionTriRapideViolations( double * Tableau1 , int * Tableau2 , int Deb, int Fin )
{
int Compt; double Pivot; int i; double X; int k;
Compt = Deb;
Pivot = Tableau1[Deb];
for ( i = Deb+1 ; i <= Fin ; i++) {
  if ( Tableau1[i] > Pivot) {
    Compt++;
    /*BB_EchangerViolation( Tableau1 , Tableau2 , Compt , i);*/		
    X = Tableau1[Compt];
    k = Tableau2[Compt];		
    Tableau1[Compt] = Tableau1[i];		
    Tableau2[Compt] = Tableau2[i];
    Tableau1[i] = X;
    Tableau2[i] = k;				
  }
}
/*BB_EchangerViolation( Tableau1 , Tableau2 , Compt , Deb);*/
X = Tableau1[Compt];
k = Tableau2[Compt];
Tableau1[Compt] = Tableau1[Deb];
Tableau2[Compt] = Tableau2[Deb];
Tableau1[Deb] = X;
Tableau2[Deb] = k;
return(Compt);
}
/*----------------------------------------------------------------------------*/
void BB_TriRapideViolations( double * Tableau1 , int * Tableau2 , int Debut , int Fin )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = BB_PartitionTriRapideViolations( Tableau1 , Tableau2 , Debut , Fin );
  BB_TriRapideViolations( Tableau1 , Tableau2 , Debut   , Pivot-1 );
  BB_TriRapideViolations( Tableau1 , Tableau2 , Pivot+1 , Fin     );
}
return;
}
/*---------------------------------------------------------------------------------------------------------*/
/* SP appele par la partie PNE pour determiner les coupes violees qui donc, n'avaient pas ete incluses
   dans le probleme */
void BB_RechercherLesCoupesViolees( BB * Bb, double * U )
{
COUPE ** Coupe; COUPE * Cpe; int NumeroDeCoupe; int il; double X; double Marge; NOEUD * NoeudCourant; int Pas;
int Capacite; int i; double * Violation; char OnContinue; double CumulDesViolations; int FinBoucle;
double * Coefficient; int * NumeroDesCoupeAjouteeAuProblemeCourant; int * IndiceDeLaVariable;

if ( Bb->ControlerLesCoupesNonInclusesPourUnNouvelleResolution == OUI ) {
  /* Dans ce cas la resolution du noeud est eventuellement relancee si des coupes non
     incluses sont violees */
  return;
}

NoeudCourant = Bb->NoeudEnExamen;

Marge              = 1.e-2 /*1.e-4*/; /* Il ne faut pas etre trop severe sinon on surcharge inutilement la matrice des contraintes */
Pas                = 20;
Capacite           = Pas;
CumulDesViolations = 0.0;

NumeroDesCoupeAjouteeAuProblemeCourant = Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant;

NoeudCourant->NumeroDesCoupesViolees = (int *)    malloc( Capacite * sizeof( int    ) );
Violation                            = (double *) malloc( Capacite * sizeof( double ) );
if ( NoeudCourant->NumeroDesCoupesViolees == NULL || Violation == NULL ) {
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_RechercherLesCoupesViolees\n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee );  
}

Coupe = Bb->NoeudRacine->CoupesGenereesAuNoeud;  
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
  Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = NON;
}
for ( i = 0 ; i < Bb->NombreDeCoupesAjoutees ; i++ ) {
  Coupe[NumeroDesCoupeAjouteeAuProblemeCourant[i]]->CoupeExamineeAuNoeudCourant = OUI;
}

/* Marquage des coupes que l'on pourra utiliser */
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
  if ( Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant == OUI ) continue;
  X = 0.;
  Cpe = Coupe[NumeroDeCoupe];
  Coefficient = Cpe->Coefficient;
  IndiceDeLaVariable = Cpe->IndiceDeLaVariable;
  for ( il = 0 ; il < Cpe->NombreDeTermes ; il++ ) {
    X += Coefficient[il] * U[IndiceDeLaVariable[il]];
  }  
  if ( X > Cpe->SecondMembre + Marge ) {
    if ( NoeudCourant->NombreDeCoupesViolees >= Capacite ) {
      Capacite += Pas;
      NoeudCourant->NumeroDesCoupesViolees = (int *)    realloc( NoeudCourant->NumeroDesCoupesViolees , Capacite * sizeof( int   ) );
      Violation                            = (double *) realloc( Violation                            , Capacite * sizeof( double ) );
      if ( NoeudCourant->NumeroDesCoupesViolees == NULL ) {
        printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_RechercherLesCoupesViolees\n");
        Bb->AnomalieDetectee = OUI;
        longjmp( Bb->EnvBB , Bb->AnomalieDetectee );  
      }     
    }
    Violation[NoeudCourant->NombreDeCoupesViolees] = X - Cpe->SecondMembre;
    CumulDesViolations += Violation[NoeudCourant->NombreDeCoupesViolees];
    NoeudCourant->NumeroDesCoupesViolees[NoeudCourant->NombreDeCoupesViolees] = NumeroDeCoupe;
    NoeudCourant->NombreDeCoupesViolees++;
  }
}

/* On Classe dans l'ordre decroissant des violation */
/*
if ( NoeudCourant->NombreDeCoupesViolees > 1 ) {
  BB_TriRapideViolations( Violation , NoeudCourant->NumeroDesCoupesViolees ,
                          0 , NoeudCourant->NombreDeCoupesViolees - 1 );
}
*/
/* De façon inattendue, le tri rapide ci-dessus est generalement plus long que le tri bulle
   ci-dessous. L'explication reside dans le fait qu'il y a le plus souvent un petit nombre
	 de coupes violees, et que l'on refait donc peu de fois la boucle de classement. */

/* Comme on prend toutes les coupes violees, le classement est inutile */
OnContinue = OUI;
FinBoucle = NoeudCourant->NombreDeCoupesViolees - 1;
/*
while ( OnContinue == OUI ) {
  OnContinue = NON; 
  for ( i = 0 ; i < FinBoucle ; i++ ) {    
    if ( Violation[i] < Violation[i+1] ) {
      OnContinue = OUI;
      il = NoeudCourant->NumeroDesCoupesViolees[i+1];
      X  = Violation[i+1];
      NoeudCourant->NumeroDesCoupesViolees[i+1] = NoeudCourant->NumeroDesCoupesViolees[i];
      NoeudCourant->NumeroDesCoupesViolees[i]   = il;
      Violation[i+1] = Violation[i];      
      Violation[i]   = X;
    }
  }
	FinBoucle--;
}
*/
/*
printf("############## NombreDeCoupes Violees: %d cumul des violations %f #################\n",
       NoeudCourant->NombreDeCoupesViolees, CumulDesViolations);
*/
free( Violation );

return;
}

/*---------------------------------------------------------------------------------------------------------*/
void BB_DemanderUneNouvelleResolutionDuProblemeRelaxe( BB * Bb )
{
Bb->ComplementDeBaseModifie = OUI;
Bb->MajorantDuNombreDeCoupesAjouteesApresResolutionDuProblemeRelaxe++;
return;
}

/*---------------------------------------------------------------------------------------------------------*/

void BB_InsererLesCoupesDansLeProblemeCourant( BB * Bb, NOEUD * NoeudDeDepart /* C'est toujours le noeud du probleme relaxe */ )  
{
NOEUD * NoeudCourant     ; COUPE ** Coupe; int i; int NumeroDeCoupe; NOEUD * NoeudPere; 
int Nb       ; int NombreMaxDeCoupesAjouteesHorsBranche    ; int Count; int NbMax;
int NbLift; int NbGom; int NbLiftTot; int NbGomTot; int NbI; int NombreMaxDeTermesDesCoupesInserees;
int NbLiftSat; int NbK; int NbKTot; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;

Bb->NombreMaxDeCoupesParProblemeRelaxe = NOMBRE_MAX_DE_COUPES_PAR_PROBLEME_RELAXE;

i = Pne->MdebTrav[Pne->NombreDeContraintesTrav-1] + Pne->NbTermTrav[Pne->NombreDeContraintesTrav-1];
NombreMaxDeTermesDesCoupesInserees = 10 * i;
if ( NombreMaxDeTermesDesCoupesInserees > NOMBRE_MAX_DE_TERMES_DES_COUPES_INSEREES ) {
  NombreMaxDeTermesDesCoupesInserees = NOMBRE_MAX_DE_TERMES_DES_COUPES_INSEREES;
}

NbLift = 0;
NbGom  = 0;
NbLiftTot = 0;
NbGomTot = 0;
NbLiftSat = 0;
NbK = 0;
NbKTot = 0;
NbI = 0;

#if VERBOSE_BB
  printf("-> Ajout des coupes avant resolution du probleme relaxe\n");
#endif

Bb->NombreDeCoupesAjoutees = 0;

if ( Bb->EcartBorneInf > 100. ) NombreMaxDeCoupesAjouteesHorsBranche = NOMBRE_MAX_DE_COUPES_HORS_BRANCHE;
else NombreMaxDeCoupesAjouteesHorsBranche = (int) ceil( 0.5 * Bb->NombreMaxDeCoupesParProblemeRelaxe );

/* Recuperation du probleme initial , c'est a dire le probleme sans les coupes ajoutees 
   pendant le branch and bound */
PNE_RecupererLeProblemeInitial( Pne );

NoeudPere = NoeudDeDepart->NoeudAntecedent;

#if VERBOSE_BB
  printf("-> Nombre de coupes disponibles dans le pool: %d\n",Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud);
#endif

/* Marquage initial des coupes */
Coupe = Bb->NoeudRacine->CoupesGenereesAuNoeud;
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {

  if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'L' ) NbLiftTot++;
  if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'G' ) NbGomTot++; 
  if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'K' ) NbKTot++;
   
  Coupe[NumeroDeCoupe]->UtiliserLaCoupe = NON;
  Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = NON;    
  Coupe[NumeroDeCoupe]->CoupeRencontreeDansLArborescence = NON;

}

Nb = 0;
NoeudCourant = NoeudDeDepart;
for ( i = 0 ; i < NoeudCourant->NombreDeCoupesExaminees ; i++ ) {
  NumeroDeCoupe = NoeudCourant->NumeroDesCoupesExaminees[i];
  Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = OUI;      
  if ( NoeudCourant->LaCoupeEstSaturee[i] == OUI ) {
    Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
    Coupe[NumeroDeCoupe]->CoupeSaturee    = OUI_PNE;   
  }
  /* Si la coupe a un code saturation de -1 on la prend en compte et on la met non saturee pour que la variable 
     d'ecart soit basique, ceci a condition qu'elle n'ait pas ete rencontree dans un fils */      
  if ( NoeudCourant->LaCoupeEstSaturee[i] == -1 && Coupe[NumeroDeCoupe]->UtiliserLaCoupe == NON &&
       Nb < NOMBRE_MAX_DE_COUPES_DE_CODE_MOINS_1 ) { 
    Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
    Coupe[NumeroDeCoupe]->CoupeSaturee    = NON_PNE;
    Nb++;
  }
}

/* On ajoute les coupes qui n'avaient pas ete ajoutees au noeud pere mais qui se sont retrouvees violees
   quand-meme */
Nb = 0;
NoeudCourant = NoeudDeDepart->NoeudAntecedent;
if ( NoeudCourant != 0 ) {  
  for ( i = 0 ; i < NoeudCourant->NombreDeCoupesViolees ; i++ ) {
    if ( Nb > NOMBRE_MAX_DE_COUPES_VIOLEES_AJOUTEES ) break;  
    if ( Nb > Bb->NombreMaxDeCoupesParProblemeRelaxe ) break;
    NumeroDeCoupe = NoeudCourant->NumeroDesCoupesViolees[i];
    /*if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'L' ) continue;*/
    if ( Coupe[NumeroDeCoupe]->UtiliserLaCoupe == OUI ) continue;
  
    Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
    Coupe[NumeroDeCoupe]->CoupeSaturee    = NON_PNE;
    Nb++;    
  }
}
/*
printf("Nombre de coupes non saturees violees ajoutees: %d\n",Nb);
*/
/* On prepare les informations qui contiendront la liste des coupes saturees apres la resolution du noeud */
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud; NumeroDeCoupe++ ) {
  if ( Coupe[NumeroDeCoupe]->UtiliserLaCoupe == OUI ) { 
    Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[Bb->NombreDeCoupesAjoutees] = NumeroDeCoupe;
    Bb->NombreDeCoupesAjoutees++; 
  }     
}

/* Si une coupe a ete creee dans une autre arborescence et qu'elle n'a pas encore ete testee dans l'arborescence 
   qui part du noeud en examen, on la teste. Mais il faut la declarer comme non saturee afin d'avoir une 
   base de depart. Ceci, dans la limite du nombre max de coupes que l'on s'autorise a integrer dans un probleme 
   relaxe */
NoeudCourant = NoeudDeDepart;
while ( 1 ) {
  if ( NoeudCourant == 0 ) break;
  for ( i = 0 ; i < NoeudCourant->NombreDeCoupesExaminees ; i++ ) {
    NumeroDeCoupe = NoeudCourant->NumeroDesCoupesExaminees[i];
    Coupe[NumeroDeCoupe]->CoupeRencontreeDansLArborescence = OUI;  
  }
  NoeudCourant = NoeudCourant->NoeudAntecedent;
}

for ( NumeroDeCoupe = 0 , Nb = 0 ;
      NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud     &&
      Bb->NombreDeCoupesAjoutees < Bb->NombreMaxDeCoupesParProblemeRelaxe &&
      Nb < NombreMaxDeCoupesAjouteesHorsBranche ;
      NumeroDeCoupe++ ) {
  if ( Coupe[NumeroDeCoupe]->UtiliserLaCoupe == NON && Coupe[NumeroDeCoupe]->CoupeRencontreeDansLArborescence == NON ) {      
    Coupe[NumeroDeCoupe]->CoupeSaturee = NON_PNE;
    Nb++;
    Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[Bb->NombreDeCoupesAjoutees] = NumeroDeCoupe;
    Bb->NombreDeCoupesAjoutees++; 
  }       
}

if ( Bb->NombreDeCoupesAjoutees == 0 ) goto FinInsertionCoupes;

/* On ajoute les coupes */
Count = 0;
NbMax = Bb->NombreDeCoupesAjoutees;
Bb->NombreDeCoupesAjoutees = 0;
for ( Nb = 0 ; Nb < NbMax ; Nb++ ) {
  NumeroDeCoupe = Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[Nb];

  Count+= Coupe[NumeroDeCoupe]->NombreDeTermes;

  /* Attention, on ne peut pas mettre de limitation ici car il se peut qu'en le faisant on enleve
     une coupe saturee et donc on se retrouve avec une base non inversible */
  /* if ( Count > NombreMaxDeTermesDesCoupesInserees ) break; */
 
  Bb->NombreDeCoupesAjoutees++;
    
  if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'L' ) {
    NbLift++;
    if ( Coupe[NumeroDeCoupe]->CoupeSaturee == OUI_PNE ) NbLiftSat++;
  }
  else if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'G' ) NbGom++;
  else if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'K' ) NbK++;
  else if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'I' ) NbI++;
  
  PNE_InsererUneContrainte( Pne,
                            Coupe[NumeroDeCoupe]->NombreDeTermes, 
                            Coupe[NumeroDeCoupe]->Coefficient, 
                            Coupe[NumeroDeCoupe]->IndiceDeLaVariable,  
                            Coupe[NumeroDeCoupe]->SecondMembre,
                            Coupe[NumeroDeCoupe]->CoupeSaturee,
                            Coupe[NumeroDeCoupe]->TypeDeCoupe
														);  
}

FinInsertionCoupes:
/*
printf(" Nombre de Gomory ajoutees : %d sur % d\n",NbGom,NbGomTot);
printf(" Nombre de Knapsack ajoutees : %d sur % d\n",NbK,NbKTot);
*/
#if VERBOSE_BB
  printf("-> Nombre de coupes ajoutees avant resolution du probleme relaxe: %d    Profondeur du noeud: %d\n",
             Bb->NombreDeCoupesAjoutees, NoeudDeDepart->ProfondeurDuNoeud);   
#endif
						 
Bb->NombreTotalDeCoupesDuPoolUtilisees += Bb->NombreDeCoupesAjoutees;

Bb->NombreTotalDeGDuPoolUtilisees += NbGom;
Bb->NombreTotalDeIDuPoolUtilisees += NbI;;
Bb->NombreTotalDeKDuPoolUtilisees += NbK;

return; 
}

