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
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__
# define __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__


//! Precision sur la factorisation des matrices
# define	 EPSIMIN   ((float) 1.0e-9)
//! Precision sur les verifications des produits LtL
# define	 EPSIMAX   ((float) 1.0e-3)


//! Limite d'approche des bornes des processus
# define  EPSIBOR  ((float) 1.0e-4)


//! Infini +
# define	 INFINIP  ((float) 1.0e+9)
//! Infini -
# define	 INFININ  ((float) -1.0e+9)



// 1+ PETIT * theta
// 1 dans le developpement limite des correlations spatiales instantanees
// PETIT * theta << 1  pour le plafonnement du terme lineaire  des dXt
// 2 * sqrt(PETIT) * theta << 1  pour le plafonnement du terme diffusion des dXt
# define	 PETIT  ((float) 1e-2)


//! A la limite superieure des lois de Weibull  la fonction de repartition vaut 1- NEGLI_WEIB
# define	 NEGLI_WEIB	  ((double) 1.0e-4)
//! A la limite superieure des lois Gamma  la fonction de repartition vaut 1- NEGLI_GAM
# define	 NEGLI_GAMM	  ((double) 1.0e-4)




#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__
