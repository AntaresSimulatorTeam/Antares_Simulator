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
#include <yuni/core/math.h>
#include <antares/study/scenario-builder/sets.h>
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


	ScenarioBuilder::ScenarioBuilder(Toolbox::InputSelector::Area* notifier) :
		pControl(nullptr),
		pTimeseries(Data::timeSeriesThermal),
		pArea(nullptr)
	{
		if (notifier)
		{
			// Event: The current selected area
			notifier->onAreaChanged.connect(this, &ScenarioBuilder::onAreaChanged);
		}
	}


	ScenarioBuilder::ScenarioBuilder(Data::TimeSeries ts) :
		pControl(nullptr),
		pTimeseries(ts),
		pArea(nullptr)
	{
	}


	ScenarioBuilder::~ScenarioBuilder()
	{
		destroyBoundEvents();
	}


	void ScenarioBuilder::onAreaChanged(Data::Area* area)
	{
		if (area != pArea)
		{
			pArea = area;
			onRefresh();
			if (pControl)
				pControl->Refresh();
		}
	}


	void ScenarioBuilder::onRulesChanged(Data::ScenarioBuilder::Rules::Ptr rules)
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


	bool ScenarioBuilder::valid() const
	{
		return !(!study) && pRules && study->areas.size() != 0
			&& !(!pRules) && (pTimeseries != Data::timeSeriesThermal || pArea);
	}


	int ScenarioBuilder::width() const
	{
		return (!study || !pRules) ? 0 : (int) study->parameters.nbYears;
	}


	int ScenarioBuilder::height() const
	{
		if (!(!study) && !(!pRules))
		{
			if (pTimeseries == Data::timeSeriesThermal)
			{
				if (pArea)
					return (int) pArea->thermal.list.size();
			}
			else
				return (int) pRules->areaCount();
		}
		return 0;
	}


	wxString ScenarioBuilder::columnCaption(int x) const
	{
		return wxString(wxT("year ")) << (1 + x);
	}


	wxString ScenarioBuilder::rowCaption(int rowIndx) const
	{
		if (!(!study) && !(!pRules))
		{
			if (pTimeseries == Data::timeSeriesThermal)
			{
				if (pArea && (uint) rowIndx < pArea->thermal.list.size())
				{
					return wxString() << wxT(" ") << wxStringFromUTF8(pArea->thermal.list.byIndex[rowIndx]->name())
						<< wxT("  ");
				}

			}
			else
			{
				if ((uint)rowIndx < study->areas.size())
				{
					return wxString() << wxT(" ") << wxStringFromUTF8(study->areas.byIndex[rowIndx]->name)
						<< wxT("  ");
				}
			}
		}
		return wxEmptyString;
	}


	bool ScenarioBuilder::cellValue(int x, int y, const String& value)
	{
		if (!(!study) && !(!pRules) && (uint) x < study->parameters.nbYears)
		{
			double dbl;
			uint d;
			if (!value.to(dbl))
				d = 0;
			else
			{
				if (dbl < 0.)
					d = 0;
				else
				{
					if (dbl > 10000.)
						d = 10000;
					else
						d = (uint) dbl;
				}
			}

			if (pTimeseries == Data::timeSeriesThermal)
			{
				if (pArea && (uint) y < pArea->thermal.list.size())
				{
					assert(pArea->index < pRules->areaCount());
					assert((uint) y < pRules->thermal[pArea->index].overlay().width);
					assert((uint) x < pRules->thermal[pArea->index].overlay().height);
					pRules->thermal[pArea->index].overlay().entry[y][x] = d;
					return true;
				}
			}
			else
			{
				if ((uint) y < study->areas.size())
				{
					switch (pTimeseries)
					{
						case Data::timeSeriesLoad:
							assert((uint) y < pRules->load.overlay().width);
							assert((uint) x < pRules->load.overlay().height);
							pRules->load.overlay().entry[y][x] = d;
							return true;
						case Data::timeSeriesHydro:
							assert((uint) y < pRules->hydro.overlay().width);
							assert((uint) x < pRules->hydro.overlay().height);
							pRules->hydro.overlay().entry[y][x] = d;
							return true;
						case Data::timeSeriesWind:
							assert((uint) y < pRules->wind.overlay().width);
							assert((uint) x < pRules->wind.overlay().height);
							pRules->wind.overlay().entry[y][x] = d;
							return true;
						case Data::timeSeriesSolar:
							assert((uint) y < pRules->solar.overlay().width);
							assert((uint) x < pRules->solar.overlay().height);
							pRules->solar.overlay().entry[y][x] = d;
							return true;
						default:
							return false;
					}
				}
			}
		}
		return false;
	}


	double ScenarioBuilder::cellNumericValue(int x, int y) const
	{
		if (!(!study) && !(!pRules) && (uint) x < study->parameters.nbYears)
		{
			if (pTimeseries == Data::timeSeriesThermal)
			{
				if (pArea && (uint) y < pArea->thermal.list.size())
				{
					assert((uint) y < pRules->thermal[pArea->index].overlay().width);
					assert((uint) x < pRules->thermal[pArea->index].overlay().height);
					return pRules->thermal[pArea->index].overlay().entry[y][x];
				}
			}
			else
			{
				if ((uint) y < study->areas.size())
				{
					switch (pTimeseries)
					{
						case Data::timeSeriesLoad:
							assert((uint) y < pRules->load.overlay().width);
							assert((uint) x < pRules->load.overlay().height);
							return (pRules->load.overlay().entry[y][x]);
						case Data::timeSeriesHydro:
							assert((uint) y < pRules->hydro.overlay().width);
							assert((uint) x < pRules->hydro.overlay().height);
							return (pRules->hydro.overlay().entry[y][x]);
						case Data::timeSeriesWind:
							assert((uint) y < pRules->wind.overlay().width);
							assert((uint) x < pRules->wind.overlay().height);
							return (pRules->wind.overlay().entry[y][x]);
						case Data::timeSeriesSolar:
							assert((uint) y < pRules->solar.overlay().width);
							assert((uint) x < pRules->solar.overlay().height);
							return (pRules->solar.overlay().entry[y][x]);
						default:
							return 0.;
					}
				}
			}
		}
		return 0.;
	}


	wxString ScenarioBuilder::cellValue(int x, int y) const
	{
		const double d = cellNumericValue(x, y);
		return (Math::Zero(d))
			? wxString() << wxT("rand")
			: wxString() << (uint) d;
	}


	IRenderer::CellStyle ScenarioBuilder::cellStyle(int x, int y) const
	{
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



	void ScenarioBuilder::onStudyClosed()
	{
		pArea = nullptr;
		pRules = nullptr;
		IRenderer::onStudyClosed();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

