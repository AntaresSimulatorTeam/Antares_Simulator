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
Column::Column()
{}

cell* Column::getLine(int y) const
{
	return cells_[y].get();
}

int Column::getNumberOfLines() const
{
	return cells_.size();
}

// gp : except ts kind, all column constructors are the same !
// gp : factor classical columns in one

// ---------------------
// Column load
// ---------------------
ColumnLoad::ColumnLoad() : Column() 
{
	tsKind_ = timeSeriesLoad;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<readyMadeTSstatus>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<generatedTSstatus>(tsKind_));
	cells_.push_back(make_unique<NumberTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshSpanCell>(tsKind_));
	cells_.push_back(make_unique<SeasonalCorrelationCell>(tsKind_));
	cells_.push_back(make_unique<storeToInputCell>(tsKind_));
	cells_.push_back(make_unique<storeToOutputCell>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}

// ---------------------
// Column thermal
// ---------------------
ColumnThermal::ColumnThermal() : Column() 
{
	tsKind_ = timeSeriesThermal;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<readyMadeTSstatus>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<generatedTSstatus>(tsKind_));
	cells_.push_back(make_unique<NumberTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshSpanCell>(tsKind_));
	cells_.push_back(make_unique<SeasonalCorrelationCell>(tsKind_));
	cells_.push_back(make_unique<storeToInputCell>(tsKind_));
	cells_.push_back(make_unique<storeToOutputCell>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}

// ---------------------
// Column hydro
// ---------------------
ColumnHydro::ColumnHydro() : Column()
{
	tsKind_ = timeSeriesHydro;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<readyMadeTSstatus>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<generatedTSstatus>(tsKind_));
	cells_.push_back(make_unique<NumberTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshSpanCell>(tsKind_));
	cells_.push_back(make_unique<SeasonalCorrelationCell>(tsKind_));
	cells_.push_back(make_unique<storeToInputCell>(tsKind_));
	cells_.push_back(make_unique<storeToOutputCell>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}

// ---------------------
// Column wind
// ---------------------
ColumnWind::ColumnWind() : Column()
{
	tsKind_ = timeSeriesWind;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<readyMadeTSstatus>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<generatedTSstatus>(tsKind_));
	cells_.push_back(make_unique<NumberTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshSpanCell>(tsKind_));
	cells_.push_back(make_unique<SeasonalCorrelationCell>(tsKind_));
	cells_.push_back(make_unique<storeToInputCell>(tsKind_));
	cells_.push_back(make_unique<storeToOutputCell>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}

// ---------------------
// Column Solar
// ---------------------
ColumnSolar::ColumnSolar() : Column()
{
	tsKind_ = timeSeriesSolar;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<readyMadeTSstatus>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<generatedTSstatus>(tsKind_));
	cells_.push_back(make_unique<NumberTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshTsCell>(tsKind_));
	cells_.push_back(make_unique<RefreshSpanCell>(tsKind_));
	cells_.push_back(make_unique<SeasonalCorrelationCell>(tsKind_));
	cells_.push_back(make_unique<storeToInputCell>(tsKind_));
	cells_.push_back(make_unique<storeToOutputCell>(tsKind_));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}

// -------------------------------
// Column renewable clusters 
// -------------------------------
ColumnRenewableClusters::ColumnRenewableClusters() : Column()
{
	tsKind_ = timeSeriesRenewable;

	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("On")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<inactiveRenewableClusterCell>(wxT("-")));
	cells_.push_back(make_unique<blankCell>());
	cells_.push_back(make_unique<intraModalCell>(tsKind_));
	cells_.push_back(make_unique<interModalCell>(tsKind_));
}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares