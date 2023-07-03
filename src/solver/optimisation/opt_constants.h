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
