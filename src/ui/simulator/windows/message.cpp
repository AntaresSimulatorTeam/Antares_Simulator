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

#include <yuni/yuni.h>
#include <yuni/core/system/memory.h>

#include "message.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>

#include <antares/study/version.h>
#include "../toolbox/resources.h"
#include "../toolbox/create.h"
#include "../application/study.h"
#include <ui/common/component/panel.h>
#include <ui/common/component/spotlight.h>

using namespace Yuni;



namespace Antares
{
namespace Window
{

	namespace // anonymous
	{

		class MessageProvider final : public Component::Spotlight::IProvider
		{
		public:
			//! Component
			typedef Component::Spotlight  Spotlight;

		public:
			//! \name Constructor & Destructor
			//@{
			/*!
			** \brief Default constructor
			*/
			explicit MessageProvider(Message::ItemList& items) :
				pItems(items)
			{}
			//! Destructor
			virtual ~MessageProvider()
			{
			}
			//@}


			virtual void search(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& /*tokens*/, const Yuni::String& text = "") override
			{
				uint count = (uint) pItems.size();
				for (uint i = 0; i != count; ++i)
					out.push_back(pItems[i]);
			}

		private:
			//! All messages
			Message::ItemList& pItems;

		}; // class IProvider




	} // nonymous namespace





	Message::Message(wxWindow* parent, const wxString& title, const wxString& subtitle, const wxString& msg,
		const char* icon) :
		wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize),
		pSpotlight(nullptr),
		pReturnStatus(-1),
		pRecommendedWidth(0)
	{
		// Informations about the study
		wxColour defaultBgColor = GetBackgroundColour();
		SetBackgroundColour(wxColour(255, 255, 255));

		auto* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddSpacer(10);

		auto* textSizer = new wxBoxSizer(wxHORIZONTAL);
		textSizer->AddSpacer(20);
		textSizer->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, icon), 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL);
		textSizer->AddSpacer(15);

		auto* v = new wxBoxSizer(wxVERTICAL);
		if (not subtitle.empty())
		{
			wxStaticText* sub = Component::CreateLabel(this, subtitle, true);
			sub->Wrap(600);
			v->Add(sub, 0, wxLEFT | wxALIGN_CENTER_VERTICAL);
			v->AddSpacer(10);
		}

		auto* lbl = Component::CreateLabel(this, msg);
		lbl->Wrap(600);
		v->Add(lbl, 0, wxLEFT | wxALIGN_CENTER_VERTICAL);

		if (not subtitle.empty())
			textSizer->Add(v, 0, wxALL | wxLEFT | wxTOP);
		else
			textSizer->Add(v, 0, wxALL | wxEXPAND);

		textSizer->SetItemMinSize(v, 50, 50);
		textSizer->AddSpacer(4);
		sizer->AddSpacer(16);
		sizer->Add(textSizer, 0, wxALL|wxEXPAND);

		{
			enum
			{
				flags = Component::Spotlight::optNoSearchInput | Component::Spotlight::optBkgWhite,
			};
			auto* sizerList = new wxBoxSizer(wxVERTICAL);
			pSpotlight = new Component::Spotlight(this, flags);
			pSpotlight->SetBackgroundColour(GetBackgroundColour());

			auto* llhz = new wxBoxSizer(wxHORIZONTAL);
			llhz->AddSpacer(20);
			llhz->Add(pSpotlight, 1, wxALL|wxEXPAND);
			llhz->AddSpacer(20);

			sizerList->AddSpacer(10);
			sizerList->Add(llhz, 1, wxALL|wxEXPAND);
			sizerList->AddSpacer(10);

			sizer->Add(sizerList, 1, wxALL | wxEXPAND);
			pListSizer = sizerList;
		}

		auto* panel = new Component::Panel(this);
		auto* sizerBar = new wxBoxSizer(wxHORIZONTAL);
		sizerBar->AddStretchSpacer();
		panel->SetSizer(sizerBar);
		panel->SetBackgroundColour(defaultBgColor);
		pPanel = panel;
		pPanelSizer = new wxBoxSizer(wxHORIZONTAL);

		sizerBar->Add(pPanelSizer, 0, wxALL|wxEXPAND, 8);
		sizerBar->Add(15, 5);

		pSpace = new Component::Panel(this);
		pSpace->SetSize(50, 35);
		pSpace->SetBackgroundColour(wxColour(255, 255, 255));
		sizer->Add(pSpace, 0, wxALL);
		sizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND);
		sizer->Add(panel, 0, wxALL|wxEXPAND);

		sizer->Layout();
		SetSizer(sizer);
	}


	Message::~Message()
	{
		// Clear the list here, to remove any references to wx components
		// before the vtable is invalid (just in case)
		pItemList.clear();

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		wxSizer* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}


	void Message::add(const wxString& caption, uint value, bool defaultButton, int space)
	{
		// We will use the userdata (a pointer) as a container for an int (value)
		auto* btn = Component::CreateButton(pPanel, caption, this, &Message::onButtonClick,
			reinterpret_cast<void*>(value));

		if (defaultButton)
		{
			btn->SetDefault();
			btn->SetFocus();
			pReturnStatus = value;
		}

		pPanelSizer->Add(btn, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE_VERTICAL|wxALL);
		pPanelSizer->AddSpacer(space);
	}


	void Message::onButtonClick(void* userdata)
	{
		pReturnStatus = (uint)((size_t)(userdata));
		Dispatcher::GUI::Close(this);
	}


	void Message::showModalAsync()
	{
		prepareShowModal();
		Dispatcher::GUI::ShowModal(this);
	}


	void Message::prepareShowModal()
	{
		//pListbox->clear();
		auto* sizer = GetSizer();

		if (pItemList.empty())
		{
			sizer->Show(pListSizer, false);
			pSpace->Show(true);
		}
		else
		{
			if (pSpotlight)
				pSpotlight->provider(new MessageProvider(pItemList));
			sizer->Show(pListSizer, true);
			pSpace->Show(false);
		}

		sizer->Layout();
		sizer->Fit(this);
		wxSize p = GetSize();

		if (pItemList.empty())
		{
			p.SetWidth(p.GetWidth() + 20);
			if (p.GetWidth() < 400)
				p.SetWidth(400);
			else
			{
				if (p.GetWidth() > 700)
					p.SetWidth(700);
			}
		}
		else
		{
			if (pRecommendedWidth && pRecommendedWidth > (uint) p.GetWidth())
			{
				if (pRecommendedWidth < 400)
					p.SetWidth(400);
				else
					p.SetWidth(pRecommendedWidth);
			}
			else
				p.SetWidth(p.GetWidth());
		}

		p.SetHeight(p.GetHeight() + (int) pItemList.size() * 18);
		if (p.GetHeight() > 600)
			p.SetHeight(600);

		SetSize(p);
		Centre(wxBOTH);

	}


	uint Message::showModal()
	{
		assert(GetSizer());

		prepareShowModal();
		ShowModal();
		return pReturnStatus;
	}


	void Message::appendError(const AnyString& text)
	{
		auto* item = new Component::Spotlight::IItem();
		item->caption(text);
		item->addTag("   error   ", 230, 30, 30);
		pItemList.push_back(item);
	}


	void Message::appendWarning(const AnyString& text)
	{
		auto* item = new Component::Spotlight::IItem();
		item->caption(text);
		item->addTag(" warning ", 255, 176, 79);
		pItemList.push_back(item);
	}




} // namespace Window
} // namespace Antares
