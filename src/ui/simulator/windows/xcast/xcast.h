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
#ifndef __ANTARES_WINDOWS_XCAST_XCAST_H__
#define __ANTARES_WINDOWS_XCAST_XCAST_H__

#include <antares/wx-wrapper.h>
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <yuni/core/event.h>
#include <antares/study.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/choice.h>

namespace Antares
{
namespace Window
{
template<enum Data::TimeSeries T>
class XCast final : public wxPanel, public Yuni::IEventObserver<XCast<T>>
{
public:
    //! The notebook ancestor type
    typedef Component::Notebook NotebookType;
    //! The Observer ancestor type
    typedef Yuni::IEventObserver<XCast<T>> ObserverAncestorType;

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
