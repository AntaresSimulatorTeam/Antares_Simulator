/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
#define __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__

namespace Antares
{
namespace Action
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

} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
