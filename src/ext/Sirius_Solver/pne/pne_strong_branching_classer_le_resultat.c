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

   FONCTION: Strong branching pour le choix de la variable a instancier. 
             Appelé par le branch and bound, ce sous-programme retourne 
             le numéro de la variable a instancier et la base de depart 
             qu'il faudra utiliser lorsque l'on évaluera le noeud dans
             une étape ultérieure.
                            
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
							     
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# define SEUIL_ACCEPTATION_NOUVEAU_CRITERE 1.e-3 /*1.e-6*/ /*1.e-7*/
/*----------------------------------------------------------------------------*/

void PNE_StrongBranchingClasserLeResultat( PROBLEME_PNE * Pne,
          char ChoixDuTypeDeVariation, 
          char ExistenceSolutionAGauche,
	        char ExistenceSolutionADroite,
          double CritereAGauche,
	        double DeltaXAGauche,
	        double CritereADroite,
	        double DeltaXADroite,	  
	        double CritereInfini,
          double Marge,
          double * SupDeNouveauCritere,
          double * SupEcartDInstanciation,
          int VariableInstanciee,
          char * ChoixDefinitif,
	        char * ChoixAmeliorant,
          double SeuilNouveauCout,	  
          /* Choix de l'instanciation */
          double * MinorantEspereAGauche,
          double * MinorantEspereADroite,	  
	  /* Base correspondante pour l'instanciation a gauche */	  
          int   * PositionDeLaVariableAGauche      , 
          int   * NbVarDeBaseComplementairesAGauche,    
          int   * ComplementDeLaBaseAGauche        ,	  
          int   * PosVarAGauche    , 
          int     NbVarComplAGauche,    
          int   * ComplBaseAGauche ,
          char * PositionDeLaVariableDEcartAGauche,	  
	  /* Base correspondante pour l'instanciation a droite */
          int   * PositionDeLaVariableADroite      ,	 
          int   * NbVarDeBaseComplementairesADroite,    
          int   * ComplementDeLaBaseADroite        ,   	  
          int   * PosVarADroite    ,	 
          int     NbVarComplADroite,    
          int   * ComplBaseADroite ,        
          char * PositionDeLaVariableDEcartADroite,
          /* Indicateur de disponibilite des bases a gauche et droite */
          int   * BasesFilsDisponibles                     
                        )  
{
double NouveauCritere; double Seuil; double DeltaFSurDeltaXG; double DeltaFSurDeltaXD;
double X; char OnPrend; int i; int * NbTerm; 

if ( CritereAGauche < Pne->Critere ) CritereAGauche = Pne->Critere;
if ( CritereADroite < Pne->Critere ) CritereADroite = Pne->Critere;

*ChoixDefinitif  = NON_PNE;
*ChoixAmeliorant = NON_PNE;
OnPrend = NON_PNE;

if ( ChoixDuTypeDeVariation == VALEUR_ABSOLUE_DE_LA_VARIATION ) {
  /* NouveauCritere est la valeur qui va servir pour le test de selection */
  NouveauCritere = Pne->Critere;
  if ( ExistenceSolutionAGauche == OUI_PNE && ExistenceSolutionADroite == OUI_PNE ) {
    NouveauCritere = ( CritereAGauche + CritereADroite ) * 0.5;		
  }
		
  if ( ExistenceSolutionAGauche == NON_PNE ) NouveauCritere = CritereInfini;
  if ( ExistenceSolutionADroite == NON_PNE ) NouveauCritere = 2. * CritereInfini;	

  if ( ExistenceSolutionAGauche == NON_PNE && ExistenceSolutionADroite == NON_PNE ) {
    /* Pour que la variable soit choisie a coup sur */
    NouveauCritere = 10. * CritereInfini;
  }

  Seuil = *SupDeNouveauCritere;

	/* Pour pourvoir tester toutes les instanciations mais conserver le fait qu'on a interet a choisir une
	   variable qui n'a pas de solution ni a droite ni a gauche ainsi on pourra fermer l'arborescence */
  SeuilNouveauCout = /*2*/ 0.99 * CritereInfini;	

  if ( NouveauCritere > SeuilNouveauCout ) {
    OnPrend = OUI_PNE;
  }
  else if ( NouveauCritere > Seuil && fabs( CritereAGauche - CritereADroite ) > SEUIL_ACCEPTATION_NOUVEAU_CRITERE ) {
    OnPrend = OUI_PNE;
    if ( fabs( NouveauCritere - Pne->Critere ) < SEUIL_ACCEPTATION_NOUVEAU_CRITERE ) {
      if ( fabs( CritereAGauche - CritereADroite ) < *SupEcartDInstanciation) {
        OnPrend = NON_PNE;
      }
    }
  }
	goto TestDesVariation;
}

if ( ChoixDuTypeDeVariation == PENTE_DE_LA_VARIATION ) {
  /* NouveauCritere est le min des pentes de variation */	
  DeltaFSurDeltaXG = 0.0;
  if ( ExistenceSolutionAGauche == OUI_PNE ) {
    X = CritereAGauche - Pne->Critere;

		/* Neutralisation des trop petites variations */
    if ( X < 1.e-6 ) X = 0.0;
		
    if ( X > 0.0 ) {
      DeltaFSurDeltaXG = X / DeltaXAGauche;
    }				
  }
  DeltaFSurDeltaXD = 0.0;
  if ( ExistenceSolutionADroite == OUI_PNE ) {
    X = CritereADroite - Pne->Critere;

		/* Neutralisation des trop petites variations */
    if ( X < 1.e-6 ) X = 0.0;
		
    if ( X > 0.0 ) {
      DeltaFSurDeltaXD = X / DeltaXADroite;
    }
  }

  if ( ExistenceSolutionAGauche == NON_PNE ) DeltaFSurDeltaXG = CritereInfini;
  if ( ExistenceSolutionADroite == NON_PNE ) DeltaFSurDeltaXD = CritereInfini;	

  NouveauCritere = DeltaFSurDeltaXG;
  if ( NouveauCritere > DeltaFSurDeltaXD ) NouveauCritere = DeltaFSurDeltaXD;

  if ( ExistenceSolutionAGauche == NON_PNE && ExistenceSolutionADroite == NON_PNE ) {
    /* Pour que la variable soit choisie a coup sur */
    NouveauCritere = 10. * CritereInfini;
  }
	
  Seuil = *SupDeNouveauCritere;

	/* Pour pourvoir tester toutes les instanciations mais conserver le fait qu'on a interet a choisir une
	   variable qui n'a pas de solution ni a droite ni a gauche ainsi on pourra fermer l'arborescence */
  SeuilNouveauCout = /*2*/ 0.99 * CritereInfini;

  if ( NouveauCritere > SeuilNouveauCout ) {
    OnPrend = OUI_PNE;
  }
  else if ( NouveauCritere > Seuil && fabs( CritereAGauche - CritereADroite ) > SEUIL_ACCEPTATION_NOUVEAU_CRITERE ) {
    OnPrend = OUI_PNE;
    if ( fabs( NouveauCritere ) < SEUIL_ACCEPTATION_NOUVEAU_CRITERE ) {
      if ( fabs( CritereAGauche - CritereADroite ) < *SupEcartDInstanciation) {
        OnPrend = NON_PNE;
      }
    }
  }

	/* 12/2014 si les 2 criteres sont vraiment trop proches on ne prend pas de toutes facons */
  if ( fabs( CritereAGauche - CritereADroite ) < SEUIL_ACCEPTATION_NOUVEAU_CRITERE ) {
	  /*if ( OnPrend == OUI_PNE ) printf("Refus car critere a gauche et a droite identiques\n");*/
    OnPrend = NON_PNE;
  }
	
  goto TestDesVariation;
}

TestDesVariation:

if ( OnPrend == OUI_PNE ) {
     
  *SupDeNouveauCritere    = NouveauCritere;
  *SupEcartDInstanciation = fabs( CritereAGauche - CritereADroite );
  *ChoixAmeliorant        = OUI_PNE;
  
  if ( VariableInstanciee >= 0 ) {
    /* Sinon c'est une Gub */
    Pne->VariableLaPlusFractionnaire = VariableInstanciee;
  }
  /* On s'arrete sur DeltaCoutNonNul que si les 2 couts sont valides ou invalides a la fois */
  if ( NouveauCritere > SeuilNouveauCout ) {		
    *ChoixDefinitif = OUI_PNE;
  }

	X = 0.001 * fabs( NouveauCritere );
	/* On borne la valeur de X */	
	
	if ( X > 10 ) X = 10;
	else if ( X < 1.e-6 ) X = 1.e-6;
	
  *SupDeNouveauCritere += X;
	  
  *MinorantEspereAGauche = CritereAGauche - Marge;
  *MinorantEspereADroite = CritereADroite - Marge;
  *BasesFilsDisponibles  = OUI_PNE;	 
  *NbVarDeBaseComplementairesAGauche = NbVarComplAGauche;
  *NbVarDeBaseComplementairesADroite = NbVarComplADroite;
  memcpy( (char *) PositionDeLaVariableAGauche , (char *) PosVarAGauche     , Pne->NombreDeVariablesTrav * sizeof( int ) );
  memcpy( (char *) ComplementDeLaBaseAGauche   , (char *) ComplBaseAGauche  , NbVarComplAGauche         * sizeof( int ) );  
  memcpy( (char *) PositionDeLaVariableADroite , (char *) PosVarADroite     , Pne->NombreDeVariablesTrav * sizeof( int ) );
  memcpy( (char *) ComplementDeLaBaseADroite   , (char *) ComplBaseADroite  , NbVarComplADroite         * sizeof( int ) );

  /* Pour ne pas avoir de fausses alertes avec les outils de debogage au sujet des zones non intitialisees on
	   fait une init a EN_BASE_LIBRE (en effet, il arrive que l'on inihibe des coupes et dans ce cas le simplexe
		 ne donne pas de position pour la variable d'ecart de ces coupes */
  NbTerm = Pne->Coupes.NbTerm;		 
  for ( i = 0 ; i < Pne->Coupes.NombreDeContraintes ; i++ ) {
	  if ( NbTerm[i] == 0 ) {
      PositionDeLaVariableDEcartAGauche[i] = EN_BASE_LIBRE;
      PositionDeLaVariableDEcartADroite[i] = EN_BASE_LIBRE;
		}
	}
	
  memcpy( (char *) Pne->Coupes.PositionDeLaVariableDEcartAGauche, 
          (char *) PositionDeLaVariableDEcartAGauche, Pne->Coupes.NombreDeContraintes * sizeof( char ) );
  memcpy( (char *) Pne->Coupes.PositionDeLaVariableDEcartADroite, 
          (char *) PositionDeLaVariableDEcartADroite, Pne->Coupes.NombreDeContraintes * sizeof( char ) );
		       
}

return;
}



