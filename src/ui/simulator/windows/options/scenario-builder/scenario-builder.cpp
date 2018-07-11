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

#include "scenario-builder.h"
#include <yuni/io/file.h>

#include <wx/panel.h>
#include <wx/sizer.h>

using namespace Yuni;

# define SEP  Yuni::IO::Separator



namespace Antares
{
namespace Window
{
namespace Options
{


	MCScenarioBuilder::MCScenarioBuilder(wxFrame* parent) :
		wxFrame(parent, wxID_ANY, wxT("MC Scenario Builder"),
			wxDefaultPosition, wxSize(900, 500),
			wxCAPTION|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER|wxCLIP_CHILDREN)
	{
	}


	MCScenarioBuilder::~MCScenarioBuilder()
	{
	}




} // namespace Options
} // namespace Window
} // namespace Antares
