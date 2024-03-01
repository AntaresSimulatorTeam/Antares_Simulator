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
#ifndef __ANTARES_LIBS_STUDY_ACTION_STATE_H__
#define __ANTARES_LIBS_STUDY_ACTION_STATE_H__

namespace Antares
{
namespace Action
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

} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_STATE_H__
