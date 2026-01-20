// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_XCAST_XCAST_H__
#define __ANTARES_WINDOWS_XCAST_XCAST_H__

#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <yuni/core/event.h>
#include <antares/study/study.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/choice.h>

namespace Antares::Window
{
template<enum Data::TimeSeriesType T>
class XCast final: public wxPanel, public Yuni::IEventObserver<XCast<T>>
{
public:
    //! The notebook ancestor type
    using NotebookType = Component::Notebook;
    //! The Observer ancestor type
    using ObserverAncestorType = Yuni::IEventObserver<XCast<T>>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    XCast(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    /*!
    ** \brief Destructor
    */
    virtual ~XCast();
    //@}

    void selectDefaultPage();

private:
    //! Event: The study has been closed
    void onStudyClosed();
    //! Event: Another area has been selected
    void onAreaChanged(Data::Area* area);
    //! Event: The user has changed the installed capacity for the current area
    void onInstalledCapacityChanged(wxCommandEvent& evt);
    //! Event: the user has changed the distribution probability
    void onUpdateDistribution(wxCommandEvent& evt);
    //! Event: the user has changed the way to use the TS average
    void onUpdateTSTranslationUse(wxCommandEvent& evt);
    //! Event: the user has changed if the transfer function should be used
    void onUpdateConversion(wxCommandEvent& evt);

private:
    //! Area notifier
    Toolbox::InputSelector::Area* pNotifier;
    //! The current area
    Data::Area* pArea;
    //! Edit
    wxTextCtrl* pInstalledCapacity;
    wxChoice* pDistribution;
    wxCheckBox* useConversion;
    wxChoice* useTranslation;

    Component::Notebook* pNotebook;
    //
    Component::Notebook::Page* pPageDailyProfile;
    Component::Notebook::Page* pPageGeneral;
    Component::Notebook::Page* pPageTranslation;

    Component::Datagrid::Component* pGridCoeffs;

}; // class XCast

} // namespace Antares::Window

#include "xcast.hxx"

#endif // __ANTARES_WINDOWS_XCAST_XCAST_H__
