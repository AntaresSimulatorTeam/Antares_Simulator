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

#include "column.h"
#include "cell.h"


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

// -------------------
// Base column class
// -------------------
Column::Column(int nbLines) : nbLines_(nbLines)
{}

Column::~Column()
{
	for (int i = 0; i < nbLines_; i++)
		if (cells_[i])
			delete cells_[i];
}

cell* Column::getLine(int y) const
{
	return cells_[y];
}

// gp : except ts kind, all column constructors are the same !
// gp : factor classical columns in one

// ---------------------
// Column load
// ---------------------
ColumnLoad::ColumnLoad(int nbLines) : Column(nbLines) 
{
	tsKind_ = timeSeriesLoad;

	cells_.push_back(new blankCell());
	cells_.push_back(new readyMadeTSstatus(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new generatedTSstatus(tsKind_));
	cells_.push_back(new NumberTsCell(tsKind_));
	cells_.push_back(new RefreshTsCell(tsKind_));
	cells_.push_back(new RefreshSpanCell(tsKind_));
	cells_.push_back(new SeasonalCorrelationCell(tsKind_));
	cells_.push_back(new storeToInputCell(tsKind_));
	cells_.push_back(new storeToOutputCell(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}

// ---------------------
// Column thermal
// ---------------------
ColumnThermal::ColumnThermal(int nbLines) : Column(nbLines) 
{
	tsKind_ = timeSeriesThermal;

	cells_.push_back(new blankCell());
	cells_.push_back(new readyMadeTSstatus(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new generatedTSstatus(tsKind_));
	cells_.push_back(new NumberTsCell(tsKind_));
	cells_.push_back(new RefreshTsCell(tsKind_));
	cells_.push_back(new RefreshSpanCell(tsKind_));
	cells_.push_back(new SeasonalCorrelationCell(tsKind_));
	cells_.push_back(new storeToInputCell(tsKind_));
	cells_.push_back(new storeToOutputCell(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}

// ---------------------
// Column hydro
// ---------------------
ColumnHydro::ColumnHydro(int nbLines) : Column(nbLines)
{
	tsKind_ = timeSeriesHydro;

	cells_.push_back(new blankCell());
	cells_.push_back(new readyMadeTSstatus(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new generatedTSstatus(tsKind_));
	cells_.push_back(new NumberTsCell(tsKind_));
	cells_.push_back(new RefreshTsCell(tsKind_));
	cells_.push_back(new RefreshSpanCell(tsKind_));
	cells_.push_back(new SeasonalCorrelationCell(tsKind_));
	cells_.push_back(new storeToInputCell(tsKind_));
	cells_.push_back(new storeToOutputCell(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}

// ---------------------
// Column wind
// ---------------------
ColumnWind::ColumnWind(int nbLines) : Column(nbLines)
{
	tsKind_ = timeSeriesWind;

	cells_.push_back(new blankCell());
	cells_.push_back(new readyMadeTSstatus(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new generatedTSstatus(tsKind_));
	cells_.push_back(new NumberTsCell(tsKind_));
	cells_.push_back(new RefreshTsCell(tsKind_));
	cells_.push_back(new RefreshSpanCell(tsKind_));
	cells_.push_back(new SeasonalCorrelationCell(tsKind_));
	cells_.push_back(new storeToInputCell(tsKind_));
	cells_.push_back(new storeToOutputCell(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}

// ---------------------
// Column Solar
// ---------------------
ColumnSolar::ColumnSolar(int nbLines) : Column(nbLines)
{
	tsKind_ = timeSeriesSolar;

	cells_.push_back(new blankCell());
	cells_.push_back(new readyMadeTSstatus(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new generatedTSstatus(tsKind_));
	cells_.push_back(new NumberTsCell(tsKind_));
	cells_.push_back(new RefreshTsCell(tsKind_));
	cells_.push_back(new RefreshSpanCell(tsKind_));
	cells_.push_back(new SeasonalCorrelationCell(tsKind_));
	cells_.push_back(new storeToInputCell(tsKind_));
	cells_.push_back(new storeToOutputCell(tsKind_));
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}

// -------------------------------
// Column renewable clusters 
// -------------------------------
ColumnRenewableClusters::ColumnRenewableClusters(int nbLines) : Column(nbLines)
{
	tsKind_ = timeSeriesRenewable;

	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new blankCell());
	cells_.push_back(new intraModalCell(tsKind_));
	cells_.push_back(new interModalCell(tsKind_));
}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares