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
#ifndef __MEMSTATS_H__
# define __MEMSTATS_H__

# include <stddef.h> /* for size_t */

 


/*!
** \brief Structure contenant des statistiques sur la memoire a un instant donne
*/
typedef struct
{
	/*! Quantite de Memoire totale de la machine (en octets) */
	size_t total;
	/*! Quantite de Memoire disponible (en octets) */
	size_t libre;

} MemoireInfo;




# ifdef __cplusplus
extern "C"
{
# endif


/*!
** \brief Recupere toutes les informations sur la memoire
**
** Dans tous les cas, la structure donnee est initialisee (avec des
** valeurs par defaut en cas d'erreur).
**
** \param[out] info Structure ou seront stockees les informations
** recoltees
** \return 0 en cas de reussite, une valeur quelconque en cas d'erreur
*/
int MemoireStatistiques(MemoireInfo* info);



/*!
** \brief Donne la quantite de memoire actuellement disponible
** \return Une taille en octets
*/
size_t MemoireDisponible( void );

/*!
** \brief Donne la quantite de Memoire totale de la machine
** \return Une taille en octets
*/
size_t MemoireTotale( void );


# ifdef __cplusplus
}
# endif

#endif /* __MEMSTATS_H__ */
