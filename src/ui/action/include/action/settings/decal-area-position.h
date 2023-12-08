/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__

#include <yuni/yuni.h>
#include "../action.h"

namespace Antares
{
namespace Action
{
namespace Settings
{
/*!
** \brief A dummy action, for displaying some text
*/
class DecalAreaPosition : public IAction
{
public:
    //! The most suitable smart ptr for the class
    using Ptr = IAction::Ptr;
    //! The threading policy
    using ThreadingPolicy = IAction::ThreadingPolicy;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit DecalAreaPosition(bool enabled = true);
    //! Destructor
    virtual ~DecalAreaPosition();
    //@}

    virtual bool autoExpand() const;

    virtual bool shouldPrepareRootNode() const;

    virtual void behaviorToText(Behavior behavior, Yuni::String& out);

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);
    virtual void prepareSkipWL(Context& ctx);

private:
    Yuni::String pValueX;
    Yuni::String pValueY;

}; // class DecalAreaPosition

} // namespace Settings
} // namespace Action
} // namespace Antares

#include "decal-area-position.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__
