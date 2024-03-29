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
#ifndef __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__
#define __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__

#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/bindingconstraint.h"
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Window
{
class BindingConstraintWeightsPanel;
class BindingConstraintOffsetsPanel;

/*!
** \brief Settings for a single Binding constraint
*/
class BindingConstraint : public Component::Panel, public Yuni::IEventObserver<BindingConstraint>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param parent The parent window
    */
    BindingConstraint(wxWindow* parent);
    //! Destructor
    virtual ~BindingConstraint();
    //@}

    void selectDefaultPage();

private:
    void onPageChanged(Component::Notebook::Page& page);

    void onSelectedConstraint(Data::BindingConstraint* contraint);
    void onDblClickConstraint(Data::BindingConstraint* contraint);

    void onAdd(void*);
    void onAddFromMouse(wxMouseEvent&);
    void onDelete(void*);
    void onDeleteFromMouse(wxMouseEvent&);
    void onEdit(void*);
    void onDeleteAll(void*);
    void onEditFromMouse(wxMouseEvent&);

private:
    Component::Notebook::Page* pPageList;
    Component::Notebook::Page* pPageWeights;
    Component::Notebook::Page* pPageOffsets;
    Toolbox::InputSelector::BindingConstraint* pAllConstraints;
    Data::BindingConstraint* pSelected;

    BindingConstraintWeightsPanel* pWeightsPanel;
    BindingConstraintOffsetsPanel* pOffsetsPanel;

public:
    // Event table
    DECLARE_EVENT_TABLE()

}; // class BindingConstraint

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__
