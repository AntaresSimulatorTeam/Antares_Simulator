/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_WINDOWS_INSPECTOR_GRID_H__
#define __ANTARES_WINDOWS_INSPECTOR_GRID_H__

#include <antares/wx-wrapper.h>
#include <antares/study.h>
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
    typedef Yuni::CString<128, false> PropertyNameType;

    void OnPropertyChanging(wxPropertyGridEvent& event);
    bool onPropertyChanging_A(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_C(wxPGProperty*, const PropertyNameType& name, const wxVariant& value);
    bool onPropertyChanging_Constraint(wxPGProperty*,
                                       const PropertyNameType& name,
                                       const wxVariant& value);
    bool onPropertyChanging_Cluster(wxPGProperty*,
                                    const PropertyNameType& name,
                                    const wxVariant& value);
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
