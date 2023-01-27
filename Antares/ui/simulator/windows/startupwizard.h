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
#ifndef __ANTARES_APPLICATION_WINDOWS_STARTUP_WIZARD_H__
#define __ANTARES_APPLICATION_WINDOWS_STARTUP_WIZARD_H__

#include <antares/wx-wrapper.h>
#include <wx/dialog.h>
#include <map>
#include "../toolbox/components/button.h"

namespace Antares
{
namespace Window
{
/*!
** \brief Startup Wizard User Interface
**
** The Startup Wizard is an easy to use, quick way to begin using Antares.
** It provides the most common File Operations on a Study.
** By default, you will be prompted to use the Startup Wizard when Antares is opened.
*/
class StartupWizard final : public wxDialog
{
public:
    /*!
    ** \brief Show an instance of StartupWizard
    */
    static void Show();

    /*!
    ** \brief Close any opened startup window
    */
    static void Close();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    explicit StartupWizard(wxFrame* parent);
    //! Destructor
    virtual ~StartupWizard();
    //@}

private:
    /*!
    ** \brief Add control which will interact with the user
    **
    ** \param mapping An internal string representation of the command to execute
    **        (`new`, `open`, <other>)
    ** \param image The icon to use
    ** \param title Title
    ** \param subtitle The subtitle (the same font than the title with a lower size)
    ** \param useEvents True if the controls can be clicked by the user
    */
    void addControls(const wxString& mapping,
                     const char* image,
                     const wxString& title,
                     const wxString& subtitle = wxString(),
                     bool useEvents = true);

    void addRecentItem(const wxString& mapping, const wxString& title, const wxString& path);

    /*!
    ** \brief Reset all default background colors of a control and its children
    ** \param selected The control
    */
    void resetAllBackgroundColors(wxWindow* selected);

    //! Event: Click on a control (New, Open or Recent)
    void onClick(wxCommandEvent& evt);

    //! Event: Close the window
    void onClose(void*);

    void onDropDownExamples(Component::Button&, wxMenu&, void*);
    void showAllExamples();
    //! Prepare the popup menu of all examples
    // void onPreparePopupMenu();
    //! Display the popup menu of all examples
    void onDisplayPopupMenu();

    //! Event: the mouse has moved on the window
    void onWindowMotion(wxMouseEvent& evt);
    //! Event: the mouse has moved on a control
    void onMotion(wxMouseEvent& evt);

    //! Load recent files and create controls according to it
    void loadRecentFiles();

private:
    /*!
    ** \brief An ugly way to get at runtime what task to do
    ** \todo Find a better way to achieve this
    */
    std::map<int, wxString> pMapping;
    //! The default background color
    wxColour pDefaultBgColor;
    //! The default color for selected items
    wxColour pSelectedBgColor;
    //! ID of the panel with
    int pPanelButtonsID;
    //! The last selected panel
    wxWindow* pLastSelected;
    //! Allow to redraw the selection
    // This variable is nearly only useful under Windows to prevent useless changes
    // selecting a folder
    bool pAllowDisplaySelection;
    //! The popup menu
    wxMenu* pMenuExamples;
    //! Relationship between menu::id and the path
    std::map<int, Yuni::String> pMenuLinks;

    //! The button 'example'
    wxWindow* pBtnExamples;
    //! The example folder
    Yuni::String pExFolder;

}; // class StartupWizard

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_STARTUP_WIZARD_H__
