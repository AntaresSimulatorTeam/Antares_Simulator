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

#include "progressbar.h"
#include <wx/dcbuffer.h>


namespace Antares
{
namespace Component
{

	BEGIN_EVENT_TABLE(ProgressBar, Panel)
		EVT_PAINT(ProgressBar::onDraw)
		EVT_SIZE(ProgressBar::onResize)
	END_EVENT_TABLE()




	ProgressBar::ProgressBar(wxWindow* parent)
		:Panel(parent),
		pValue(0.)
	{
		SetSize(100, 14);
		SetMinSize(wxSize(100, 14));
		SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by Windows
	}

	ProgressBar::~ProgressBar()
	{}



	void ProgressBar::onDraw(wxPaintEvent& evt)
	{
		// The DC
		wxAutoBufferedPaintDC dc(this);
		// Shifts the device origin so we don't have to worry
		// about the current scroll position ourselves
		PrepareDC(dc);

		const wxColour background (213, 216, 219);
		const wxColour highlight  (158, 164, 169);
		const wxColour highlight2 (148, 154, 159);
		const wxColour highlight3 (148, 144, 149);

		// Drawing the background
		const wxRect pRect = this->GetClientSize();
		dc.SetPen(wxPen(highlight, 1));
		dc.SetBrush(wxBrush(background));
		dc.DrawRectangle(0, 0, pRect.width, pRect.height);

		if (pValue > 0.)
		{
			dc.SetBrush(wxBrush(highlight));
			const int w = (int)((pRect.width - 5) * (pValue / 100.)) + 1;
			const int h = pRect.height - 4;

			dc.DrawRectangle(2, 2, w, h);

			dc.SetPen(wxPen(highlight3, 1));
			dc.DrawLine(2, 2 + h - 1, w + 2, 2 + h - 1);
			dc.SetPen(wxPen(highlight2, 1));
			dc.DrawLine(2, 2 + h - 2, w + 2, 2 + h - 2);

			// Drawing the Caption in the middle of the panel
			// Cute font
			enum
			{ fontSize = 7, };
			static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
			dc.SetFont(font);

			wxString text;
			text.Printf(wxT("%.2f%%"), pValue);
			const wxSize textExtent = dc.GetTextExtent(text);
			const wxPoint p(
				//pRect.width  / 2 - textExtent.GetWidth() / 2,
				12,
				pRect.height / 2 - textExtent.GetHeight() / 2 );
			dc.SetTextForeground(wxColour(70, 50, 50));
			dc.DrawText(text, p.x, p.y);
		}

		evt.Skip();
	}


	void ProgressBar::onResize(wxSizeEvent& evt)
	{
		this->Refresh();
		evt.Skip();
	}



} // namespace Component
} // namespace Antares

