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

   FONCTION: S'il y a des variables entieres on ajoute la contrainte de
	           cout max avec une variable. C'est une contrainte d'egalite
						 avec une variable non bornee et un second membre nul.
						 Lorsqu'opn dispose d'une solution admissible, la variable
						 non bornee devient une variable >= 0 et on met le cout
						 dans le scond membre.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif     

/*----------------------------------------------------------------------------*/

void PNE_AjouterLaContrainteDeCoutMax( PROBLEME_PNE * Pne )
{
int Var; int NombreDeVariables; int NombreDeContraintes; int * Mdeb; int * NbTerm;
int * Nuvar; double * A; int NbT; double * CoutLineaire; int Cnt; int il; int Nb;

/* Je ne recommande pas d'utiliser la contrainte de cout max */
# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == NON_PNE
  return;
# endif

if ( Pne->NombreDeContraintesTrav + 1 > Pne->NombreDeContraintesAllouees ) {
  PNE_AugmenterLeNombreDeContraintes( Pne );
}
if ( Pne->NombreDeVariablesTrav + 1 > Pne->NombreDeVariablesAllouees ) {
  PNE_AugmenterLeNombreDeVariables( Pne );
}

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
CoutLineaire = Pne->LTrav;

Cnt = NombreDeContraintes - 1;
il = Mdeb[Cnt] + NbTerm[Cnt] + 1;

NbT = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( CoutLineaire[Var] != 0.0 ) NbT++;
}

Nb = 0;
ControleTailleMatrice:
if ( il +  NbT + 1 > Pne->TailleAlloueePourLaMatriceDesContraintes ) {
  PNE_AugmenterLaTailleDeLaMatriceDesContraintes( Pne );
	Nuvar = Pne->NuvarTrav;
  A = Pne->ATrav;
	Nb++;
	if ( Nb < 100 ) goto ControleTailleMatrice;
	else {
	  /* Probleme de place: remarque dans ce cas il faudrait activer UtiliserCoutMax donc il faut
		   monter un flag: a faire */
	  return;
	}
}

Cnt++;
Mdeb[Cnt] = il;
NbT = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( CoutLineaire[Var] == 0.0 ) continue;
  Nuvar[il] = Var;
	A[il] = CoutLineaire[Var];
	NbT++;
	il++;
}
if ( NbT == 0 ) return;

/* Variable d'ecart de la contrainte de cout max */
Var = NombreDeVariables;
Pne->TypeDeVariableTrav[Var] = REEL;
Pne->TypeDeBorneTrav   [Var] = VARIABLE_NON_BORNEE;
Pne->VariableAInverser [Var] = NON_PNE;
Pne->UTrav   [Var] = 0.;
Pne->UminTrav[Var] = -LINFINI_PNE;
Pne->UmaxTrav[Var] = LINFINI_PNE;
Pne->LTrav   [Var] = 0.;

Pne->NumeroDeLaVariableDEcartPourCoutMax = Var;

Nuvar[il] = Var; /* <- important: coefficient 1 */
A[il] = 1.;
NbT++;
il++;

/*                */

NbTerm[Cnt] = NbT;
Pne->BTrav[Cnt] = 0.0;
Pne->SensContrainteTrav[Cnt] = '=';
Pne->CorrespondanceCntPneCntEntree[Cnt] = -1;

Pne->NumeroDeLaContrainteDeCoutMax = Cnt;

Pne->NombreDeVariablesTrav++;
Pne->NombreDeContraintesTrav++;

return;
}
