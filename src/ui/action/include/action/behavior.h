// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
#define __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__

namespace Antares::Action
{
/*!
** \brief Behavior for a single action
*/
enum Behavior
{
    //! Disabled the action and all sub-actions
    bhSkip = 0,
    /*!
    ** \brief Create if not exists
    */
    bhMerge,
    //! Overwrite any existing item
    bhOverwrite,

    //! The maximum number of behaviors
    bhMax

}; // enum Behavior

/*!
** \brief Convert a behavior into its human string representation
*/
const char* BehaviorToString(Behavior behavior);

} // namespace Antares::Action

#endif // __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
