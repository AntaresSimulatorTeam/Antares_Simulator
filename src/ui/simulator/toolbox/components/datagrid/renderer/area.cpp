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
#include "../../../../application/study.h"
#include "../../../../toolbox/components/refresh.h"


using namespace Yuni;



namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	ARendererArea::ARendererArea(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
		pControl(control),
		pArea(nullptr)
	{
		// Event: The current selected area
		if (notifier)
			notifier->onAreaChanged.connect(this, &ARendererArea::onAreaChanged);

		// Event: An area has been deletde
		OnStudyAreaDelete.connect(this, &ARendererArea::onAreaDelete);
	}


	ARendererArea::~ARendererArea()
	{
		onRefresh.clear();
		destroyBoundEvents();
	}


	void ARendererArea::onAreaChanged(Data::Area* area)
	{
		// FIXME
		if (area && !study)
			study = Data::Study::Current::Get();

		pArea = !study ? nullptr : area;
		internalAreaChanged(area);
		onRefresh();
		if (pControl)
			RefreshAllControls(pControl);
	}


	void ARendererArea::onAreaDelete(Data::Area* area)
	{
		if (area == pArea)
		{
			pArea = nullptr;
			internalAreaChanged(nullptr);
			onRefresh();
			if (pControl)
				RefreshAllControls(pControl);
		}
	}


	void ARendererArea::internalAreaChanged(Data::Area*)
	{
		// Do nothing
	}


	void ARendererArea::onStudyClosed()
	{
		onAreaChanged(nullptr);
		IRenderer::onStudyClosed();
	}


	void ARendererArea::onStudyLoaded()
	{
		IRenderer::onStudyLoaded();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

