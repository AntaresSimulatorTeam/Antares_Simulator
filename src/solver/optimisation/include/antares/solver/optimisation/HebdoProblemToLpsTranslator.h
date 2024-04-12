
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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/lps/LpsFromAntares.h"
#include "opt_period_string_generator_base.h"

namespace Antares::Solver
{

/**
 * @class HebdoProblemTranslationException
 * @brief Exception class for errors during the translation of a weekly problem.
 *
 * This class is a custom exception class that is thrown when an error occurs during the translation of a weekly problem.
 */
class HebdoProblemTranslationException: public std::runtime_error
{
public:
    explicit HebdoProblemTranslationException(const std::string& string) noexcept;
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
    [[nodiscard]] HebdoDataFromAntaresPtr translate(const PROBLEME_ANTARES_A_RESOUDRE* problem,
                                                    std::string_view name) const;
    [[nodiscard]] ConstantDataFromAntaresPtr commonProblemData(const PROBLEME_ANTARES_A_RESOUDRE* problem) const;
};

} // namespace Antares::Solver
