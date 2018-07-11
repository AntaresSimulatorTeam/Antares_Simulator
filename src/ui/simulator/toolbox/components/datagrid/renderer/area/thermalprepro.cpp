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

#include "thermalprepro.h"
#include <yuni/core/math.h>
#include <limits>


using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

	/*!
	** \brief Compute the MTBF for FO
	**
	** MTBF: Mean Time Between Failure
	*/
	static double MTBFForFO(Antares::Data::PreproThermal* preproThermal, int y)
	{
		if (y >= 0 and y < DAYS_PER_YEAR and preproThermal)
		{
			auto&  data     = preproThermal->data;
			double rate     = data[Antares::Data::PreproThermal::foRate][y];
			double duration = data[Antares::Data::PreproThermal::foDuration][y];

			if (Math::Zero(rate))
				return std::numeric_limits<double>::infinity();

			double mtbf = (duration / rate) - duration;
			return mtbf < 1. ? mtbf : Math::Round(mtbf, 2);
		}
		return 0.;
	}


	/*!
	** \brief Compute the MTBF for PO
	**
	** MTBF: Mean Time Between Failure
	*/
	static double MTBFForPO(Antares::Data::PreproThermal* preproThermal, int y)
	{
		if (y >= 0 and y < DAYS_PER_YEAR and preproThermal)
		{
			auto&  data     = preproThermal->data;
			double rate     = data[Antares::Data::PreproThermal::poRate][y];
			double duration = data[Antares::Data::PreproThermal::poDuration][y];

			if (Math::Zero(rate))
				return std::numeric_limits<double>::infinity();

			double mtbf = (duration / rate) - duration;
			return mtbf < 1. ? mtbf : Math::Round(mtbf, 2);
		}
		return 0.;
	}



	ThermalClusterPrepro::ThermalClusterPrepro(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier) :
		MatrixAncestorType(control),
		pPreproThermal(nullptr)
	{
		if (notifier)
			notifier->onThermalClusterChanged.connect(this, &ThermalClusterPrepro::internalThermalClusterChanged);
	}

	ThermalClusterPrepro::~ThermalClusterPrepro()
	{
		destroyBoundEvents();
	}



	wxString ThermalClusterPrepro::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
			case 0: return wxT(" FO Duration ");
			case 1: return wxT(" PO Duration ");
			case 2: return wxT("   FO Rate   ");
			case 3: return wxT("   PO Rate   ");
			case 4: return wxT("   NPO Min   ");
			case 5: return wxT("   NPO Max   ");
			case 6: return wxT("   MTBF FO   ");
			case 7: return wxT("   MTBF PO   ");
			default : return wxEmptyString;
		}
		return wxEmptyString;
	}


	wxString ThermalClusterPrepro::cellValue(int x, int y) const
	{
		const double d = cellNumericValue(x, y);
		switch (Math::Infinite(d))
		{
			case 0:
				{
					if (x > 3)
					{
						double rounded = Math::Round(d, 2);
						if (d < 1. and !(rounded < 1.)) // nearly 1.
							return wxT("~ 1");
						if (Math::Zero(rounded) and not Math::Zero(d))
							return wxT("~ 0");
						return DoubleToWxString(d);
					}
					return DoubleToWxString(d);
				}
			case 1:
				return wxT("\u221E ");
			case -1:
				return wxT("-\u221E ");
		}
		return DoubleToWxString(d);
	}


	double ThermalClusterPrepro::cellNumericValue(int x, int y) const
	{
		if (x > 5)
		{
			if (y >= 0 and y < DAYS_PER_YEAR and pPreproThermal)
			{
				switch (x)
				{
					case 6 : return MTBFForFO(pPreproThermal, y);
					case 7 : return MTBFForPO(pPreproThermal, y);
				}
			}
			return 0.;
		}
		return MatrixAncestorType::cellNumericValue(x, y);
	}


	bool ThermalClusterPrepro::cellValue(int x, int y, const String& value)
	{
		if (x < 2)
		{
			double v;
			if (not value.to(v))
				return false;
			if (v < 1)
				return MatrixAncestorType::cellValue(x, y, "1");
			if (v > 365)
				return MatrixAncestorType::cellValue(x, y, "365");

			uint duration = (uint) Math::Round(v);
			return MatrixAncestorType::cellValue(x, y, CString<64,false>() << duration);
		}
		return MatrixAncestorType::cellValue(x, y, value);
	}


	void ThermalClusterPrepro::internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster)
	{
		pCluster = cluster;
		pPreproThermal = (cluster) ? cluster->prepro : nullptr;
		MatrixAncestorType::matrix((pPreproThermal) ? &pPreproThermal->data : nullptr);
		onRefresh();
	}


	wxString ThermalClusterPrepro::rowCaption(int row) const
	{
		if (!study or row >= study->calendar.maxDaysInYear)
			return wxEmptyString;
		return wxStringFromUTF8(study->calendar.text.daysYear[row]);
	}


	IRenderer::CellStyle ThermalClusterPrepro::cellStyle(int col, int row) const
	{
		if (col > 5) // MTBF
		{
			// MTBF FO
			if (col == 6)
			{
				double mtbf = MTBFForFO(pPreproThermal, row);
				if (Math::Zero(mtbf))
					return IRenderer::cellStyleWarning;
			}
			// MTBF PO
			if (col == 7)
			{
				double mtbf = MTBFForPO(pPreproThermal, row);
				if (Math::Zero(mtbf))
					return IRenderer::cellStyleWarning;
			}

			return IRenderer::cellStyleDisabled;
		}

		if (col <= 1 and cellNumericValue(col, row) < 1.)
			return IRenderer::cellStyleError;

		if (col > 1 and col <= 3)
		{
			double d = cellNumericValue(col, row);
			if (d < 0. or d > 1)
				return IRenderer::cellStyleError;
		}
		if (col > 3 and col < 6)
		{
			double d = cellNumericValue(col, row);
			if (d < 0)
				return IRenderer::cellStyleError;

			if (pCluster)
			{
				if (col == 4)
				{
					if (d > cellNumericValue(col + 1, row) or d > pCluster->unitCount)
						return IRenderer::cellStyleError;
				}
				else
				{
					if (col == 5)
					{
						if (d < cellNumericValue(col - 1, row))
							return IRenderer::cellStyleError;
						if (d > pCluster->unitCount)
							return IRenderer::cellStyleWarning;
					}
				}
			}
		}
		return IRenderer::cellStyleWithNumericCheck(col, row);
	}





	wxColour ThermalClusterPrepro::verticalBorderColor(int x, int y) const
	{
		return (x == 3 or x == 5)
			? Default::BorderHighlightColor() : IRenderer::verticalBorderColor(x, y);
	}


	wxColour ThermalClusterPrepro::horizontalBorderColor(int x, int y) const
	{
		// Getting informations about the next hour
		// (because the returned color is about the bottom border of the cell,
		// so the next hour for the user)
		if (!(!study) and y + 1 < Date::Calendar::maxHoursInYear)
		{
			auto& hourinfo = study->calendar.hours[y + 1];

			if (hourinfo.firstHourInMonth)
				return Default::BorderMonthSeparator();
			if (hourinfo.firstHourInDay)
				return Default::BorderDaySeparator();
		}
		return IRenderer::verticalBorderColor(x, y);
	}


	void ThermalClusterPrepro::onStudyClosed()
	{
		internalThermalClusterChanged(nullptr);
		MatrixAncestorType::onStudyClosed();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

