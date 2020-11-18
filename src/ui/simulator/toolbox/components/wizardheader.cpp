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
#include "wizardheader.h"
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include "../resources.h"
#include "../create.h"
#include <wx/dcbuffer.h>
#include <ui/common/component/panel.h>




namespace Antares
{
namespace Toolbox
{
namespace Components
{

	BEGIN_EVENT_TABLE(WizardHeader, Panel)
		EVT_PAINT(WizardHeader::onDraw)
		EVT_ERASE_BACKGROUND(WizardHeader::onEraseBackground)
	END_EVENT_TABLE()



	namespace // anonymous
	{
		// The font re-used for each drawing
		enum
		{
			fontSize = 8,
		};
		const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
		const wxFont fontSmall(wxFontInfo(fontSize-1).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

	} // anonymous namespace




	WizardHeader::WizardHeader(wxWindow* parent, const char* icon, const wxString& text, const wxString& sub)
		:Antares::Component::Panel(parent),
		pText(text), pSubText(sub)
	{
		SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by Windows
		SetSize(100, 100);
		pIcon = Resources::BitmapLoadFromFile(icon);
	}


	WizardHeader::~WizardHeader()
	{
		delete pIcon;

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		wxSizer* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}



	void WizardHeader::onDraw(wxPaintEvent&)
	{
		// The DC
		wxClientDC dc(this);
		// Shifts the device origin so we don't have to worry
		// about the current scroll position ourselves
		PrepareDC(dc);

		// Drawing the background
		wxRect rect = this->GetRect();
		rect.x = 0;
		rect.y = 0;
		dc.GradientFillLinear(rect,
			wxColour(30, 30, 30), wxColour(100, 100, 100), wxSOUTH);
		// Drawing the border
		dc.SetPen(wxPen(wxColour(20,20,20), 1));
		dc.DrawLine(0, rect.GetHeight() - 1, rect.GetWidth(), rect.GetHeight() - 1);

		// Cute font
		wxFont f = font;
		f.SetWeight(wxFONTWEIGHT_BOLD);
		f.SetPointSize(f.GetPointSize() + 2);
		dc.SetFont(f);

		// Drawing the text
		wxSize size = dc.GetTextExtent(pText);
		// offset
		int offset = rect.height >> 1;
		if (!pSubText.IsEmpty())
			offset -= ((size.GetHeight()) >> 1);
		sizeNeeded = (size.GetHeight() >> 1);
		offset -= sizeNeeded;

		++offset; // for beauty (alignment)

		// -- Caption --
		// Shadow
		dc.SetTextForeground(wxColour(30,30,30));
		dc.DrawText(pText, 71, offset + 2);
		dc.DrawText(pText, 72, offset + 1);
		dc.SetTextForeground(wxColour(0,0,0));
		dc.DrawText(pText, 71, offset + 1);
		// White
		dc.SetTextForeground(wxColour(250,250, 250));
		dc.DrawText(pText, 70, offset);

		// Recalculating the offset
		offset += size.GetHeight();

		if (!pSubText.IsEmpty())
		{
			// Cute font
			f.SetWeight(wxFONTWEIGHT_NORMAL);
			f.SetPointSize(f.GetPointSize() - 3);
			dc.SetFont(f);

			size = dc.GetTextExtent(pSubText);

			// -- Caption --
			// Shadow
			dc.SetTextForeground(wxColour(0,0,0));
			dc.DrawText(pSubText, 71, offset + 1);
			// White
			dc.SetTextForeground(wxColour(190,190, 190));
			dc.DrawText(pSubText, 70, offset);
		}

		// -- Icon --
		if (pIcon)
			dc.DrawBitmap(*pIcon, 25, (rect.GetHeight() >> 1) - (pIcon->GetHeight() >> 1), true);

		if (sizeNeeded < 40)
			sizeNeeded = 40;
		sizeNeeded += size.GetHeight();
	}





	Antares::Component::Panel*
	WizardHeader::Create(wxWindow* parent, const wxString& caption,
		const char* img, const wxString& help, const int wrap, bool customDraw)
	{
		Antares::Component::Panel* p;
		if (customDraw)
		{
			WizardHeader* pnl = new WizardHeader(parent, img, caption, help);
			p = pnl;
			wxPaintEvent evt(pnl->GetId());
			pnl->onDraw(evt);
			p->SetSize(100, pnl->sizeNeeded);
		}
		else
		{
			p = new Antares::Component::Panel(parent);
			wxBoxSizer* s = new wxBoxSizer(wxHORIZONTAL);
			p->SetSizer(s);

			// Space
			s->AddSpacer(14);

			// The Image
			s->Add(Resources::StaticBitmapLoadFromFile(p, wxID_ANY, img), 0, wxALL|wxALIGN_CENTER, 10);

			// The Text
			if (help.IsEmpty())
			{
				// The caption
				wxWindow* t = Antares::Component::CreateLabel(p, caption, true, false, +2);
				s->Add(t, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
			}
			else
			{
				// Sub Panel
				wxBoxSizer* subP = new wxBoxSizer(wxVERTICAL);

				// The caption
				wxWindow* t = Antares::Component::CreateLabel(p, caption, true, false, +2);
				subP->AddSpacer(5);
				subP->Add(t, 0, wxLEFT|wxALL);

				// Help
				wxStaticText* stH = Antares::Component::CreateLabel(p, help);
				stH->SetForegroundColour(wxColour(60, 60, 60));
				stH->Wrap(wrap - 40);
				subP->AddSpacer(1);
				subP->Add(stH, 1, wxLEFT);
				subP->AddSpacer(5);

				s->Add(subP, 1, wxALL|wxEXPAND, 8);
			}
			s->AddSpacer(10);
			s->Layout();
		}

		p->Layout();
		return p;
	}





} // namespace Antares
} // namespace Toolbox
} // namespace Antares

