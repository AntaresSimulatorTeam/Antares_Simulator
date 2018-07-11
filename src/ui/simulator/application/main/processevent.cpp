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

#include "../main.h"
#include "../../toolbox/components/datagrid/dbgrid.h"
#include <typeinfo>



namespace Antares
{
namespace Forms
{


	// Forward command events to the current rich text control, if any
	bool ApplWnd::ProcessEvent(wxEvent& event)
	{
		if (event.IsCommandEvent() and not event.IsKindOf(CLASSINFO(wxChildFocusEvent)))
		{
			// Problem: we can get infinite recursion because the events
			// climb back up to this frame, and repeat.
			// Assume that command events don't cause another command event
			// to be called, so we can rely on inCommand not being overwritten

			static int s_eventType = 0;
			static wxWindowID s_id = 0;

			if (s_id != event.GetId() && s_eventType != event.GetEventType())
			{
				s_eventType = event.GetEventType();
				s_id = event.GetId();
				//wxWindow* focusWin = wxFindFocusDescendant(this);
				wxWindow* focusWin = FindFocus();

				if (focusWin)
				{
					// DBGrid: Copy
					if (s_id == wxID_COPY and dynamic_cast<Component::Datagrid::DBGrid*>(focusWin))
						dynamic_cast<Component::Datagrid::DBGrid*>(focusWin)->copyToClipboard();

					// Standard Component
					if (focusWin->GetEventHandler()->ProcessEvent(event))
					{
						//s_command = NULL;
						s_eventType = 0;
						s_id = 0;
						return true;
					}
				}
				s_eventType = 0;
				s_id = 0;
			}
			else
				return false;
		}
		return wxFrame::ProcessEvent(event);
	}




} // namespace Forms
} // namespace Antares

