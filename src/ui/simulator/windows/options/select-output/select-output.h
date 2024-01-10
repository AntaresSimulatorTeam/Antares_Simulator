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
*/
#ifndef __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__
#define __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__

#include "../../../toolbox/components/datagrid/component.h"
#include <wx/dialog.h>
#include <wx/checkbox.h>

namespace Antares
{
namespace Window
{
namespace Options
{
/*!
** \brief Startup Wizard User Interface
**
** The Startup Wizard is an easy to use, quick way to begin using Antares.
** It provides the most common File Operations on a Study.
** By default, you will be prompted to use the Startup Wizard when Antares is opened.
*/
class SelectOutput final : public wxDialog
{
public:
    enum IDs
    {
        mnIDEnabled = wxID_HIGHEST + 1,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    SelectOutput(wxFrame* parent);
    //! Destructor
    virtual ~SelectOutput();
    //@}

protected:
    void mouseMoved(wxMouseEvent& evt);

protected:
    //! Select all output variables
    void onSelectAll(void*);
    void onUnselectAll(void*);
    void onToggle(void*);
    //! Event: Close the window
    void onClose(void*);
    void onUpdateStatus();
    void evtEnabled(wxCommandEvent& evt);
    void updateCaption();

private:
    Component::Datagrid::Component* pGrid;
    wxCheckBox* pStatus;
    wxWindow* pPanel;
    wxWindow* pInfo;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class SelectOutput

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__
