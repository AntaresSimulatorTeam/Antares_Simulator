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
#ifndef __ANTARES_WINDOWS_INSPECTOR_GRID_H__
#define __ANTARES_WINDOWS_INSPECTOR_GRID_H__

#include <antares/study/study.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>
#include "frame.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
/*!
** \brief Custom implementation of a property grid
*/
class InspectorGrid final : public wxPropertyGrid
{
public:
    InspectorGrid(Frame& frame,
                  wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = (0)) :

     wxPropertyGrid(parent, id, pos, size, style), pFrame(frame)
    {
    }

    virtual ~InspectorGrid()
    {
    }

    void apply(const InspectorData::Ptr& data)
    {
        pCurrentSelection = data;
    }

protected:
    using PropertyNameType = Yuni::CString<128, false>;

    void OnPropertyChanging(wxPropertyGridEvent& event);
    bool onPropertyChanging_A(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_C(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_Constraint(wxPGProperty*,
                                       const PropertyNameType& name,
                                       const wxVariant& value);
    bool onPropertyChanging_ThermalCluster(wxPGProperty*,
                                           const PropertyNameType& name,
                                           const wxVariant& value);
    bool onPropertyChanging_RenewableClusters(const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_L(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_S(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);

private:
    //! Reference to the parent frame
    Frame& pFrame;
    //! The current selection
    InspectorData::Ptr pCurrentSelection;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class InspectorGrid

} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_INSPECTOR_GRID_H__
