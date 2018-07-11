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
# define OUI_MPS  1
# define NON_MPS  0

# define REEL    1
# define ENTIER  2

# define NON_DEFINI 128

typedef struct {

int NentreesVar;
int NentreesCnt;

int NbVar;
int NbCnt;

int NbCntRange;

int CoeffHaschCodeContraintes;
int SeuilHaschCodeContraintes;
int CoeffHaschCodeVariables;
int SeuilHaschCodeVariables; 

int   *  Mdeb;
double *  A;      
int   *  Nuvar;  
int   *  Msui;   
int   *  Mder;
int   *  NbTerm;
double *  B;
char   *  SensDeLaContrainte;
double *  BRange; /* Pas nul si contrainte range */
double *  VariablesDualesDesContraintes;

char   ** LabelDeLaContrainte;
char   ** LabelDuSecondMembre;
char   *  LabelDeLObjectif;

char   ** LabelDeLaVariable; 

int   *  TypeDeVariable;  
int   *  TypeDeBorneDeLaVariable;  
double *  U;	     
double *  L; 	      
double *  Umin;	     
double *  Umax;	

int * FirstNomCnt;
int * NomCntSuivant;

int * FirstNomVar;
int * NomVarSuivant;

} PROBLEME_MPS;
