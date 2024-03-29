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
#ifndef __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__
#define __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__

#include <yuni/core/event.h>
#include <antares/study/constraint.h>
#include "../input.h"

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
class BindingConstraint : public AInput, public Yuni::IEventObserver<BindingConstraint>
{
public:
    BindingConstraint(wxWindow* parent);
    virtual ~BindingConstraint();

    virtual void update();
    virtual void updateRowContent();
    virtual void updateInnerData(const wxString& searchString);

    virtual wxPoint recommendedSize() const
    {
        return wxPoint(150, 200);
    }

public:
    //! A binding constraint has been selected
    Yuni::Event<void(Data::BindingConstraint*)> onBindingConstraintChanged;
    //! A binding constraint has been double clicked
    Yuni::Event<void(Data::BindingConstraint*)> onBindingConstraintDblClick;

protected:
    virtual void internalBuildSubControls();

private:
    //! Event: The application is about to leave
    void onApplicationOnQuit();

    void onUpdate();
    void onStudyAreaUpdate(Data::Area*);
    void onStudyLinkUpdate(Data::AreaLink*);

    /*!
    ** \brief Clear the control and broadcast a change in the current selected area
    */
    void clear();

}; // class Area

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__
