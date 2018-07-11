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

#include "main.h"
#include "internal-data.h"

// Map
#include "../../toolbox/components/map/component.h"
// inspector
#include "../../windows/inspector.h"
#include <ui/common/lock.h>

using namespace Yuni;



namespace Antares
{
namespace Forms
{

	void ApplWnd::evtOnViewAllSystem(wxCommandEvent&)
	{
		pSectionNotebook->select(wxT("input"));
		pNotebook->select(wxT("sys"));
		pMainMap->showLayerAll();
	}

	void ApplWnd::copyToClipboard()
	{
		// Copy to the clipboard items present in the inspector
		uint count = Window::Inspector::CopyToClipboard();

		// Keeping the user informed about the operation
		switch (count)
		{
			case 0:
				SetStatusText(wxT("  Nothing has been copied"));
				break;
			case 1:
				SetStatusText(wxT("  1 item has been copied"));
				break;
			default:
				SetStatusText(wxString(wxT("  ")) << count << wxT(" items have been copied"));
		}
		// paste: applicate/main/paste-from-clipboard.cpp
	}


	void ApplWnd::evtOnEditCopy(wxCommandEvent&)
	{
		if (GUIIsLock())
			return;
		copyToClipboard();
		// paste: applicate/main/paste-from-clipboard.cpp
	}


	void ApplWnd::evtOnEditMapSelectAll(wxCommandEvent&)
	{
		if (GUIIsLock() || !pMainMap)
			return;
		pMainMap->selectAll();
	}


	void ApplWnd::evtOnEditMapUnselectAll(wxCommandEvent&)
	{
		if (GUIIsLock())
			return;
		if (!pMainMap)
			return;
		pMainMap->unselectAll();
	}


	void ApplWnd::evtOnEditMapReverseSelection(wxCommandEvent&)
	{
		if (GUIIsLock())
			return;
		if (!pMainMap)
			return;
		pMainMap->reverseSelection();
	}





} // namespace Forms
} // namespace Antares
