// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_FWD_H__
#define __ANTARES_LIBS_STUDY_ACTION_FWD_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <antares/study/study.h>
#include <action/behavior.h>
#include <action/state.h>

namespace Antares::Action
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
    ActionInformations():
        behavior(bhOverwrite),
        state(stUnknown)
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

} // namespace Antares::Action

#endif // __ANTARES_LIBS_STUDY_ACTION_FWD_H__
