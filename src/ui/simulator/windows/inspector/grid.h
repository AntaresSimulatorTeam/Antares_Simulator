// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_INSPECTOR_GRID_H__
#define __ANTARES_WINDOWS_INSPECTOR_GRID_H__

#include <antares/study/study.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>
#include "frame.h"

namespace Antares::Window::Inspector
{
/*!
** \brief Custom implementation of a property grid
*/
class InspectorGrid final: public wxPropertyGrid
{
public:
    InspectorGrid(Frame& frame,
                  wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = (0)):

        wxPropertyGrid(parent, id, pos, size, style),
        pFrame(frame)
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

} // namespace Antares::Window::Inspector

#endif // __ANTARES_WINDOWS_INSPECTOR_GRID_H__
