/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "../simulation/sim_constants.h"

constexpr char PREMIERE_OPTIMISATION = 1;
constexpr char DEUXIEME_OPTIMISATION = 2;

constexpr char CONTRAINTE_HORAIRE = 1;
constexpr char CONTRAINTE_JOURNALIERE = 2;
constexpr char CONTRAINTE_HEBDOMADAIRE = 3;

constexpr char PAS_DE_LISSAGE_HYDRAULIQUE = 0;
constexpr char LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS = 1;
constexpr char LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX = 2;

constexpr char ALTRUISTE = 1;
constexpr char EGOISTE = 2;

constexpr char ANTARES_SIMPLEXE = 1;
constexpr char ANTARES_PNE = 2;
