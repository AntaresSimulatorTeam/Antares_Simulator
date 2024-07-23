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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Link
{
class Create : public IAction
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
    Create(const Data::AreaName& fromarea, const Data::AreaName& toarea);

    Create(const Data::AreaName& fromarea,
           const Data::AreaName& targetfromarea,
           const Data::AreaName& toarea,
           const Data::AreaName& targettoarea);

    //! Destructor
    virtual ~Create();
    //@}

    virtual bool allowUpdate() const;

    /*!
    ** \brief Create all standard actions for copy/pasting an area
    */
    void createActionsForAStandardLinkCopy(Context& ctx);

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! From
    Data::AreaName pOriginalFromAreaName;
    Data::AreaName pTargetFromAreaName;
    //! To
    Data::AreaName pOriginalToAreaName;
    Data::AreaName pTargetToAreaName;

}; // class IAction

} // namespace Link
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

#include "create.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__
