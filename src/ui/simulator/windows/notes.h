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
#ifndef __ANTARES_APPLICATION_WINDOWS_NOTES_H__
#define __ANTARES_APPLICATION_WINDOWS_NOTES_H__

#include <antares/wx-wrapper.h>
#include <ui/common/component/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>

namespace Antares
{
namespace Window
{
/*!
** \brief `Save As` Dialog
*/
class Notes final : public Antares::Component::Panel, public Yuni::IEventObserver<Notes>
{
public:
    enum IDs
    {
        mnIDUserNotes = wxID_HIGHEST + 1,
    };

    /*!
    ** \brief Event triggered when the user notes are changed
    */
    static Yuni::Event<void(Notes* sender)> OnChanged;

public:
    // \name Constructors & Destructor
    //@{
    /*!
    ** \brief Constructor with a parent window and a study
    */
    Notes(wxWindow* parent, uint margin = 18);
    //! Destructor
    virtual ~Notes();
    //@}

    /*!
    ** \brief Save the comments to the study
    */
    void saveToStudy();

    /*!
    ** \brief Load the comments from the study
    */
    void loadFromStudy();

    /*!
    ** \brief Connect to the notification of user notes updates
    */
    void connectToNotification();

    /*!
    ** \brief Disconnect from the notifications of the user notes updates
    */
    void disconnectFromNotification();

private:
    void onBold(void*);
    void onItalic(void*);
    void onUnderline(void*);

    void onAlignLeft(void*);
    void onAlignRight(void*);
    void onAlignCenter(void*);

    void onListBullet(void*);
    void onListNumbered(void*);

    void onIndentIncrease(void*);
    void onIndentDecrease(void*);

    void appendStyles();

    void onUserNotesCharacter(wxRichTextEvent& evt);
    void onUserNotesStyleChanged(wxRichTextEvent& evt);

    void onNotesModified(Notes* sender);

    void notifyChanges();

    void onStudyClosed();

    /*!
    ** \brief Initialize temporary files addresses
    **
    ** This method must be called whenever pWxTempFile or pTempFile
    ** are empty (on-demand, to reduce startup time overhead as much
    ** as possible).
    */
    bool initializeTemporaryFile();

private:
    //! Rich edit
    wxRichTextCtrl* pRichEdit;
    wxRichTextStyleSheet* pStyleSheet;
    uint pLocalRevision;
    uint pUpdatesToSkip;

    /*!
    ** \brief Temporary file
    **
    ** These variables are empty on purpose at the startup. To get a proper
    ** content, please call initializeTemporaryFile() when needed
    */
    YString pTempFile;

    // Event Table
    DECLARE_EVENT_TABLE()

}; // class Notes

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_NOTES_H__
