// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#ifndef __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__
#define __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__

#include "../../../toolbox/components/datagrid/component.h"
#include <wx/dialog.h>
#include <wx/checkbox.h>

namespace Antares::Window::Options
{
class AdequacyPatchAreas final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    explicit AdequacyPatchAreas(wxFrame* parent);
    //! Destructor
    ~AdequacyPatchAreas() override;
    //@}

private:
    void mouseMoved(wxMouseEvent& evt);
    void onClose(const void*);

    Component::Datagrid::Component* pGrid;
    wxWindow* pPanel;
    DECLARE_EVENT_TABLE()

}; // class AdequacyPatchAreas

}

#endif // __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__
