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
# ifndef CONSTANTES_EXTERNES_LU_DEJA_DEFINIES  
/*******************************************************************************************/
/* 
  Definition des constantes symboliques a utiliser par le module appelant la 
  factorisation lu 
*/ 
			 									        
# define OUI_LU      1  	             				     
# define NON_LU      0  	 	         
# define SATURATION_MEMOIRE     2        	      
# define MATRICE_SINGULIERE     3
# define PRECISION_DE_RESOLUTION_NON_ATTEINTE  4  /* Quand on fait du raffinement iteratif, si on atteint
                                                     pas la precision demandee */
  
/* Les contextes d'utilisation de la factorisation */
# define LU_SIMPLEXE         1
# define LU_POINT_INTERIEUR  2
# define LU_GENERAL          3

/*******************************************************************************************/
# define CONSTANTES_EXTERNES_LU_DEJA_DEFINIES	
# endif 

 


