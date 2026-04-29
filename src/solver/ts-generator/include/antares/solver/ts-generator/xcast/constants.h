// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__

//! Precision sur la factorisation des matrices
#define EPSIMIN ((float)1.0e-9)
//! Precision sur les verifications des produits LtL
#define EPSIMAX ((float)1.0e-3)

//! Limite d'approche des bornes des processus
#define EPSIBOR ((float)1.0e-4)

//! Infini +
#define INFINIP ((float)1.0e+9)
//! Infini -
#define INFININ ((float)-1.0e+9)

// 1+ PETIT * theta
// 1 dans le developpement limite des correlations spatiales instantanees
// PETIT * theta << 1  pour le plafonnement du terme lineaire  des dXt
// 2 * sqrt(PETIT) * theta << 1  pour le plafonnement du terme diffusion des dXt
#define PETIT ((float)1e-2)

//! A la limite superieure des lois de Weibull  la fonction de repartition vaut 1- NEGLI_WEIB
#define NEGLI_WEIB ((double)1.0e-4)
//! A la limite superieure des lois Gamma  la fonction de repartition vaut 1- NEGLI_GAM
#define NEGLI_GAMM ((double)1.0e-4)

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_CONSTANTS_H__
