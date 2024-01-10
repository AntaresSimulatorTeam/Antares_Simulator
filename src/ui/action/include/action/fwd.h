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
#ifndef __ANTARES_LIBS_STUDY_ACTION_FWD_H__
#define __ANTARES_LIBS_STUDY_ACTION_FWD_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <antares/study/study.h>
#include <action/behavior.h>
#include <action/state.h>

namespace Antares
{
namespace Action
{
enum LevelOfDetails
{
    //! For standard purposes
    lodStandard = 0,
    //! For advanced purposes
    lodAdvanced,
};

class IAction;

//! Property map
using PropertyMap = std::map<Yuni::String, Yuni::String>;

class ActionInformations
{
public:
    ActionInformations() : behavior(bhOverwrite), state(stUnknown)
    {
    }

public:
    //! Behavior of the action
    Behavior behavior;
    //! State of the action
    State state;
    //! Caption
    Yuni::String caption;
    //! Message
    Yuni::String message;
    //! Properties
    PropertyMap property;

}; // class ActionInformations

} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_FWD_H__
