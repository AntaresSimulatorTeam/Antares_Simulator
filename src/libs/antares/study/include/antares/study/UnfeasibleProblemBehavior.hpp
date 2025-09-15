/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#ifndef ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP
#define ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP

namespace Antares::Data
{
/*! Enum class to define unfeasible problem behavior */
enum class UnfeasibleProblemBehavior : unsigned char
{
    WARNING_DRY, /*! Continue simulation without MPS export */
    WARNING_MPS, /*! Continue simulation with MPS export */
    ERROR_DRY,   /*! Stop simulation without MPS export */
    ERROR_MPS    /*! Stop simulation with MPS export */
};

/*!
 *  \brief Get icon from UnfeasibleProblemBehavior enum
 *
 *  \param unfeasibleProblemBehavior : UnfeasibleProblemBehavior enum
 *  \return icon
 */
const char* getIcon(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior);

/*!
 *  \brief Get display name from UnfeasibleProblemBehavior enum
 *
 *  \param unfeasibleProblemBehavior : UnfeasibleProblemBehavior enum
 *  \return displayName
 */
std::string getDisplayName(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior);

/*!
 *  \brief Define if MPS must be exported in case of unfeasible problem
 *
 *  \param unfeasibleProblemBehavior : UnfeasibleProblemBehavior enum
 *  \return true if MPS must be exported, false otherwise
 */
bool exportMPS(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior);

/*!
 *  \brief Define if simulation must be stopped in case of unfeasible problem
 *
 *  \param unfeasibleProblemBehavior : UnfeasibleProblemBehavior enum
 *  \return true if simulation must be stopped, false otherwise
 */
bool stopSimulation(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior);

} // namespace Antares::Data

#endif // ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP
