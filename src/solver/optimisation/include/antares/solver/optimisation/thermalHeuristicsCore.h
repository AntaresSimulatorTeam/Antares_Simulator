// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <functional>
#include <vector>

// Algorithmic cores of the thermal heuristics run between the two optimisations. They are shared
// by the legacy thermal clusters and by the GEMS components whose integer strategy is "heuristic".

/// Heuristic "fast": guide number of units at each time step, ceil(production / unit size)
void OPT_CalculerLaCourbeGuideDuNombreDeGroupes(int NombreDePasDeTemps,
                                                const std::function<double(int)>& production,
                                                double TailleUnitaireDUnGroupe,
                                                std::vector<int>& NbGrpCourbeGuide);

/// Heuristic "fast": number of units on, constant by blocks of MUTetMDT time steps, at least
/// equal to the guide. Returns false if no solution was found (NbGrpOpt is then meaningless).
bool OPT_CalculerLeNombreDeGroupesParBlocsDeMUTetMDT(int NombreDePasDeTemps,
                                                     int MUTetMDT,
                                                     std::vector<int>& NbGrpCourbeGuide,
                                                     std::vector<int>& NbGrpOpt);

/// Heuristic "accurate": adjusts the minimum number of units on to respect the minimum up and
/// down durations (auxiliary linear problem). NbMinOptDeGroupesEnMarche is updated in place when
/// the auxiliary problem has a solution.
void OPT_ResoudreLePbLineaireDuNombreMinDeGroupesEnMarche(
  std::vector<int>& NbMinOptDeGroupesEnMarche,
  const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique,
  int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique,
  int DureeMinimaleDArretDUnGroupeDuPalierThermique,
  int NombreDePasDeTempsProblemeHebdo);
