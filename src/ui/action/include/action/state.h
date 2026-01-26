// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_STATE_H__
#define __ANTARES_LIBS_STUDY_ACTION_STATE_H__

namespace Antares::Action
{
/*!
** \brief State of the action
*/
enum State
{
    //! Unknown
    stUnknown = 0,
    //! Disabled
    stDisabled,
    //! Error
    stError,
    //! Nothing to do
    stNothingToDo,
    //! Ready for execution
    stReady,
    //! A conflict has been found
    stConflict,

    //! The maximum number of states
    stMax

}; // enum State

/*!
** \brief Convert a state into its human string representation
*/
const char* StateToString(State state);

} // namespace Antares::Action

#endif // __ANTARES_LIBS_STUDY_ACTION_STATE_H__
