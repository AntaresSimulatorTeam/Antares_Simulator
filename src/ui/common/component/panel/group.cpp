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

#include "group.h"
#include <wx/sizer.h>
#include <yuni/core/math.h>

using namespace Yuni;



namespace Antares
{
namespace Component
{

	void PanelGroup::SetDarkBackgroundColor(wxWindow* ctrl, int lightModifier)
	{
		wxColour def = ctrl->GetBackgroundColour();
		int r = Math::MinMax<int>(def.Red()   - 100 + lightModifier, 0, 255);
		int g = Math::MinMax<int>(def.Green() - 90 + lightModifier, 0, 255);
		int b = Math::MinMax<int>(def.Blue()  - 70 + lightModifier, 0, 255);
		ctrl->SetBackgroundColour(wxColour(r, g, b));
	}


	void PanelGroup::SetLighterBackgroundColor(wxWindow* ctrl, int lightModifier)
	{
		wxColour def = ctrl->GetBackgroundColour();
		int r = Math::MinMax<int>(def.Red()   + 40 + lightModifier, 0, 255);
		int g = Math::MinMax<int>(def.Green() + 40 + lightModifier, 0, 255);
		int b = Math::MinMax<int>(def.Blue()  + 40 + lightModifier, 0, 255);
		ctrl->SetBackgroundColour(wxColour(r, g, b));
	}


	PanelGroup::PanelGroup(wxWindow* parent, const char* /*image*/) :
		Panel(parent)
	{
		assert(parent);

		auto* hz = new wxBoxSizer(wxHORIZONTAL);
		subpanel = new Panel(this);
		hz->AddSpacer(3);
		leftSizer = new wxBoxSizer(wxHORIZONTAL);
		hz->Add(leftSizer, 0, wxALL|wxEXPAND, 1);
		hz->AddSpacer(3);
		hz->Add(subpanel, 1, wxALL|wxEXPAND, 1);
		SetSizer(hz);

		wxColour def = GetBackgroundColour();
		{
			int r = Math::MinMax<int>(def.Red()   - 80, 0, 255);
			int g = Math::MinMax<int>(def.Green() - 70, 0, 255);
			int b = Math::MinMax<int>(def.Blue()  - 50, 0, 255);
			SetBackgroundColour(wxColour(r, g, b));
		}
		{
			int r = Math::MinMax<int>(def.Red()   - 5, 0, 255);
			int g = Math::MinMax<int>(def.Green() - 5, 0, 255);
			int b = Math::MinMax<int>(def.Blue()  - 0, 0, 255);
			subpanel->SetBackgroundColour(wxColour(r, g, b));
		}

		hz = new wxBoxSizer(wxHORIZONTAL);
		hz->AddSpacer(1);
		subpanel->SetSizer(hz);
	}


	PanelGroup::~PanelGroup()
	{
	}




} // namespace Component
} // namespace Antares

