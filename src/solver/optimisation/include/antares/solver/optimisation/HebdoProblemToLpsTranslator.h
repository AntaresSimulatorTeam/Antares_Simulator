
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver
{

/**
 * @class WeeklyProblemTranslationException
 * @brief Exception class for errors during the translation of a weekly problem.
 *
 * This class is a custom exception class that is thrown when an error occurs during the translation
 * of a weekly problem.
 */
class WeeklyProblemTranslationException: public std::runtime_error
{
public:
    explicit WeeklyProblemTranslationException(const std::string& string) noexcept;
};

/**
 * @class HebdoProblemToLpsTranslator
 * @brief Class for translating a weekly problem to a linear programming problem.
 *
 * This class is responsible for translating a weekly problem to a linear programming problem.
 */
class HebdoProblemToLpsTranslator
{
public:
    /**
     * @brief Translates a weekly problem to a linear programming problem.
     *
     * This function takes a pointer to a PROBLEME_ANTARES_A_RESOUDRE object and a string_view
     * representing the name of the problem. It translates the weekly problem to a linear
     * programming problem and returns a WeeklyDataFromAntaresPtr to the translated problem. Datas
     * from the PROBLEME_ANTARES_A_RESOUDRE are copied to the WeeklyDataFromAntaresPtr.
     *
     * @param problem A pointer to the weekly problem to be translated.
     * @param name The name of the problem.
     * @return WeeklyDataFromAntaresPtr A WeeklyDataFromAntaresPtr to the translated problem.
     */
    [[nodiscard]] WeeklyDataFromAntares translate(const PROBLEME_ANTARES_A_RESOUDRE* problem,
                                                  std::string_view name) const;

    /**
     * @brief Retrieves common problem data, the part common to every weekly problems
     *
     * This function takes a pointer to a PROBLEME_ANTARES_A_RESOUDRE object and retrieves the
     * common problem data. It returns a ConstantDataFromAntaresPtr to the common problem data.
     *
     * @param problem A pointer to the problem from which to retrieve the common data.
     * @return ConstantDataFromAntaresPtr A ConstantDataFromAntaresPtr to the common problem data.
     */
    [[nodiscard]] ConstantDataFromAntares commonProblemData(
      const PROBLEME_ANTARES_A_RESOUDRE* problem) const;
};

} // namespace Antares::Solver
