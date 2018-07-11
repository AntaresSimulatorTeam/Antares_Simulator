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

#include "create.h"
#include <wx/statline.h>
#include <ui/common/component/panel.h>
#include <antares/logs.h>
#include <wx/stattext.h>


namespace Antares
{
namespace Private
{
namespace Component
{


	CustomWxButton::CustomWxButton(wxWindow* parent, const wxString& title)
		:wxButton(parent, wxID_ANY, title, wxDefaultPosition, wxSize(-1, 22), wxBU_EXACTFIT)
	{
		Connect(GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCommandEventFunction, &CustomWxButton::evtOnUserClick),
			NULL, this);
	}


	void CustomWxButton::evtOnUserClick(wxCommandEvent&)
	{
		Antares::Dispatcher::GUI::Post(onUserClick);
	}




} // namespace Component
} // namespace Private
} // namespace Antares





namespace Antares
{
namespace Component
{

	namespace // anonymous
	{

		enum
		{
			fontSize = 8,
		};
		static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
		

		class FlickerFreeLabel final : public wxStaticText
		{
		public:
			FlickerFreeLabel(wxWindow* parent,
					int id, const wxString& label,
					const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					long style = 0, const wxString& name = wxT("staticText"))
				:wxStaticText(parent,id,label,pos,size,style,name)
			{
				SetFont(font);
			}

			virtual ~FlickerFreeLabel()
			{}

			void OnEraseBackGround(wxEraseEvent&)
			{
				// do nothing
			}

			void OnMotion(wxMouseEvent&)
			{
				Panel::OnMouseMoveFromExternalComponent();
			}


			DECLARE_EVENT_TABLE()
		};

		BEGIN_EVENT_TABLE(FlickerFreeLabel,wxStaticText)
			EVT_ERASE_BACKGROUND(FlickerFreeLabel::OnEraseBackGround)
			EVT_MOTION(FlickerFreeLabel::OnMotion)
		END_EVENT_TABLE()



		class SeparatorPanel final : public Antares::Component::Panel
		{
		public:
			SeparatorPanel(wxWindow* parent, uint size = 1)
				:Antares::Component::Panel(parent)
			{
				SetBackgroundColour(wxColour(190, 190, 190));
				SetSize(size, 1);
			}

			virtual ~SeparatorPanel()
			{}

		}; // class SeparatorPanel

	} // anonymous namespace




	wxWindow* AddVerticalSeparator(wxWindow* parent, wxSizer* sizer, uint size, uint border)
	{
		enum
		{
			margin = 4,
		};
		wxWindow* obj = new SeparatorPanel(parent, size);
		if (sizer)
		{
			sizer->AddSpacer(margin);
			sizer->Add(obj, 0, wxALL|wxEXPAND, border);
			sizer->SetItemMinSize(obj, size, 1);
			sizer->AddSpacer(margin);
		}
		return obj;
	}


	wxWindow* AddHorizontalSeparator(wxWindow* parent, wxSizer* sizer, bool spacing)
	{
		enum
		{
			margin = 4,
			border = 4
		};
		wxWindow* obj = new SeparatorPanel(parent);
		if (sizer)
		{
			if (spacing)
			{
				sizer->AddSpacer(margin);
				sizer->Add(obj, 0, wxLEFT|wxRIGHT|wxEXPAND, border);
				sizer->SetItemMinSize(obj, 1, 1);
				sizer->AddSpacer(margin);
			}
			else
			{
				sizer->Add(obj, 0, wxLEFT|wxRIGHT|wxEXPAND, border);
				sizer->SetItemMinSize(obj, 1, 1);
			}
		}
		return obj;
	}


	wxStaticText* CreateLabel(wxWindow* parent, const wxString& caption)
	{
		return new FlickerFreeLabel(parent, wxID_ANY, caption);
	}


	wxStaticText* CreateLabel(wxWindow* parent, const wxString& caption, bool bold, bool grey)
	{
		wxStaticText* lbl = new FlickerFreeLabel(parent, wxID_ANY, caption);

		if (bold)
		{
			wxFont f = lbl->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			lbl->SetFont(f);
		}
		if (grey)
			lbl->SetForegroundColour(wxColour(150, 150, 150));
		return lbl;
	}


	wxStaticText* CreateLabel(wxWindow* parent, const wxString& caption, bool bold, bool grey, int size)
	{
		wxStaticText* lbl = new FlickerFreeLabel(parent, wxID_ANY, caption);

		if (bold)
		{
			wxFont f = lbl->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			f.SetPointSize(f.GetPointSize() + size);
			lbl->SetFont(f);
		}
		else
		{
			if (size)
			{
				wxFont f = lbl->GetFont();
				f.SetPointSize(f.GetPointSize() + size);
				lbl->SetFont(f);
			}
		}
		if (grey)
			lbl->SetForegroundColour(wxColour(150, 150, 150));
		return lbl;
	}





} // namespace Component
} // namespace Antares
