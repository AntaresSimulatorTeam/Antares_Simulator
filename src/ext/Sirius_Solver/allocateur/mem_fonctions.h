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
# ifdef __CPLUSPLUS
  extern "C"
	{
# endif
# ifndef FONCTION_EXTERNES_MEMOIRE_DEJA_DEFINIES
/*****************************************************************/

void * MEM_Init( void );
void   MEM_Quit( void * );
char * MEM_Malloc( void * , size_t );
void   MEM_Free( void * ); 
char * MEM_Realloc( void * , void * , size_t );
long   MEM_QuantiteLibre( BLOCS_LIBRES * );

/*****************************************************************/
# define FONCTION_EXTERNES_MEMOIRE_DEJA_DEFINIES
# endif
# ifdef __CPLUSPLUS
  }
# endif 
