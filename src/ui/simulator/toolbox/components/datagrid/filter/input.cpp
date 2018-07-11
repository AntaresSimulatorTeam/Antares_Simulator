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

#include "input.h"
#include <wx/choice.h>
#include <wx/sizer.h>
#include "../../../resources.h"
#include "component.h"
#include "../../button.h"


using namespace Yuni;


namespace Antares
{
namespace Toolbox
{
namespace Filter
{

	namespace // anonymous
	{
		//! The next global ID for inputs
		int gInputNextId = 0;

	} // anonymous namespace



	void Input::addStdPreset()
	{
		this->add(wxT("any"));
		this->add(wxT("day"));
		this->add(wxT("dayyear"));
		this->add(wxT("hour"));
		this->add(wxT("houryear"));
		this->add(wxT("month"));
		this->add(wxT("week"));
		this->add(wxT("weekday"));
		this->add(wxT("columnindex"));
	}



	Input::Input(Component* parent) :
		Antares::Component::Panel(parent),
		pId(gInputNextId++),
		pParent(parent),
		pSelected(nullptr),
		pBtnMinus(nullptr),
		pBtnPlus(nullptr),
		pPrecision(Date::stepAny)
	{
		// Main sizer
		auto* sizer = new wxBoxSizer(wxHORIZONTAL);
		SetSizer(sizer);

		// -
		pBtnMinus = new Antares::Component::Button(this, wxEmptyString, "images/16x16/minus.png",
			this, &Input::onRemoveFilter);
		sizer->Add(pBtnMinus, 0, wxALL|wxALIGN_CENTER, 1);

		// +
		pBtnPlus = new Antares::Component::Button(this, wxEmptyString, "images/16x16/plus.png",
			this, &Input::onAddFilter);
		sizer->Add(pBtnPlus, 0, wxALL|wxALIGN_CENTER, 1);

		sizer->AddSpacer(5);

		// Filter selector
		pChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 22));
		sizer->Add(pChoice, 0, wxALL|wxEXPAND);

		// Sizer for the controls of the current filter
		pFilterSizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(pFilterSizer, 1, wxALL|wxEXPAND);

		sizer->Layout();

		// Events
		pChoice->Connect(pChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
			wxCommandEventHandler(Input::onFilterChanged), nullptr, this);
	}


	Input::~Input()
	{
		// Disconnection
		if (pBtnMinus)
			pBtnMinus->disconnectClickEvent();
		if (pBtnPlus)
			pBtnPlus->disconnectClickEvent();
	}



	void Input::add(const wxString& filterName)
	{
		if (not filterName.empty())
		{
			// Inserting the new filter in the list of available filters
			const wxChar* caption = AFilterBase::CaptionFromName(filterName, pPrecision);
			if (caption)
			{
				auto* data = new SelectorClientData(filterName);
				// insert new item at the end
				pChoice->Insert(caption, pChoice->GetCount(), data);

				// Autoselection - The first one
				if (1 == pChoice->GetCount())
				{
					pChoice->SetSelection(0 /* The first item in the list */);
					selectFilter(data);
				}
			}
		}
	}



	void Input::onFilterChanged(wxCommandEvent& evt)
	{
		auto* data = dynamic_cast<SelectorClientData*>(evt.GetClientObject());
		if (data)
		{
			# ifndef NDEBUG
			String textdbg;
			wxStringToString(data->id, textdbg);
			logs.debug() << "  component.datagrid: selecting " << textdbg;
			# endif
			selectFilter(data);
			pParent->refresh();
		}
		else
		{
			# ifndef NDEBUG
			logs.error() << "component.datagrid: no data from wxChoice::GetClientObject()";
			# endif
		}
	}


	void Input::selectFilter(SelectorClientData* data)
	{
		// Create the filter if not already exists
		if (data)
		{
			if (data->createIfNeeded(this, this))
			{
				assert(pFilterSizer && "The sizer for the filter must not be null");
				data->attachToSizer(*pFilterSizer);
				pSelected = data;
			}
			else
			{
				# ifndef NDEBUG
				logs.error() << "component.datagrid.selectFilter: createIfNeeded failed";
				# endif
			}
		}
	}





	Input::SelectorClientData::SelectorClientData(const wxString& i) :
		id(i),
		filter(nullptr)
	{}


	Input::SelectorClientData::~SelectorClientData()
	{
		delete filter;
	}


	AFilterBase* Input::SelectorClientData::createIfNeeded(Input* input, wxWindow* parent)
	{
		if (!filter)
		{
			filter = AFilterBase::FactoryCreate(input, id);
			if (filter)
				filter->recreateGUI(parent);
		}
		return filter;
	}


	void Input::SelectorClientData::attachToSizer(wxSizer& sizer)
	{
		if (filter)
		{
			/* Asserts */
			assert(filter->sizer());

			// Hide all controls
			auto end = sizer.GetChildren().end();
			for (auto i = sizer.GetChildren().begin(); i != end; ++i)
				(*i)->Show(false);

			// Attaching our filter to the given sizer
			if (!sizer.GetItem(filter->sizer()))
				sizer.Add(filter->sizer(), 0, wxALL|wxEXPAND);

			// Make the new sizer visible
			sizer.Show(filter->sizer(), true);
			filter->refreshGUIOperator();

			// Layout for sizers
			filter->sizer()->Layout();
			sizer.Layout();
		}
	}


	void Input::onRemoveFilter(void*)
	{
		if (pParent)
		{
			// Disconnection first to avoid SegV
			if (pBtnMinus)
				pBtnMinus->disconnectClickEvent();
			if (pBtnPlus)
				pBtnPlus->disconnectClickEvent();
			pBtnMinus = nullptr;
			// remove me !
			pParent->remove(pId);
		}
	}


	void Input::onAddFilter(void*)
	{
		if (pParent)
		{
			pParent->add();
			pParent->refresh();
		}
	}


	void Input::showBtnToRemoveFilter(bool visible)
	{
		if (pBtnMinus && GetSizer())
		{
			GetSizer()->Show(pBtnMinus, visible);
			GetSizer()->Layout();
		}
	}





} // namespace Filter
} // namespace Toolbox
} // namespace Antares

