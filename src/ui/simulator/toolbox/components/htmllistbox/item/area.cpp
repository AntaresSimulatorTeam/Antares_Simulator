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

#include "area.h"
#include <wx/colour.h>


namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{


	Area::Area(Data::Area* a)
		:pArea(a)
	{}

	Area::Area(Data::Area* a, const wxString& additional)
		:pArea(a), pText(additional)
	{}



	Area::~Area()
	{}


	bool Area::HtmlContent(wxString& out, Data::Area* area, const wxString& searchString)
	{
		bool highlight = false;
		out	<< wxT("<td width=5></td><td width=16 bgcolor=\"")
			<< wxColour(area->ui->color[0], area->ui->color[1], area->ui->color[2]).GetAsString(wxC2S_HTML_SYNTAX)
			<< wxT("\" align=center><font size=\"-3\" color=\"")
			<< ColorDarker(area->ui->color[0], area->ui->color[1], area->ui->color[2]).GetAsString(wxC2S_HTML_SYNTAX)
			<< wxT("\">Ar</font></td><td width=8></td><td nowrap><font size=\"-1\"");
		wxString name = wxStringFromUTF8(area->name);
		if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
			out << wxT(">") << name << wxT("</font>");
		else
			out << wxT(" color=\"#999999\">") << name << wxT("</font>");
		// Post
		out << wxT("</td>");
		return highlight;
	}


	wxString Area::htmlContent(const wxString& searchString)
	{
		if (pArea)
		{
			wxString d;
			d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
			pHighlighted = HtmlContent(d, pArea, searchString);
			// Post
			d << pText << wxT("</tr></table>");
			return d;
		}
		pHighlighted = false;
		return wxEmptyString;
	}





} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
