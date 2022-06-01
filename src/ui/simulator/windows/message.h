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
#ifndef __ANTARES_APPLICATION_MESSAGE_H__
#define __ANTARES_APPLICATION_MESSAGE_H__

#include <antares/wx-wrapper.h>
#include <yuni/thread/thread.h>
#include <wx/dialog.h>
#include <ui/common/component/spotlight.h>

namespace Antares
{
namespace Window
{
/*!
** \brief Standard message Box for Antares
**
** \code
** // The main form
** Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
**
** // Creating the message dialog
** Window::Message message(&mainFrm, wxT("Title of the form"),
**		wxT("Main message (in bold)"),
**		wxT("Hello World !"));
**
** // Adding some buttons
** message.add(Window::Message::btnUpgrade);
** message.add(Window::Message::btnSaveAs, false, 15);
** message.add(Window::Message::btnCancel, true);
**
** // Display the message
** switch (message.showModal())
** {
**	case Window::Message::btnUpgrade: do something; break;
**	case Window::Message::btnSaveAs : ...; break;
** }
** \endcode
**
** \note The method `showModal()` should be used instead of `ShowModal()`
*/
class Message final : public wxDialog
{
public:
    enum DefaultButtonType
    {
        //! constant
        btnStartID = 100000,
        //! Standard button: Ok
        btnOk,
        //! Standard button: Yes
        btnYes,
        //! Standard button: No
        btnNo,
        //! Standard button: retry
        btnRetry,
        //! Standard button: Discard
        btnDiscard,
        //! Quit without saving
        btnQuitWithoutSaving,
        //! Standard button: Save changes
        btnSaveChanges,
        //! Standard button: Cancel
        btnCancel,
        //! Standard button: Save as...
        btnSaveAs,
        //! Standard button: Upgrade
        btnUpgrade,
        //! Standard button: continue
        btnContinue,
        //! Standard button: view simulation results
        btnViewResults,
        //! Standard button: quit
        btnQuit
    };
    //! Array of items
    using ItemList = std::vector<Component::Spotlight::IItem::Ptr>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Message(wxWindow* parent,
            const wxString& title,
            const wxString& subtitle = wxEmptyString,
            const wxString& msg = wxEmptyString,
            const char* icon = "images/misc/book.png");
    //! Destructor
    virtual ~Message();
    //@}

    //! \name Buttons
    //@{
    /*!
    ** \brief Add a new custom button
    **
    ** \param caption Caption of the button
    ** \param value An arbitrary unique identifier for this button. It should be less than
    **    btnStartID.
    ** \param defaultButton True to make it the default result
    ** \param space The space to add after before this button
    */
    void add(const wxString& caption,
             DefaultButtonType value,
             bool defaultButton = false,
             int space = 3);

    /*!
    ** \brief Add a predefined button
    **
    ** \param btn The predefined button ID
    ** \param defaultButton True to make it the default result
    ** \param space The space to add after before this button
    */
    void add(DefaultButtonType btn, bool defaultButton = false, int space = 3);
    //@}

    //! \name List
    //@{
    /*!
    ** \brief Add an item in the list
    */
    void add(Component::Spotlight::IItem::Ptr item);
    /*!
    ** \brief Add an error
    */
    void appendError(const AnyString& text);
    /*!
    ** \brief Add a warning
    */
    void appendWarning(const AnyString& text);

    //! \see add(Component::Spotlight::IItem::Ptr)
    Message& operator+=(Component::Spotlight::IItem::Ptr item);
    //@}

    //! \name Show
    //@{
    /*!
    ** \brief Display the message box
    */
    uint showModal();

    /*!
    ** \brief Display the message box (async)
    */
    void showModalAsync();
    //@}

    //! \name Extras
    //@{
    /*!
    ** \brief Set the recommended width of the dialog box
    **
    ** A value of 0 means `automatic`. This value will be used
    ** only if the message list is not empty
    */
    void recommendedWidth(uint w);
    //@}

private:
    //! Event: A button has been clicked
    void onButtonClick(DefaultButtonType userdata);
    //
    void prepareShowModal();

private:
    //! The parent window for the user's buttons
    wxPanel* pPanel;
    //! The sizer for the text
    wxBoxSizer* pPanelSizer;
    //! Item list
    ItemList pItemList;
    //! Spotlight
    Component::Spotlight* pSpotlight;
    //! Sizer where the spotlight component will be found
    wxSizer* pListSizer;
    //! The return status code
    DefaultButtonType pReturnStatus;
    //! Recommended width
    uint pRecommendedWidth;
    //! Empty panel
    wxPanel* pSpace;

}; // class Message

} // namespace Window
} // namespace Antares

#include "message.hxx"

#endif // __ANTARES_APPLICATION_MESSAGE_H__
