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

#ifndef ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP
#define ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP

namespace Antares {

namespace Data {


/*! Enum class to define unfeasible problem behavior */
enum class UnfeasibleProblemBehavior : unsigned char {
    WARNING_DRY, /*! Continue simulation without MPS export */
	WARNING_MPS, /*! Continue simulation with MPS export */
	ERROR_DRY,   /*! Stop simulation without MPS export */
	ERROR_MPS    /*! Stop simulation with MPS export */
};

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

}  // namespace Data

}  // namespace Antares

#endif  // ANTARES_DATA_UNFEASIBLEPROBLEMHAVIOR_HPP