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

#pragma once

#include <vector>
#include "cell.h"


using namespace std;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

using namespace Antares::Data;

// -------------------
//  Base column class
// -------------------
class Column
{
protected:
	using study_ptr = Data::Study::Ptr;

public:
	Column(int nbLines);
	virtual ~Column();
	cell* getLine(int y) const;

protected:
	int nbLines_;
	vector<cell*> cells_;
	TimeSeries tsKind_;
};

// -------------------
//  Load column
// -------------------
class ColumnLoad final : public Column
{
public:
	ColumnLoad(int nbLines);
	~ColumnLoad() = default;
};

// -------------------
//  Thermal column
// -------------------
class ColumnThermal final : public Column
{
public:
	ColumnThermal(int nbLines);
	~ColumnThermal() = default;
};

// -------------------
//  Hydro column
// -------------------
class ColumnHydro final : public Column
{
public:
	ColumnHydro(int nbLines);
	~ColumnHydro() = default;
};

// -------------------
//  Wind column
// -------------------
class ColumnWind final : public Column
{
public:
	ColumnWind(int nbLines);
	~ColumnWind() = default;
};

// -------------------
//  Solar column
// -------------------
class ColumnSolar final : public Column
{
public:
	ColumnSolar(int nbLines);
	~ColumnSolar() = default;
};

// -----------------------------
//  Renewable clusters column
// -----------------------------
class ColumnRenewableClusters final : public Column
{
public:
	ColumnRenewableClusters(int nbLines);
	~ColumnRenewableClusters() = default;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares