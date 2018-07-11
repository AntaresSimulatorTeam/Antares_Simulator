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

#include "bindingconstraint.h"
#include "../../../../application/study.h"
#include "../item/bindingconstraint.h"
#include "../item/group.h"
#include "../component.h"
#include <map>
#include <list>
#include "../../../../application/main.h"



namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
namespace BindingConstraints
{

	typedef Data::BindingConstraint  BindingConstraintFromLib;
	typedef std::list<BindingConstraintFromLib*> BindingConstraintList;


	namespace // anonymous
	{

		struct SortAlphaOrder
		{
			inline bool operator() (const Data::BindingConstraint* a, const Data::BindingConstraint* b)
			{
				return a->name() < b->name();
			}
		};

		struct SortAlphaReverseOrder
		{
			inline bool operator() (const Data::BindingConstraint* a, const Data::BindingConstraint* b)
			{
				return a->name() > b->name();
			}
		};


		void GetBindingConstraintList(Data::Study& study, BindingConstraintList& l, const wxString& search)
		{
			Data::BindConstList::iterator end = study.bindingConstraints.end();
			for (Data::BindConstList::iterator i = study.bindingConstraints.begin(); i != end; ++i)
			{
				if (search.empty())
					l.push_back(*i);
			}
		}

	} // anonymous namespace





	ByAlphaOrder::ByAlphaOrder(HTMLListbox::Component& parent)
		:IDatasource(parent)
	{
	}

	//! Destructor
	ByAlphaOrder::~ByAlphaOrder()
	{
		destroyBoundEvents();
	}


	void ByAlphaOrder::refresh(const wxString& search)
	{
		pParent.clear();

		if (Data::Study::Current::Valid())
		{
			BindingConstraintList l;
			GetBindingConstraintList(*Data::Study::Current::Get(), l, search);
			if (!l.empty())
			{
				l.sort(SortAlphaOrder());
				// Added the area as a result
				auto jend = l.end();
				for (auto j = l.begin(); j != jend; ++j)
					pParent.add(new Antares::Component::HTMLListbox::Item::BindingConstraint(*j));
			}
		}
		pParent.invalidate();
	}






	ByAlphaReverseOrder::ByAlphaReverseOrder(HTMLListbox::Component& parent)
		:IDatasource(parent)
	{
	}

	//! Destructor
	ByAlphaReverseOrder::~ByAlphaReverseOrder()
	{
		destroyBoundEvents();
	}

	void ByAlphaReverseOrder::refresh(const wxString& search)
	{
		pParent.clear();

		if (Data::Study::Current::Valid())
		{
			BindingConstraintList l;
			GetBindingConstraintList(*Data::Study::Current::Get(), l, search);
			if (!l.empty())
			{
				l.sort(SortAlphaReverseOrder());
				// Added the area as a result
				BindingConstraintList::iterator jend = l.end();
				for (BindingConstraintList::iterator j = l.begin(); j != jend; ++j)
					pParent.add(new Antares::Component::HTMLListbox::Item::BindingConstraint(*j));
			}
		}
		pParent.invalidate();
	}






} // namespace BindingConstraints
} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

