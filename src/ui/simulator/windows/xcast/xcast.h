/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/
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

namespace Antares
{
namespace Window
{
template<enum Data::TimeSeriesType T>
class XCast final : public wxPanel, public Yuni::IEventObserver<XCast<T>>
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

} // namespace Window
} // namespace Antares

#include "xcast.hxx"

#endif // __ANTARES_WINDOWS_XCAST_XCAST_H__
