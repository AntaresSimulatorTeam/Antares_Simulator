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
# ifdef __cplusplus
  extern "C"
	{
# endif
# ifndef BB_MACROS_POUR_FONCTION_EXTERNES_DE_GESTION_MEMOIRE
# include "mem_fonctions.h"
/*****************************************************************

 
  Macros pour redefinir les primitives de gestion memoire lorsqu'on
  ne veut pas utiliser celles de lib de l'OS

	
*****************************************************************/
	
# define malloc(Taille)           MEM_Malloc(Bb->Tas,Taille)
# define free(Pointeur)           MEM_Free(Pointeur) 
# define realloc(Pointeur,Taille) MEM_Realloc(Bb->Tas,Pointeur,Taille)

/*****************************************************************/
# define BB_MACROS_POUR_FONCTION_EXTERNES_DE_GESTION_MEMOIRE
# endif
# ifdef __cplusplus
  }
# endif
