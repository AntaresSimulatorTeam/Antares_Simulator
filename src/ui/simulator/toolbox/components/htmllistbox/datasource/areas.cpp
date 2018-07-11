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

# include "areas.h"
# include "../../../../application/study.h"
# include "../item/area.h"
# include "../item/group.h"
# include "../component.h"


namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
namespace Areas
{



	struct SortAlphaOrder
	{
		inline bool operator() (const Data::Area* a, const Data::Area* b) const
		{
			return a->id < b->id;
		}
	};

	struct SortAlphaReverseOrder
	{
		inline bool operator() (const Data::Area* a, const Data::Area* b) const
		{
			return a->id > b->id;
		}
	};

	struct SortColor
	{
		inline bool operator() (const Data::Area* a, const Data::Area* b) const
		{
			// We must have strict weak ordering
			return (a->ui->cacheColorHSV == b->ui->cacheColorHSV)
				? (a->id < b->id)
				: (a->ui->cacheColorHSV) > (b->ui->cacheColorHSV);
		}
	};



	static void GetAreaList(Data::Area::List& l, const wxString& search)
	{
		auto study = Data::Study::Current::Get();
		if (!(!study) && !study->areas.areas.empty())
		{
			// Adding each area
			const Data::Area::Map::iterator end = study->areas.end();
			for (Data::Area::Map::iterator i = study->areas.begin(); i != end; ++i)
			{
				if (search.empty())
					l.push_back(i->second);
			}
		}
	}



	ByAlphaOrder::ByAlphaOrder(HTMLListbox::Component& parent)
		:IDatasource(parent)
	{}
	//! Destructor
	ByAlphaOrder::~ByAlphaOrder()
	{}

	void ByAlphaOrder::refresh(const wxString& search)
	{
		pParent.clear();

		Data::Area::List l;
		GetAreaList(l, search);
		if (!l.empty())
		{
			l.sort(SortAlphaOrder());
			const Data::Area::List::const_iterator end = l.end();
			pParent.add(new Antares::Component::HTMLListbox::Item::Group(wxT("A-Z")));
			wxString add;
			char lastC = '\0';
			for (Data::Area::List::const_iterator i = l.begin(); i != end; ++i)
			{
				add.Clear();
				if (lastC != toupper((*i)->name[0]))
				{
					lastC = toupper((*i)->name[0]);
					add << wxT("<td width=20><b><font color=\"#CCCCDD\" size=\"-1\">")
						<< wxString::FromAscii(lastC)
						<< wxT("</font></b></td>");
				}
				// Added the area as a result
				pParent.add(new Antares::Component::HTMLListbox::Item::Area(*i, add));
			}
		}
		pParent.invalidate();
	}



	ByAlphaReverseOrder::ByAlphaReverseOrder(HTMLListbox::Component& parent)
		:IDatasource(parent)
	{}
	//! Destructor
	ByAlphaReverseOrder::~ByAlphaReverseOrder()
	{}

	void ByAlphaReverseOrder::refresh(const wxString& search)
	{
		pParent.clear();

		Data::Area::List l;
		GetAreaList(l, search);
		if (!l.empty())
		{
			l.sort(SortAlphaReverseOrder());
			const Data::Area::List::const_iterator end = l.end();
			pParent.add(new Antares::Component::HTMLListbox::Item::Group(wxT("Z-A")));
			wxString add;
			char lastC = '\0';
			for (Data::Area::List::const_iterator i = l.begin(); i != end; ++i)
			{
				add.Clear();
				if (lastC != toupper((*i)->name[0]))
				{
					lastC = toupper((*i)->name[0]);
					add << wxT("<td width=20><b><font color=\"#CCCCDD\" size=\"-1\">")
						<< wxString::FromAscii(lastC)
						<< wxT("</font></b></td>");
				}
				// Added the area as a result
				pParent.add(new Antares::Component::HTMLListbox::Item::Area(*i, add));
			}
		}
		pParent.invalidate();
	}



	ByColor::ByColor(HTMLListbox::Component& parent)
		:IDatasource(parent)
	{}

	//! Destructor
	ByColor::~ByColor()
	{}



	void ByColor::refresh(const wxString& search)
	{
		pParent.clear();

		Data::Area::List l;
		GetAreaList(l, search);
		if (!l.empty())
		{
			l.sort(SortColor());
			{
				const Data::Area::List::iterator end = l.end();
				for (Data::Area::List::iterator i = l.begin(); i != end; ++i)
				{
					// Added the area as a result
					pParent.add(new Antares::Component::HTMLListbox::Item::Area(*i));
				}
			}
		}
		pParent.invalidate();
	}




} // namespace Areas
} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

