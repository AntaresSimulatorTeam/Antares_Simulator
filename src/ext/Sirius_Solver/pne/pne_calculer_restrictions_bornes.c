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

   FONCTION: On essaie de fixer des variables entieres par des tests
             d'amissibilite en essayant de resserer des bornes.
             
                
   AUTEUR: R. GONZALEZ     

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h" 
# include "pne_define.h"

# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define ZERO_COEFF 1.e-9

char PNE_AffinerUneBorne( PROBLEME_PNE * , int , char , int , double , double , char * , char * , double * , double * );

void PNE_MiseAJourMinMaxContraintes( PROBLEME_PNE * , int , char * , char * , double * , double * );
				   
void PNE_CalculerLeMinEtLeMaxDUneContrainte( PROBLEME_PNE * , int , char * , char * , double * , double * );

char PNE_AffinerUneBorneSurCoupe( PROBLEME_PNE * , int , char , int , double , double  , char * , double * );             

void PNE_MiseAJourMinMaxCoupes( PROBLEME_PNE * , int , char * , char * , double * , double * );

void PNE_CalculerLeMinEtLeMaxDUneCoupe( PROBLEME_PNE * , int , char * , char * , double * , double * );
			   
# define BORNE_MODIFIEE  1
# define PAS_DE_SOLUTION 2
# define RIEN_A_SIGNALER 3

# define NOMBRE_MAX_DE_CYCLES 10

# define EPSILON_BORNES 1.e-5
			   
/*----------------------------------------------------------------------------*/

void PNE_CalculerLesRestrictionsDeBornes( PROBLEME_PNE * Pne,
                                          int * NombreDeVariablesEntieresFixees,
                                          char * InfaisabiliteDetectee,
					                                char   Mode ) 
{
int Var;int i ; int Cnt; int VariableTestee; int il; int ilMax; int NbVarEntieresFixees;
double B; char Sens; BB * Bb;

/* Test */
char OnFixe; int ic; int Var1;
/* Fin test */

char CodeModifBornes; char UneBorneAEteModifiee; int NombreDeCycles;

double * UmaxSv; double * UminSv; int * TypeDeBorneSv;

double * MinContrainte; char * MinContrainteDisponible;
double * MaxContrainte; char * MaxContrainteDisponible;

double * MinCoupe; char * MinCoupeDisponible;
double * MaxCoupe; char * MaxCoupeDisponible;

return;

Bb = (BB *) Pne->ProblemeBbDuSolveur;

/* Test */
goto AAA;
printf("################# Fixation sur critere ############################\n");
i = 0;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( Pne->LTrav[Var] != 0.0 ) continue;
  if ( Pne->UminTrav[Var] == Pne->UmaxTrav[Var] ) continue;
  OnFixe = OUI_PNE;
  ic = Pne->CdebTrav[Var];
  while ( ic >= 0 ) {
    Cnt = Pne->NumContrainteTrav[ic];
    if ( Pne->NbTermTrav[Cnt] != 2 ) goto NextIc;
    il    = Pne->MdebTrav[Cnt];	    
    ilMax = il + Pne->NbTermTrav[Cnt];
    while ( il < ilMax ) {
      Var1 = Pne->NuvarTrav[il];
      if ( Var1 != Var ) {
        if ( Pne->UmaxTrav[Var1] != 0.0 ) {
	  OnFixe = NON_PNE;
	  goto NextVar;
	}
      }
      il++;
    }
    NextIc:
    ic = Pne->CsuiTrav[ic];   
  }
  NextVar:
  if ( OnFixe == OUI_PNE ) {
    /* On peut fixer */
    Pne->UminTrav[Var] = 0.0;
    Pne->UmaxTrav[Var] = 0.0;
    printf("On peut fixer la variable %d sur critere\n",Var);
    i++;    
  }
}
  
printf("    Nombre de variables fixables sur critere: %d\n",i);
printf("################# Fin fixation sur critere ############################\n");
return;

AAA:
/*********************************************************************************/

UminSv          = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) ); 
UmaxSv          = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
TypeDeBorneSv   = (int *)   malloc( Pne->NombreDeVariablesTrav * sizeof( int   ) );

memcpy( (char *) UminSv , (char *) Pne->UminTrav  , Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) UmaxSv , (char *) Pne->UmaxTrav  , Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) TypeDeBorneSv , (char *) Pne->TypeDeBorneTrav , Pne->NombreDeVariablesTrav * sizeof( int ) );

MinContrainte           = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) ); 
MaxContrainte           = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) ); 
MinContrainteDisponible = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) ); 
MaxContrainteDisponible = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) );

MinCoupe           = (double *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( double ) ); 
MaxCoupe           = (double *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( double ) ); 
MinCoupeDisponible = (char *)   malloc( Pne->Coupes.NombreDeContraintes * sizeof( char   ) ); 
MaxCoupeDisponible = (char *)   malloc( Pne->Coupes.NombreDeContraintes * sizeof( char   ) );

/* On calcule le min et le max de chaque contrainte */
for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  PNE_CalculerLeMinEtLeMaxDUneContrainte( Pne, Cnt, MinContrainteDisponible, MaxContrainteDisponible,
                                          MinContrainte, MaxContrainte );
}

/* On calcule le min et le max de chaque coupe (on n'a besoin que du min) */
for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) {
  PNE_CalculerLeMinEtLeMaxDUneCoupe( Pne, Cnt, MinCoupeDisponible, MaxCoupeDisponible,
                                     MinCoupe, MaxCoupe );
}

NombreDeCycles         = 0;
NbVarEntieresFixees    = 0;
*InfaisabiliteDetectee = NON_PNE;

DebutAffinageDesBornes:

UneBorneAEteModifiee = NON_PNE;

/* On balaye les contraintes et on lance les calculs de bornes */
for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  B     = Pne->BTrav[Cnt];
  il    = Pne->MdebTrav[Cnt];	    
  ilMax = il + Pne->NbTermTrav[Cnt];
  while ( il < ilMax ) {
    VariableTestee = Pne->NuvarTrav[il];
    if ( Pne->UmaxTrav[VariableTestee] - Pne->UminTrav[VariableTestee] > ZERO_VARFIXE && 
         Pne->TypeDeBorneTrav[VariableTestee] != VARIABLE_FIXE && Pne->ATrav[il] != 0.0 ) {	       
      Sens = '<';
      CodeModifBornes = PNE_AffinerUneBorne( Pne, VariableTestee, Sens, Cnt    ,
		                                         B             , Pne->ATrav[il],
			                                       MinContrainteDisponible      ,
                                             MaxContrainteDisponible      ,
                                             MinContrainte                ,
                                             MaxContrainte );              
      if ( CodeModifBornes == BORNE_MODIFIEE ) {
        UneBorneAEteModifiee = OUI_PNE;
	      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) NbVarEntieresFixees++;
          /* Recalcul des nouvelles valeurs de min max */
        PNE_MiseAJourMinMaxContraintes( Pne, VariableTestee, MinContrainteDisponible, MaxContrainteDisponible,
                                        MinContrainte, MaxContrainte );
        PNE_MiseAJourMinMaxCoupes( Pne, VariableTestee, MinCoupeDisponible, MaxCoupeDisponible,
                                   MinCoupe, MaxCoupe );					
      }
      if ( CodeModifBornes == PAS_DE_SOLUTION ) {
        printf(" -----------> pas de solution \n");
        *InfaisabiliteDetectee = OUI_PNE;
	      goto Fin;
      }
      if ( Pne->SensContrainteTrav[Cnt] == '=' ) {
        /* Si la contrainte est de type '=' il est possible de faire le test des bornes dans les 2 sens */	      
        Sens = '>';
        CodeModifBornes = PNE_AffinerUneBorne( Pne, VariableTestee, Sens         , Cnt,
		                               B             , Pne->ATrav[il],
			                       MinContrainteDisponible      ,
                                               MaxContrainteDisponible      ,
                                               MinContrainte                ,
                                               MaxContrainte );                 
        if ( CodeModifBornes == BORNE_MODIFIEE ) {
          UneBorneAEteModifiee = OUI_PNE;
	  if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) NbVarEntieresFixees++;
          /* Recalcul des nouvelles valeurs de min max */
          PNE_MiseAJourMinMaxContraintes( Pne, VariableTestee, MinContrainteDisponible, MaxContrainteDisponible,
                                          MinContrainte, MaxContrainte );
          PNE_MiseAJourMinMaxCoupes( Pne, VariableTestee, MinCoupeDisponible, MaxCoupeDisponible,
                                     MinCoupe, MaxCoupe );					  
	}
	if ( CodeModifBornes == PAS_DE_SOLUTION ) {
          /* A completer */
	  printf(" -----------> pas de solution \n");	  
          *InfaisabiliteDetectee = OUI_PNE;
	  goto Fin;
 	}
      }
    }
    il++;
  }    
}

/* On etudie maintenant les coupes */
for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) {


/*
continue;
*/


  B     = Pne->Coupes.B[Cnt];
  il    = Pne->Coupes.Mdeb[Cnt];	    
  ilMax = il + Pne->Coupes.NbTerm[Cnt];
  while ( il < ilMax ) {    
    VariableTestee = Pne->Coupes.Nuvar[il];       
    if ( VariableTestee >= Pne->NombreDeVariablesTrav ) exit(0);
    if ( VariableTestee < 0 ) exit(0);
    
    if ( Pne->UmaxTrav[VariableTestee] - Pne->UminTrav[VariableTestee] > ZERO_VARFIXE && 
         Pne->TypeDeBorneTrav[VariableTestee] != VARIABLE_FIXE && Pne->Coupes.A[il] != 0.0 ) {	       
      Sens = '<';
      CodeModifBornes = PNE_AffinerUneBorneSurCoupe( Pne, VariableTestee, Sens, Cnt   ,
		                                              B             , Pne->Coupes.A[il],
			                                            MinCoupeDisponible      ,
                                                  MinCoupe );              
      if ( CodeModifBornes == BORNE_MODIFIEE ) {
        UneBorneAEteModifiee = OUI_PNE;
	if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) NbVarEntieresFixees++;
        /* Recalcul des nouvelles valeurs de min max */
        PNE_MiseAJourMinMaxContraintes( Pne, VariableTestee, MinContrainteDisponible, MaxContrainteDisponible,
                                        MinContrainte, MaxContrainte );
        PNE_MiseAJourMinMaxCoupes( Pne, VariableTestee, MinCoupeDisponible, MaxCoupeDisponible,
                                   MinCoupe, MaxCoupe );				
      }
      if ( CodeModifBornes == PAS_DE_SOLUTION ) {
        printf(" -----------> pas de solution \n");
        *InfaisabiliteDetectee = OUI_PNE;
	goto Fin;
      }
    }
    il++;
  }    
}

NombreDeCycles++;
if ( NombreDeCycles < NOMBRE_MAX_DE_CYCLES ) {
  if ( UneBorneAEteModifiee == OUI_PNE ) {
    /* printf("On Relance un affinage de bornes Cycle %d\n",NombreDeCycles); */
    goto DebutAffinageDesBornes;
  }
}

Fin:

free( MinContrainte );
free( MaxContrainte );
free( MinContrainteDisponible );
free( MaxContrainteDisponible );

free( MinCoupe );
free( MaxCoupe );
free( MinCoupeDisponible );
free( MaxCoupeDisponible );

/* Car la validite des coupes dans tout l'arbre n'est pas assuree dans la cas de variations de bornes
   autres que pour les variables entieres */
if ( Mode != SIMULATION ) {
  for( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
    if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) continue;
    if ( Bb->NoeudEnExamen->ProfondeurDuNoeud <= 30 /*SEUIL_PROFONDEUR_LIMITE_1*/ /* a revoir car dans la partie branch and bound */ ||
         Bb->NoeudEnExamen->ProfondeurDuNoeud <= 30 /*SEUIL_PROFONDEUR_LIMITE_2*/ || 1 ) {
      Pne->UminTrav       [Var] = UminSv[Var];
      Pne->UmaxTrav       [Var] = UmaxSv[Var];
      Pne->TypeDeBorneTrav[Var] = TypeDeBorneSv[Var];
    }    
  }
}
else {
  /* Mode simulation => on remet tout comme avant */
  for( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
    Pne->UminTrav       [Var] = UminSv[Var];
    Pne->UmaxTrav       [Var] = UmaxSv[Var];
    Pne->TypeDeBorneTrav[Var] = TypeDeBorneSv[Var];  
  }
}

free( UminSv );
free( UmaxSv );
free( TypeDeBorneSv );

*NombreDeVariablesEntieresFixees = NbVarEntieresFixees;

return;
}

/*----------------------------------------------------------------------------*/
/* On y modifie: Pne->UminTrav, Pne->UmaxTrav, Pne->TypeDeBorneTrav */

char PNE_AffinerUneBorne( PROBLEME_PNE * Pne,
                          int VariableTestee,
                          char SensDeLaContrainte, int Cnt,
		          double B,
			  double A,
			  char * MinContrainteDisponible,
                          char * MaxContrainteDisponible,
                          double * MinContrainte,
                          double * MaxContrainte           
		      	   )
{
double BMX; double BMN; double X; char CodeRetour;
double ValeurMinDeLaSomme; double ValeurMaxDeLaSomme;

CodeRetour = RIEN_A_SIGNALER;

if ( SensDeLaContrainte == '<' ) {
  /* On cherche le max du second membre. Pour cela on lui retranche le min de la somme */
  if ( MinContrainteDisponible[Cnt] == NON_PNE ) return( CodeRetour );
  if ( A >= 0.0 ) {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
      ValeurMinDeLaSomme = MinContrainte[Cnt] - A * Pne->UminTrav[VariableTestee];
    }
    else return( CodeRetour );  
  }
  else {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      ValeurMinDeLaSomme = MinContrainte[Cnt] - A * Pne->UmaxTrav[VariableTestee];
    }
    else return( CodeRetour );
  } 
  BMX = B - ValeurMinDeLaSomme;
  if ( A > ZERO_COEFF ) {
    X = BMX / A;
    if ( Pne->UmaxTrav[VariableTestee] > X + EPSILON_BORNES ) {          
      /* On peut abaisser la borne sup */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) {
        /*
	printf("->>>>>>>>>>> Variable entiere %d X %e floor %e\n",VariableTestee,X,floor(X));
	*/
        X = floor( X );
      }
      /* Precaution */
      if ( X < Pne->UminTrav[VariableTestee] ) {
        /* La nouvelle borne sup doit etre inferieure a la borne inf => pas de solution */
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );	
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        /* A revoir */
        return( CodeRetour );	
      }      
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
      /*
      printf("   ---> affinage de borne SUP sur variable %d: Old %e New %e type de borne %d Cnt %d B %e\n",VariableTestee,
                      Pne->UmaxTrav[VariableTestee],X,Pne->TypeDeBorneTrav[VariableTestee],Cnt,B); 
      */     
      Pne->UmaxTrav[VariableTestee] = X;      
      CodeRetour = BORNE_MODIFIEE;				      
    }
  }
  else if ( A < -ZERO_COEFF ) {
    X = BMX / A;		  
    if ( Pne->UminTrav[VariableTestee] < X - EPSILON_BORNES ) {
      /* On peut remonter la borne inf */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) {
        /*
	printf("->>>>>>>>>>> Variable entiere %d X %e ceil %e\n",VariableTestee,X,ceil(X));
	*/
        X = ceil( X );
      }
      /* Precaution */
      if ( X > Pne->UmaxTrav[VariableTestee] ) {
        /* La nouvelle borne inf doit etre superieure a la borne sup => pas de solution */
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );	
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_INFERIEUREMENT;
      }
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
      /*
      printf("   ---> affinage de borne INF sur variable %d: Old %e New %e type de borne %d Cnt %d B %e\n",VariableTestee,
                      Pne->UminTrav[VariableTestee],X,Pne->TypeDeBorneTrav[VariableTestee],Cnt,B); 
      */
      
      Pne->UminTrav[VariableTestee] = X;               
      CodeRetour = BORNE_MODIFIEE;
    }       		  
  }
  return( CodeRetour );
}

if ( SensDeLaContrainte == '>') {
  /* On cherche le min du second membre. Pour cela on lui retranche le max de la somme */
  if ( MaxContrainteDisponible[Cnt] == NON_PNE ) return( CodeRetour );
  if ( A >= 0.0 ) {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      ValeurMaxDeLaSomme = MaxContrainte[Cnt] - A * Pne->UmaxTrav[VariableTestee];
    }
    else return( CodeRetour );  
  }
  else {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
      ValeurMaxDeLaSomme = MaxContrainte[Cnt] - A * Pne->UminTrav[VariableTestee];
    }
    else return( CodeRetour );
  }   
  BMN = B - ValeurMaxDeLaSomme;
  if ( A > ZERO_COEFF ) {
    X = BMN / A;		  
    if ( Pne->UminTrav[VariableTestee] < X - EPSILON_BORNES ) {
      /* On peut remonter la borne inf */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) X = ceil( X );
      /* Precaution */
      if ( X > Pne->UmaxTrav[VariableTestee] ) {
        /* La nouvelle borne inf doit etre superieure a la borne sup => pas de solution */
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );		
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_INFERIEUREMENT;
      }
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
      /*
      printf("   ---> affinage de borne INF sur variable %d: Old %e New %e type de borne %d Cnt %d B %e\n",VariableTestee,
                      Pne->UminTrav[VariableTestee],X,Pne->TypeDeBorneTrav[VariableTestee],Cnt,B);
      */
      Pne->UminTrav[VariableTestee] = X;      
      CodeRetour = BORNE_MODIFIEE;      
    }
  }
  else if ( A < -ZERO_COEFF ) {
    X = BMN / A;		  
    if ( Pne->UmaxTrav[VariableTestee] > X + EPSILON_BORNES ) {
      /* On peut abaisser la borne sup */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) X = floor( X );
      /* Precaution */
      if ( X < Pne->UminTrav[VariableTestee] ) {
        /* La nouvelle borne sup doit etre inferieure a la borne inf => pas de solution */           
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );		
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        /* A ce stade on ne peut plus utiliser le type de borne VARIABLE_BORNEE_SUPERIEUREMENT sans trop de complication */
        /*Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_SUPERIEUREMENT;*/
	return( CodeRetour );
      }
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
      /*
      printf("   ---> affinage de borne SUP sur variable %d: Old %e New %e type de borne %d Cnt %d B %e\n",VariableTestee,
                      Pne->UmaxTrav[VariableTestee],X,Pne->TypeDeBorneTrav[VariableTestee],Cnt,B);
      */
      Pne->UmaxTrav[VariableTestee] = X;     
      CodeRetour = BORNE_MODIFIEE;     
    }
  }
  return( CodeRetour );
}

return( CodeRetour );
}

/*----------------------------------------------------------------------------*/
/* On y modifie: Pne->UminTrav, Pne->UmaxTrav, Pne->TypeDeBorneTrav */		      	   
char PNE_AffinerUneBorneSurCoupe( PROBLEME_PNE * Pne,
                                  int VariableTestee, char SensDeLaContrainte, int Cnt,
                                  double B           , double A ,
			                            char * MinCoupeDisponible,
                                  double * MinCoupe )              
{
double BMX; double X; char CodeRetour; double ValeurMinDeLaSomme; 

CodeRetour = RIEN_A_SIGNALER;

if ( SensDeLaContrainte == '<' ) {
  /* On cherche le max du second membre. Pour cela on lui retranche le min de la somme */
  if ( MinCoupeDisponible[Cnt] == NON_PNE ) return( CodeRetour );
  if ( A >= 0.0 ) {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
      ValeurMinDeLaSomme = MinCoupe[Cnt] - A * Pne->UminTrav[VariableTestee];
    }
    else return( CodeRetour );  
  }
  else {
    if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      ValeurMinDeLaSomme = MinCoupe[Cnt] - A * Pne->UmaxTrav[VariableTestee];
    }
    else return( CodeRetour );
  } 
  BMX = B - ValeurMinDeLaSomme;
  if ( A > ZERO_COEFF ) {
    X = BMX / A;
    if ( Pne->UmaxTrav[VariableTestee] > X + EPSILON_BORNES ) {          
      /* On peut abaisser la borne sup */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) {
        X = floor( X );
      }
      /* Precaution */
      if ( X < Pne->UminTrav[VariableTestee] ) {
        /* La nouvelle borne sup doit etre inferieure a la borne inf => pas de solution */
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );	
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        /* A revoir */
        return( CodeRetour );	
      }      
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_INFERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
      Pne->UmaxTrav[VariableTestee] = X;      
      CodeRetour = BORNE_MODIFIEE;      
    }
  }
  else if ( A < -ZERO_COEFF ) {
    X = BMX / A;		  
    if ( Pne->UminTrav[VariableTestee] < X - EPSILON_BORNES ) {
      /* On peut remonter la borne inf */
      if ( Pne->TypeDeVariableTrav[VariableTestee] == ENTIER ) {
        X = ceil( X );
      }
      /* Precaution */
      if ( X > Pne->UmaxTrav[VariableTestee] ) {
        /* La nouvelle borne inf doit etre superieure a la borne sup => pas de solution */
	printf("Pas de solution\n");
	CodeRetour = PAS_DE_SOLUTION;	
        return( CodeRetour );	
      }
      if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_NON_BORNEE ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_INFERIEUREMENT;
      }
      else if ( Pne->TypeDeBorneTrav[VariableTestee] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        Pne->TypeDeBorneTrav[VariableTestee] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }      
      Pne->UminTrav[VariableTestee] = X;               
      CodeRetour = BORNE_MODIFIEE;	      
    }       		  
  }
  return( CodeRetour );
}

return( CodeRetour );
}

/*----------------------------------------------------------------------------*/

void PNE_MiseAJourMinMaxContraintes( PROBLEME_PNE * Pne, int Var,
			             char * MinContrainteDisponible,
                                     char * MaxContrainteDisponible,
                                     double * MinContrainte,
                                     double * MaxContrainte
				   )
{
int ic; int Cnt; 
ic = Pne->CdebTrav[Var];
while ( ic >= 0 ) {
  Cnt = Pne->NumContrainteTrav[ic];
  PNE_CalculerLeMinEtLeMaxDUneContrainte( Pne, Cnt, MinContrainteDisponible, MaxContrainteDisponible,
                                          MinContrainte, MaxContrainte );     
  ic = Pne->CsuiTrav[ic];
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_MiseAJourMinMaxCoupes( PROBLEME_PNE * Pne, int Var,
			        char * MinCoupeDisponible,
                                char * MaxCoupeDisponible,
                                double * MinCoupe,
                                double * MaxCoupe
			      )
{
int il; int ilMax; int Cnt; 

for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) {
  il    = Pne->Coupes.Mdeb[Cnt];	    
  ilMax = il + Pne->Coupes.NbTerm[Cnt];
  while ( il < ilMax ) {
    if ( Pne->Coupes.Nuvar[il] == Var ) {
      PNE_CalculerLeMinEtLeMaxDUneCoupe( Pne, Cnt, MinCoupeDisponible, MaxCoupeDisponible,
                                         MinCoupe, MaxCoupe );
      break;
    }
    il++;
  }
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_CalculerLeMinEtLeMaxDUneContrainte( PROBLEME_PNE * Pne, int Cnt,
			                     char * MinContrainteDisponible,
                                             char * MaxContrainteDisponible,
                                             double * MinContrainte,
                                             double * MaxContrainte           
		      	                   )
{
double X; int il; int ilMax; int Var;

MinContrainteDisponible[Cnt] = OUI_PNE;
MaxContrainteDisponible[Cnt] = OUI_PNE;
MinContrainte[Cnt] = 0.0;
MaxContrainte[Cnt] = 0.0;
il    = Pne->MdebTrav[Cnt];	    
ilMax = il + Pne->NbTermTrav[Cnt];
while ( il < ilMax ) {
  if ( MinContrainteDisponible[Cnt] == NON_PNE && MaxContrainteDisponible[Cnt] == NON_PNE ) break;
  Var = Pne->NuvarTrav[il];
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {
    MinContrainteDisponible[Cnt] = NON_PNE;
    MaxContrainteDisponible[Cnt] = NON_PNE;
    break;      
  }
  X = Pne->ATrav[il];
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
    MaxContrainte[Cnt]+= X * Pne->UTrav[Var];
    MinContrainte[Cnt]+= X * Pne->UTrav[Var];    
  }
  else {
    if ( X >= 0.0 ) {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        MaxContrainte[Cnt]+= X * Pne->UmaxTrav[Var];
      }
      else MaxContrainteDisponible[Cnt] = NON_PNE;
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {    
        MinContrainte[Cnt]+= X * Pne->UminTrav[Var];
      }
      else MinContrainteDisponible[Cnt] = NON_PNE;
    }
    else {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
        MaxContrainte[Cnt]+= X * Pne->UminTrav[Var];
      }
      else MaxContrainteDisponible[Cnt] = NON_PNE;
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {    
        MinContrainte[Cnt]+= X * Pne->UmaxTrav[Var];
      }
      else MinContrainteDisponible[Cnt] = NON_PNE;
    }     
  }
  il++;
}

return;

}

/*----------------------------------------------------------------------------*/

void PNE_CalculerLeMinEtLeMaxDUneCoupe( PROBLEME_PNE * Pne, int Cnt,
			                char * MinCoupeDisponible,
                                        char * MaxCoupeDisponible,
                                        double * MinCoupe,
                                        double * MaxCoupe           
		      	              )
{
double X; int il; int ilMax; int Var;

MinCoupeDisponible[Cnt] = OUI_PNE;
MaxCoupeDisponible[Cnt] = OUI_PNE;
MinCoupe[Cnt] = 0.0;
MaxCoupe[Cnt] = 0.0;
il    = Pne->Coupes.Mdeb[Cnt];	    
ilMax = il + Pne->Coupes.NbTerm[Cnt];
while ( il < ilMax ) {
  if ( MinCoupeDisponible[Cnt] == NON_PNE && MaxCoupeDisponible[Cnt] == NON_PNE ) break;
  Var = Pne->Coupes.Nuvar[il];
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {
    MinCoupeDisponible[Cnt] = NON_PNE;
    MaxCoupeDisponible[Cnt] = NON_PNE;
    break;      
  }
  X = Pne->Coupes.A[il];
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
    MaxCoupe[Cnt]+= X * Pne->UTrav[Var];
    MinCoupe[Cnt]+= X * Pne->UTrav[Var];    
  }
  else {
    if ( X >= 0.0 ) {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        MaxCoupe[Cnt]+= X * Pne->UmaxTrav[Var];
      }
      else MaxCoupeDisponible[Cnt] = NON_PNE;
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
         Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {    
        MinCoupe[Cnt]+= X * Pne->UminTrav[Var];
      }
      else MinCoupeDisponible[Cnt] = NON_PNE;
    }
    else {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
        MaxCoupe[Cnt]+= X * Pne->UminTrav[Var];
      }
      else MaxCoupeDisponible[Cnt] = NON_PNE;
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
           Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {    
        MinCoupe[Cnt]+= X * Pne->UmaxTrav[Var];
      }
      else MinCoupeDisponible[Cnt] = NON_PNE;
    }     
  }
  il++;
}

return;

}
