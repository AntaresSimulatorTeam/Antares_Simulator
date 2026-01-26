// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_NOTES_H__
#define __ANTARES_APPLICATION_WINDOWS_NOTES_H__

#include <ui/common/component/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>

namespace Antares::Window
{
/*!
** \brief `Save As` Dialog
*/
class Notes final: public Antares::Component::Panel, public Yuni::IEventObserver<Notes>
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

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_WINDOWS_NOTES_H__
