/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/







# ifdef __CPLUSPLUS
  extern "C"
  {
# endif

# include "simplexe/spx_definition_arguments.h"
# include "simplexe/spx_fonctions.h"

# ifdef __CPLUSPLUS
  }
# endif

# include "h2o2_j_donnees_mensuelles.h"
# include "h2o2_j_fonctions.h"

# ifdef _MSC_VER
#	define SNPRINTF sprintf_s
# else
#	define SNPRINTF snprintf
# endif



void H2O2_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES_ETENDUES * DonneesMensuelles , int NumeroDeProbleme )
{
int Var; double * pt; char PremierPassage;

PROBLEME_HYDRAULIQUE_ETENDU * ProblemeHydrauliqueEtendu;
PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE * ProblemeLineaireEtenduPartieVariable;
PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE * ProblemeLineaireEtenduPartieFixe;

PROBLEME_SIMPLEXE * Probleme; PROBLEME_SPX * ProbSpx;

PremierPassage = OUI;

ProblemeHydrauliqueEtendu = DonneesMensuelles->ProblemeHydrauliqueEtendu;

ProblemeLineaireEtenduPartieVariable = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[NumeroDeProbleme];
ProblemeLineaireEtenduPartieFixe = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[NumeroDeProbleme];

ProbSpx = (PROBLEME_SPX *) ProblemeHydrauliqueEtendu->ProblemeSpx[NumeroDeProbleme];

Probleme = (PROBLEME_SIMPLEXE *) ProblemeHydrauliqueEtendu->Probleme;
if ( Probleme == NULL ) {
  Probleme = (PROBLEME_SIMPLEXE *) malloc( sizeof( PROBLEME_SIMPLEXE ) );
	if ( Probleme == NULL ) {
    DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;		
    return;	
	}
	ProblemeHydrauliqueEtendu->Probleme = (void *) Probleme;
}

RESOLUTION:

if ( ProbSpx == NULL ) {
	Probleme->Contexte            = SIMPLEXE_SEUL;
	Probleme->BaseDeDepartFournie = NON_SPX;
}
else {
	
	Probleme->Contexte            = BRANCH_AND_BOUND_OU_CUT_NOEUD;
	
	Probleme->BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;

	
	SPX_ModifierLeVecteurSecondMembre( ProbSpx, ProblemeLineaireEtenduPartieVariable->SecondMembre, ProblemeLineaireEtenduPartieFixe->Sens, ProblemeLineaireEtenduPartieFixe->NombreDeContraintes );
}


Probleme->NombreMaxDIterations = -1; 
Probleme->DureeMaxDuCalcul     = -1.;

Probleme->CoutLineaire      = ProblemeLineaireEtenduPartieFixe->CoutLineaire;
Probleme->X                 = ProblemeLineaireEtenduPartieVariable->X;
Probleme->Xmin              = ProblemeLineaireEtenduPartieVariable->Xmin;
Probleme->Xmax              = ProblemeLineaireEtenduPartieVariable->Xmax;
Probleme->NombreDeVariables = ProblemeLineaireEtenduPartieFixe->NombreDeVariables;
Probleme->TypeDeVariable    = ProblemeLineaireEtenduPartieFixe->TypeDeVariable;

Probleme->NombreDeContraintes                   = ProblemeLineaireEtenduPartieFixe->NombreDeContraintes;
Probleme->IndicesDebutDeLigne                   = ProblemeLineaireEtenduPartieFixe->IndicesDebutDeLigne;
Probleme->NombreDeTermesDesLignes               = ProblemeLineaireEtenduPartieFixe->NombreDeTermesDesLignes;
Probleme->IndicesColonnes                       = ProblemeLineaireEtenduPartieFixe->IndicesColonnes;
Probleme->CoefficientsDeLaMatriceDesContraintes = ProblemeLineaireEtenduPartieFixe->CoefficientsDeLaMatriceDesContraintes;
Probleme->Sens                                  = ProblemeLineaireEtenduPartieFixe->Sens;
Probleme->SecondMembre                          = ProblemeLineaireEtenduPartieVariable->SecondMembre;

Probleme->ChoixDeLAlgorithme = SPX_DUAL;

Probleme->TypeDePricing               = PRICING_STEEPEST_EDGE ;
Probleme->FaireDuScaling              = OUI_SPX ;
Probleme->StrategieAntiDegenerescence = AGRESSIF;

Probleme->PositionDeLaVariable       = ProblemeLineaireEtenduPartieVariable->PositionDeLaVariable;
Probleme->NbVarDeBaseComplementaires = 0; 
Probleme->ComplementDeLaBase         = ProblemeLineaireEtenduPartieVariable->ComplementDeLaBase;

Probleme->LibererMemoireALaFin = NON_SPX;

Probleme->UtiliserCoutMax = NON_SPX;
Probleme->CoutMax = 0.0;

Probleme->CoutsMarginauxDesContraintes = ProblemeLineaireEtenduPartieVariable->CoutsMarginauxDesContraintes;
Probleme->CoutsReduits                 = ProblemeLineaireEtenduPartieVariable->CoutsReduits;


# ifndef NDEBUG
  if ( PremierPassage == OUI ) Probleme->AffichageDesTraces = NON_SPX;
  else Probleme->AffichageDesTraces = OUI_SPX;
# else
  Probleme->AffichageDesTraces = NON_SPX;
# endif

Probleme->NombreDeContraintesCoupes = 0;


ProbSpx = SPX_Simplexe( Probleme , ProbSpx );

if ( ProbSpx != NULL ) {	
	ProblemeHydrauliqueEtendu->ProblemeSpx[NumeroDeProbleme] = (void *) ProbSpx;
}

ProblemeLineaireEtenduPartieVariable->ExistenceDUneSolution = Probleme->ExistenceDUneSolution;

if ( ProblemeLineaireEtenduPartieVariable->ExistenceDUneSolution != OUI_SPX && PremierPassage == OUI && ProbSpx != NULL ) {
  if ( ProblemeLineaireEtenduPartieVariable->ExistenceDUneSolution != SPX_ERREUR_INTERNE ) {
    
	  SPX_LibererProbleme( ProbSpx );
	  
	  ProbSpx = NULL;
	  PremierPassage = NON;
	  goto RESOLUTION;
	}
	else {		
    DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;		
	  return;
	}
}

if ( ProblemeLineaireEtenduPartieVariable->ExistenceDUneSolution == OUI_SPX ) {

	
	DonneesMensuelles->CoutSolution = 0.0;
	for ( Var = 0 ; Var < Probleme->NombreDeVariables ; Var++ )	
		DonneesMensuelles->CoutSolution += Probleme->CoutLineaire[Var] * Probleme->X[Var];
	

  DonneesMensuelles->ResultatsValides = OUI;
	
	for ( Var = 0 ; Var < ProblemeLineaireEtenduPartieFixe->NombreDeVariables ; Var++ ) {
		pt = ProblemeLineaireEtenduPartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
		if ( pt != NULL ) {
			*pt = ProblemeLineaireEtenduPartieVariable->X[Var];			
		}
	}
}
else {
  DonneesMensuelles->ResultatsValides = NON;		
	return ;
}

return;
}




char H2O2_J_EcrireJeuDeDonneesLineaireAuFormatMPS(DONNEES_MENSUELLES_ETENDUES * DonneesMensuelles, FILE * Flot )
{
int Cnt; int Var; int il; int ilk; int ilMax; char * Nombre;
int * Cder; int * Cdeb; int * NumeroDeContrainte; int * Csui;
PROBLEME_SIMPLEXE * Probleme;
  

int NombreDeVariables; int * TypeDeBorneDeLaVariable;
double * Xmax; double * Xmin; double * CoutLineaire; int NombreDeContraintes;
double * SecondMembre; char * Sens; int * IndicesDebutDeLigne;
int * NombreDeTermesDesLignes;	double * CoefficientsDeLaMatriceDesContraintes;
int * IndicesColonnes;


Probleme = (PROBLEME_SIMPLEXE *) (DonneesMensuelles->ProblemeHydrauliqueEtendu)->Probleme;

NombreDeVariables       = Probleme->NombreDeVariables;
TypeDeBorneDeLaVariable = Probleme->TypeDeVariable;
Xmax                    = Probleme->Xmax;
Xmin                    = Probleme->Xmin;
CoutLineaire            = Probleme->CoutLineaire;
NombreDeContraintes                   = Probleme->NombreDeContraintes;
SecondMembre                          = Probleme->SecondMembre;
Sens                                  = Probleme->Sens;
IndicesDebutDeLigne                   = Probleme->IndicesDebutDeLigne;
NombreDeTermesDesLignes               = Probleme->NombreDeTermesDesLignes;
CoefficientsDeLaMatriceDesContraintes = Probleme->CoefficientsDeLaMatriceDesContraintes;
IndicesColonnes                       = Probleme->IndicesColonnes;


for ( ilMax = -1 , Cnt = 0 ; Cnt < NombreDeContraintes; Cnt++ ) {
  if ( ( IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1 ) > ilMax ) {
    ilMax = IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1;
  }
}
ilMax+= NombreDeContraintes; 

Cder               = (int *) malloc( NombreDeVariables * sizeof( int ) );
Cdeb               = (int *) malloc( NombreDeVariables * sizeof( int ) );
NumeroDeContrainte = (int *) malloc( ilMax             * sizeof( int ) );
Csui               = (int *) malloc( ilMax             * sizeof( int ) );
Nombre             = (char *) malloc( 1024 );
if ( Cder == NULL || Cdeb == NULL || NumeroDeContrainte == NULL || Csui == NULL || Nombre == NULL ) {
  return( 0 );
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) Cdeb[Var] = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = IndicesDebutDeLigne[Cnt];
  ilMax = il + NombreDeTermesDesLignes[Cnt];
  while ( il < ilMax ) {
    Var = IndicesColonnes[il];
    if ( Cdeb[Var] < 0 ) {
      Cdeb              [Var] = il;
      NumeroDeContrainte[il]  = Cnt;
      Csui              [il]  = -1;
      Cder              [Var] = il;
    }
    else {
      ilk                     = Cder[Var];
      Csui              [ilk] = il;
      NumeroDeContrainte[il]  = Cnt;
      Csui              [il]  = -1;
      Cder              [Var] = il;
    }
    il++;
  }
}
free( Cder );

















fprintf(Flot,"* Number of variables:   %d\n",NombreDeVariables);
fprintf(Flot,"* Number of constraints: %d\n",NombreDeContraintes);












fprintf(Flot,"NAME          Pb Solve\n");


fprintf(Flot,"ROWS\n");











fprintf(Flot," N  OBJECTIF\n");

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( Sens[Cnt] == '=' ) {
    fprintf(Flot," E  R%07d\n",Cnt);
  }
  else if (  Sens[Cnt] == '<' ) {
    fprintf(Flot," L  R%07d\n",Cnt);
  }
  else if (  Sens[Cnt] == '>' ) {
    fprintf(Flot," G  R%07d\n",Cnt);
  }
  else {
    fprintf(Flot,"PNE_EcrireJeuDeDonneesMPS : le sens de la contrainte %d: %c ne fait pas partie des sens reconnus\n",
            Cnt, Sens[Cnt]);
    fprintf(Flot,"Nombre de contraintes %d\n",NombreDeContraintes);
		return( 0 );
  } 
}


fprintf(Flot,"COLUMNS\n");
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( CoutLineaire[Var] != 0.0 ) {
    SNPRINTF(Nombre,1024,"%-.10lf",CoutLineaire[Var]);
    fprintf(Flot,"    C%07d  OBJECTIF  %s\n",Var,Nombre);
  }
  il = Cdeb[Var];
  while ( il >= 0 ) {
    SNPRINTF(Nombre,1024,"%-.10lf",CoefficientsDeLaMatriceDesContraintes[il]);
    fprintf(Flot,"    C%07d  R%07d  %s\n",Var,NumeroDeContrainte[il],Nombre);
    il = Csui[il];
  }
}


fprintf(Flot,"RHS\n");
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SecondMembre[Cnt] != 0.0 ) {
    SNPRINTF(Nombre,1024,"%-.9lf",SecondMembre[Cnt]);
    fprintf(Flot,"    RHSVAL    R%07d  %s\n",Cnt,Nombre);
  }
}


fprintf(Flot,"BOUNDS\n");












for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE ) {
    SNPRINTF(Nombre,1024,"%-.9lf",Xmin[Var]);
    fprintf(Flot," FX BNDVALUE  C%07d  %s\n",Var,Nombre);
    continue;
  }
  
  
  if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
    if ( Xmin[Var] != 0.0 ) {
      SNPRINTF(Nombre,1024,"%-.9lf",Xmin[Var]);
      fprintf(Flot," LO BNDVALUE  C%07d  %s\n",Var,Nombre);
    }
    SNPRINTF(Nombre,1024,"%-.9lf",Xmax[Var]);
    fprintf(Flot," UP BNDVALUE  C%07d  %s\n",Var,Nombre);
  }
  if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
    if ( Xmin[Var] != 0.0 ) {
      SNPRINTF(Nombre,1024,"%-.9lf",Xmin[Var]);
      fprintf(Flot," LO BNDVALUE  C%07d  %s\n",Var,Nombre);
    }
  }
  if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    fprintf(Flot," MI BNDVALUE  C%07d\n",Var);
    if ( Xmax[Var] != 0.0 ) {
      SNPRINTF(Nombre,1024,"%-.9lf",Xmax[Var]);
      fprintf(Flot," UP BNDVALUE  C%07d  %s\n",Var,Nombre);
    }
  }
  if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE ) {
    fprintf(Flot," FR BNDVALUE  C%07d\n",Var);
  }
}


fprintf(Flot,"ENDATA\n");

free ( Cdeb );
free ( NumeroDeContrainte );
free ( Csui );
free ( Nombre );

fclose( Flot );

return( 1 );
}
