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
#ifndef __ANTARES_LIBS_STUDY_ACTION_TEXT_H__
#define __ANTARES_LIBS_STUDY_ACTION_TEXT_H__

#include <yuni/yuni.h>
#include "action.h"

namespace Antares
{
namespace Action
{
/*!
** \brief A dummy action, for displaying some text
*/
template<bool AutoExpandT = true>
class Text : public IAction
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
    template<class StringT>
    explicit Text(const StringT& text);
    //! Destructor
    virtual ~Text();
    //@}

    virtual bool bold() const;

    virtual bool autoExpand() const;

    virtual bool canDoSomething() const;

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

}; // class IAction

} // namespace Action
} // namespace Antares

#include "text.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_TEXT_H__
