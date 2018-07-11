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

   FONCTION: Resolution du probleme relaxe par le point interieur
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "pi_define.h"
# include "pi_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_SolvePbRlxPi( PROBLEME_PNE * Pne, double * Critere, int * Faisabilite )
{
int i; int il; int ilMax; double S; int PourJeanYves;

double * Qtrav; 

/*double * VariablesDualesDesContraintes;*/

double ToleranceSurLAdmissibilite   ; int ChoixToleranceParDefautSurLAdmissibilite   ;
double ToleranceSurLaStationnarite  ; int ChoixToleranceParDefautSurLaStationnarite  ;
double ToleranceSurLaComplementarite; int ChoixToleranceParDefautSurLaComplementarite;

PROBLEME_POINT_INTERIEUR Probleme; char * VariableBinaire;

char LigneLue[128]; char LabelDeLaVariableLue[128]; char ValeurLue[128]; int NbChamps; int LgMax; 
FILE * Flot; 

*Faisabilite = OUI_PNE;					 

PourJeanYves = 0; 

/* Allocations temporaires */
VariableBinaire = (char *) malloc( Pne->NombreDeVariablesTrav * sizeof( char ) );
if ( VariableBinaire == NULL ) {
  printf(" Memoire insuffisante. Sous-programme: PNE_SolvePbRlxPi\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  if ( Pne->TypeDeVariableTrav[i] == ENTIER ) VariableBinaire[i] = OUI_PI;
  else VariableBinaire[i] = NON_PI;
}

Qtrav = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );

free( Pne->VariablesDualesDesContraintesTravEtDesCoupes );
Pne->VariablesDualesDesContraintesTravEtDesCoupes = NULL;
Pne->VariablesDualesDesContraintesTravEtDesCoupes = (double *) malloc( ( Pne->NombreDeContraintesTrav + Pne->Coupes.NombreDeContraintes ) * sizeof( double ) ); 
 
if ( Qtrav == NULL || Pne->VariablesDualesDesContraintesTravEtDesCoupes == NULL ) {
  printf(" PNE, memoire insuffisante dans le sous programme  PNE_SolvePbRlx \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Qtrav[i] = 0.0;

/* Temporairement pour Jean-Yves et Aurelie, ouverture d'un fichier de couts quadratique */
if ( PourJeanYves == 0 ) goto OnOptimise;
Flot = fopen( "A_JEU_DE_DONNEES_QUADR", "r" ); 
if( Flot == NULL ) {
  printf("Erreur ouverture du fichier contenant les couts quadratiques \n");
  exit(0);
}
LgMax = 128;
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  fgets( LigneLue , LgMax , Flot );
  NbChamps = sscanf( LigneLue , "%s %s" , LabelDeLaVariableLue , ValeurLue ); 
  if ( NbChamps <= 0 ) break;
  Qtrav[i] = atof( ValeurLue );
  /*printf("Cout quadratique de la variable %d nommee %s = %e\n",i,LabelDeLaVariableLue,Qtrav[i]);*/
}
/* Fin pour Jean-Yves et Aurelie, ouverture d'un fichier de couts quadratique */

OnOptimise:
ToleranceSurLAdmissibilite               = 1.e-6/*1.e-5*/; 
ChoixToleranceParDefautSurLAdmissibilite = NON_PNE; 

ToleranceSurLaStationnarite               = 1.e-6; 
ChoixToleranceParDefautSurLaStationnarite = NON_PNE; /* Pour prendre la tolerance par defaut du point interieur */

ToleranceSurLaComplementarite               = 1.e-6; 
ChoixToleranceParDefautSurLaComplementarite = NON_PNE;

/* Appel du point interieur */
printf("----------------------------------------------------------\n");
printf("Appel de Pi_Quamin nombre de variables %d contraintes %d\n",Pne->NombreDeVariablesTrav,Pne->NombreDeContraintesTrav);

Probleme.NombreDeVariables = Pne->NombreDeVariablesTrav;
Probleme.CoutQuadratique   = Qtrav;
Probleme.CoutLineaire      = Pne->LTrav;
Probleme.X                 = Pne->UTrav;  
Probleme.Xmin              = Pne->UminTrav;
Probleme.Xmax              = Pne->UmaxTrav;
Probleme.TypeDeVariable    = Pne->TypeDeBorneTrav;
Probleme.VariableBinaire   = VariableBinaire;  
Probleme.NombreDeContraintes = Pne->NombreDeContraintesTrav;
Probleme.IndicesDebutDeLigne = Pne->MdebTrav;
Probleme.NombreDeTermesDesLignes = Pne->NbTermTrav;
Probleme.IndicesColonnes = Pne->NuvarTrav;
Probleme.CoefficientsDeLaMatriceDesContraintes = Pne->ATrav;
Probleme.Sens = Pne->SensContrainteTrav;
Probleme.SecondMembre = Pne->BTrav;
Probleme.NombreMaxDIterations = -1;
Probleme.AffichageDesTraces = OUI_PI;
Probleme.UtiliserLaToleranceDAdmissibiliteParDefaut = ChoixToleranceParDefautSurLAdmissibilite;
Probleme.ToleranceDAdmissibilite = ToleranceSurLAdmissibilite;
Probleme.UtiliserLaToleranceDeStationnariteParDefaut = ChoixToleranceParDefautSurLaStationnarite;
Probleme.ToleranceDeStationnarite = ToleranceSurLaStationnarite;
Probleme.UtiliserLaToleranceDeComplementariteParDefaut = ChoixToleranceParDefautSurLaComplementarite;
Probleme.ToleranceDeComplementarite = ToleranceSurLaComplementarite;
Probleme.CoutsMarginauxDesContraintes = Pne->VariablesDualesDesContraintesTravEtDesCoupes;
Probleme.CoutsMarginauxDesContraintesDeBorneInf = Pne->S1Trav;
Probleme.CoutsMarginauxDesContraintesDeBorneSup = Pne->S2Trav;

PI_Quamin( &Probleme );

if ( Probleme.ExistenceDUneSolution == OUI_PI ) *Faisabilite = OUI_PNE;
else *Faisabilite = NON_PNE;

/*                           FIN  POINT INTERIEUR                      */

/* Synthese des résultats */
printf("\n Resultat de la resolution du probleme relaxe \n");

for ( *Critere = Pne->Z0 , i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  *Critere+= Pne->LTrav[i] * Pne->UTrav[i];
  *Critere+= Qtrav[i] * Pne->UTrav[i] * Pne->UTrav[i];
}
printf("  Valeur du critere recalculee en sortie de l'optimisation %lf \n",*Critere); 

for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
  S     = 0.;
  il    = Pne->MdebTrav[i];
  ilMax = Pne->MdebTrav[i] + Pne->NbTermTrav[i];
  while ( il < ilMax ) {
    S+= Pne->ATrav[il] * Pne->UTrav[ Pne->NuvarTrav[il] ];
    il++;
  }

  if ( Pne->SensContrainteTrav[i] == '=' ) {
    if ( fabs( Pne->BTrav[i] - S ) > 1.e-3 ) {    
      printf(" Contrainte type = numero %d non satisfaite. Second membre %lf calcul %lf ecart %lf\n",i,Pne->BTrav[i],S,Pne->BTrav[i] - S);    
      *Faisabilite = NON_PNE; 
    }
  }
  else if ( Pne->SensContrainteTrav[i] == '<' ) { 
    if ( S > Pne->BTrav[i] + 1.e-3 ) {    
      printf(" Contrainte type < numero %d non satisfaite. Second membre %lf calcul %lf ecart %lf\n",i,Pne->BTrav[i],S,Pne->BTrav[i] - S);    
      *Faisabilite = NON_PNE;
    }
  }
}

free( VariableBinaire);
VariableBinaire = NULL;

free( Qtrav );
Qtrav = NULL;

return;
}
