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
#ifndef __ANTARES_WINDOWS_XCAST_XCAST_HXX__
# define __ANTARES_WINDOWS_XCAST_XCAST_HXX__

# include <assert.h>
# include <wx/statline.h>
# include "../../toolbox/components/datagrid/renderer/area/xcast-k.h"
# include "../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h"
# include "../../toolbox/components/datagrid/renderer/area/xcast-translation.h"
# include "../../toolbox/components/datagrid/renderer/area/xcast-conversion.h"
# include "../../toolbox/validator.h"
# include "../../application/study.h"
# include "../../windows/inspector.h"
# include <ui/common/component/panel.h>



namespace Antares
{
namespace Window
{


	template<enum Data::TimeSeries T>
	XCast<T>::XCast(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
		wxPanel(parent, wxID_ANY),
		pNotifier(notifier),
		pArea(nullptr)
	{
		assert(pNotifier != NULL);

		// Grids
		Component::Datagrid::Component* grid;

		auto* hsizer = new wxBoxSizer(wxVERTICAL);
		this->SetSizer(hsizer);

		auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
		pNotebook = notebook;
		notebook->theme(Component::Notebook::themeLight);

		grid = new Component::Datagrid::Component(notebook,
			new Component::Datagrid::Renderer::XCastCoefficients<T>(this, notifier));
		pGridCoeffs = grid;
		pPageGeneral = notebook->add(grid, wxT("coeffs"), wxT("Coefficients"));

		grid = new Component::Datagrid::Component(notebook,
			new Component::Datagrid::Renderer::XCastK<T>(this, notifier));
		pPageDailyProfile = notebook->add(grid, wxT("Daily profile"));


		grid = new Component::Datagrid::Component(notebook,
			new Component::Datagrid::Renderer::XCastTranslation<T>(this, notifier));
		pPageDailyProfile = notebook->add(grid, wxT("Translation"));

		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		hsizer->Add(sizer, 0, wxALL|wxEXPAND);
		hsizer->Add(notebook, 1, wxALL|wxEXPAND);

		{
			auto* pGridSizer = new wxFlexGridSizer(2, 0, 0);
			wxStaticText* t;
			wxTextCtrl* edit;

			// Capacity
			{
				t = new wxStaticText(this, wxID_ANY, wxT("Capacity : "),
					wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
				wxFont f = t->GetFont();
				f.SetWeight(wxFONTWEIGHT_BOLD);
				t->SetFont(f);
				pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

				edit = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
					wxDefaultPosition, wxSize(-1,22), 0, Toolbox::Validator::Numeric());
				pInstalledCapacity = edit;
				pGridSizer->Add(edit, 1, wxALL| wxEXPAND, 1);
			}


			// Probability distribution
			t = new wxStaticText(this, wxID_ANY, wxT("Distribution : "),
				wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			wxArrayString list;
			for (uint i = 1; i < Data::XCast::dtMax; ++i)
				list.Add(wxStringFromUTF8(Data::XCast::DistributionToCString(Data::XCast::Distribution(i))));
			wxChoice* ch = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1,22), list);
			pDistribution = ch;
			pGridSizer->Add(ch, 1, wxALL| wxEXPAND, 1);

			// TS Translation
			t = new wxStaticText(this, wxID_ANY, wxT("Translation : "),
				wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			// see XCast::CStringToTSTranslationUse if you modifiy this list
			list.Clear();
			list.Add(wxT("Do not use"));
			list.Add(wxT("Add BEFORE scaling"));
			list.Add(wxT("Add AFTER scaling"));
			useTranslation = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1,22), list);
			pGridSizer->Add(useTranslation, 1, wxALL| wxEXPAND, 1);

			wxBoxSizer* divider = new wxBoxSizer(wxHORIZONTAL);
			divider->Add(20, 5);
			divider->Add(pGridSizer, 0, wxALL|wxEXPAND, 7);
			divider->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxALL|wxEXPAND, 5);
			divider->AddSpacer(15);
			wxBoxSizer* transferSizer = new wxBoxSizer(wxVERTICAL);

			useConversion = new wxCheckBox(this, wxID_ANY, wxT(" Use Conversion"));
			transferSizer->AddSpacer(15);
			transferSizer->Add(useConversion, 0, wxALL|wxEXPAND);

			divider->Add(transferSizer, 1, wxALL|wxEXPAND);

			sizer->Add(divider, 1, wxALL|wxEXPAND);


			pDistribution->Connect(pDistribution->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
				wxCommandEventHandler(XCast<T>::onUpdateDistribution), NULL, this);
			useTranslation->Connect(useTranslation->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
				wxCommandEventHandler(XCast<T>::onUpdateTSTranslationUse), NULL, this);
			useConversion->Connect(useConversion->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(XCast<T>::onUpdateConversion), NULL, this);
		}

		{
			Antares::Component::Panel* panel = new Antares::Component::Panel(notebook);
			wxSizer* ss = new wxBoxSizer(wxVERTICAL);

			grid = new Component::Datagrid::Component(panel,
				new Component::Datagrid::Renderer::XCastConversion<T>(this, notifier));
			ss->Add(grid, 0, wxALL|wxEXPAND);
			ss->SetItemMinSize(grid, 100, 150);

			panel->SetSizer(ss);
			pPageTranslation = notebook->add(panel, wxT("Conversion"));
		}

		// Events
		OnStudyClosed.connect(this, &XCast<T>::onStudyClosed);
		if (notifier)
			notifier->onAreaChanged.connect(this, &XCast<T>::onAreaChanged);
		pInstalledCapacity->Connect(pInstalledCapacity->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(XCast<T>::onInstalledCapacityChanged), NULL, this);

		// Select the page by default
		pPageGeneral->select();
	}


	template<enum Data::TimeSeries T>
	inline XCast<T>::~XCast()
	{
		ObserverAncestorType::destroyBoundEvents();
	}


	template<enum Data::TimeSeries T>
	void XCast<T>::onUpdateDistribution(wxCommandEvent& evt)
	{
		wxChoice* obj = (wxChoice*)evt.GetEventObject();
		if (obj and pArea)
		{
			Data::XCast* xcast = pArea->xcastData<T>();
			if (!xcast)
				return;

			YString s;
			wxStringToString(obj->GetStringSelection(), s);
			auto d = Data::XCast::StringToDistribution(s);
			if (d != Data::XCast::dtNone and d != xcast->distribution)
			{
				xcast->distribution = d;
				MarkTheStudyAsModified();
				Window::Inspector::Refresh();
				if (pGridCoeffs and pNotebook->selected() and pNotebook->selected()->name() == wxT("coeffs"))
					pGridCoeffs->Refresh();
			}
		}
	}


	template<enum Data::TimeSeries T>
	void XCast<T>::onUpdateConversion(wxCommandEvent& evt)
	{
		Data::XCast* xcast = pArea->xcastData<T>();
		if (!xcast)
			return;

		if (xcast->useConversion != evt.IsChecked())
		{
			xcast->useConversion = evt.IsChecked();
			MarkTheStudyAsModified();
			Window::Inspector::Refresh();
		}
	}


	template<enum Data::TimeSeries T>
	void XCast<T>::onUpdateTSTranslationUse(wxCommandEvent& evt)
	{
		wxChoice* obj = (wxChoice*)evt.GetEventObject();
		if (obj and pArea)
		{
			Data::XCast* xcast = pArea->xcastData<T>();
			if (!xcast)
				return;

			YString s;
			wxStringToString(obj->GetStringSelection(), s);
			Data::XCast::TSTranslationUse d = Data::XCast::CStringToTSTranslationUse(s);
			if (d != xcast->useTranslation)
			{
				xcast->useTranslation = d;
				MarkTheStudyAsModified();
				Window::Inspector::Refresh();
			}
		}
	}




	template<enum Data::TimeSeries T>
	inline void XCast<T>::selectDefaultPage()
	{
		if (pPageGeneral)
			pPageGeneral->select();
	}



	template<enum Data::TimeSeries T>
	void XCast<T>::onStudyClosed()
	{
		pArea = nullptr;
		selectDefaultPage();
	}


	template<enum Data::TimeSeries T>
	void XCast<T>::onAreaChanged(Data::Area* area)
	{
		using namespace Yuni;
		// Keeping a pointer to the current area
		pArea = area;

		// Updating the installed capacity of the current area
		if (pArea)
		{
			Data::XCast* xcast = pArea->xcastData<T>();
			if (xcast)
			{
				// Capacity
				pInstalledCapacity->ChangeValue(DoubleToWxString(xcast->capacity));

				// Distribution
				int indx = (int)xcast->distribution - 1;
				if (indx < 0 or indx > 5)
					indx = 1;
				pDistribution->SetSelection(indx);

				// Transfert
				useConversion->SetValue(xcast->useConversion);
				useTranslation->SetSelection((int)xcast->useTranslation);
				return;
			}
		}
		pInstalledCapacity->ChangeValue(wxT("0"));
		pDistribution->SetSelection(1);
		useConversion->SetValue(false);
		useTranslation->SetSelection(0);
	}


	template<enum Data::TimeSeries T>
	void XCast<T>::onInstalledCapacityChanged(wxCommandEvent& evt)
	{
		if (!pArea)
			return;
		double d;
		evt.GetString().ToDouble(&d);
		if (d < 0.)
			d = 0.;

		Data::XCast* xcastData = pArea->xcastData<T>();
		if (xcastData and not Yuni::Math::Equals(d, xcastData->capacity))
		{
			xcastData->capacity = d;
			MarkTheStudyAsModified();
		}
	}





} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_XCAST_XCAST_HXX__
