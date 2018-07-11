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

#include "refresh.h"
#include "datagrid/component.h"
#include "notebook/notebook.h"
#include "../../application/study.h"
#include "../../../common/lock.h"


namespace Antares
{


	static void RefreshAllControlsRecursive(wxWindow* comObj)
	{
		auto* com = dynamic_cast<Component::Datagrid::Component*>(comObj);
		if (com)
		{
			if (com->IsShown())
			{
				com->InvalidateBestSize();
				com->ClearBackground();
				com->forceRefresh();
			}
		}
		else
		{
			auto* nbk = dynamic_cast<Component::Notebook*>(comObj);
			if (nbk)
			{
				auto* page = nbk->selected();
				if (page)
					RefreshAllControls(page->control());
			}
			else
			{
				const wxWindowList& list = comObj->GetChildren();
				const wxWindowList::const_iterator end = list.end();
				for (wxWindowList::const_iterator i = list.begin(); i != end; ++i)
					RefreshAllControlsRecursive(*i);
				comObj->Refresh();
			}
		}
	}


	static void EnableRefreshForAllDatagridRecursive(wxWindow* comObj, bool enabled)
	{
		auto* com = dynamic_cast<Component::Datagrid::Component*>(comObj);
		if (com)
		{
			com->enableRefresh(enabled);
		}
		else
		{
			auto* nbk = dynamic_cast<Component::Notebook*>(comObj);
			if (nbk)
			{
				nbk->enableRefreshForAllDatagrid(enabled);
			}
			else
			{
				const wxWindowList& list = comObj->GetChildren();
				const wxWindowList::const_iterator end = list.end();
				for (wxWindowList::const_iterator i = list.begin(); i != end; ++i)
					EnableRefreshForAllDatagrid(*i, enabled);
			}
		}
	}




	void RefreshAllControls(wxWindow* comObj)
	{
		if (comObj && !IsGUIAboutToQuit())
			RefreshAllControlsRecursive(comObj);
	}


	void EnableRefreshForAllDatagrid(wxWindow* comObj, bool enabled)
	{
		if (comObj && !IsGUIAboutToQuit())
			EnableRefreshForAllDatagridRecursive(comObj, enabled);
	}





} // namespace Antares

