// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_SETS_H__
#define __ANTARES_APPLICATION_WINDOWS_SETS_H__

#include <ui/common/component/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>

namespace Antares::Window
{
/*!
** \brief `Save As` Dialog
*/
class Sets final: public Antares::Component::Panel, public Yuni::IEventObserver<Sets>
{
public:
    enum IDs
    {
        mnIDUserNotes = wxID_HIGHEST + 1,
    };

    /*!
    ** \brief Event triggered when the sets are changed
    */
    static Yuni::Event<void(Sets* sender)> OnChanged;

public:
    // \name Constructors & Destructor
    //@{
    /*!
    ** \brief Constructor with a parent window and a study
    */
    Sets(wxWindow* parent, uint margin = 18);
    //! Destructor
    virtual ~Sets();
    //@}

    /*!
    ** \brief Save the sets to the study
    */
    void saveToStudy();

    /*!
    ** \brief Load the sets from the study
    */
    void loadFromStudy();

    /*!
    ** \brief Connect to the notification of sets updates
    */
    void connectToNotification();

    /*!
    ** \brief Disconnect from the notifications of the sets updates
    */
    void disconnectFromNotification();

private:
    void onNewSet(void*);

    void onCaption(void*);
    void onFilter(void*);
    void onOutput(void*);
    void onComments(void*);

    void onAdd(void*);
    void onRemove(void*);

    void onCheck(void*);

    void onUserNotesCharacter(wxRichTextEvent& evt);
    void onUserNotesStyleChanged(wxRichTextEvent& evt);

    void onSetsModified(Sets* sender);

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

}; // class Sets

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_WINDOWS_SETS_H__
