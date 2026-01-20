// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

#ifdef BUILD_UI
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
#endif

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
