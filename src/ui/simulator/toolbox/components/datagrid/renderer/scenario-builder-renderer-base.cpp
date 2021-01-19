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


#include "scenario-builder-renderer-base.h"
#include "../../refresh.h"


using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{	
	
	ScBuilderRendererBase::ScBuilderRendererBase() :
		pControl(nullptr),
		pArea(nullptr)
	{
	}


	ScBuilderRendererBase::~ScBuilderRendererBase()
	{
		destroyBoundEvents();
	}


	void ScBuilderRendererBase::onAreaChanged(Data::Area* area)
	{
		if (area != pArea)
		{
			pArea = area;
			onRefresh();
			if (pControl)
				pControl->Refresh();
		}
	}


	void ScBuilderRendererBase::onRulesChanged(Data::ScenarioBuilder::Rules::Ptr rules)
	{
		if (pRules != rules)
		{
			pRules = rules;
			invalidate = true;
			onRefresh();
			if (pControl)
				RefreshAllControls(pControl);
		}
	}


	bool ScBuilderRendererBase::valid() const
	{
		return !(!study) && pRules && study->areas.size() != 0 && !(!pRules);
	}


	int ScBuilderRendererBase::width() const
	{
		return (!study || !pRules) ? 0 : (int) study->parameters.nbYears;
	}


	int ScBuilderRendererBase::height() const
	{
		if (!(!study) && !(!pRules))
			return (int) pRules->areaCount();
		return 0;
	}

	wxString ScBuilderRendererBase::columnCaption(int x) const
	{
		return wxString(wxT("year ")) << (1 + x);
	}


	wxString ScBuilderRendererBase::rowCaption(int rowIndx) const
	{
		if (!(!study) && !(!pRules))
		{
			if ((uint)rowIndx < study->areas.size())
				return wxString() << wxT(" ") << wxStringFromUTF8(study->areas.byIndex[rowIndx]->name) << wxT("  ");
		}
		return wxEmptyString;
	}


	wxString ScBuilderRendererBase::cellValue(int x, int y) const
	{
		const double d = cellNumericValue(x, y);
		return (Math::Zero(d))
			? wxString() << wxT("rand")
			: wxString() << (uint) d;
	}


	IRenderer::CellStyle ScBuilderRendererBase::cellStyle(int x, int y) const
	{
		double blabla = cellNumericValue(x, y);
		double blibli = Math::Zero(cellNumericValue(x, y));
		bool valid = (!(!study) && !(!pRules) && Math::Zero(cellNumericValue(x, y)));
		if (valid)
		{
			auto& parameters = study->parameters;
			if (parameters.userPlaylist && parameters.yearsFilter)
				valid = !parameters.yearsFilter[x];
		}
		return (valid)
			? ((y % 2) ? cellStyleDefaultCenterAlternateDisabled : cellStyleDefaultCenterDisabled)
			: ((y % 2) ? cellStyleDefaultCenterAlternate : cellStyleDefaultCenter);
	}



	void ScBuilderRendererBase::onStudyClosed()
	{
		pArea = nullptr;
		pRules = nullptr;
		IRenderer::onStudyClosed();
	}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

