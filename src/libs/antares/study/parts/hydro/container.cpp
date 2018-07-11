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

#include "../../study.h"
#include "container.h"
#include "../../../inifile.h"
#include "../../../array/array1d.h"


using namespace Antares;
using namespace Yuni;

#define SEP IO::Separator


namespace Antares
{
namespace Data
{


	PartHydro::PartHydro() :
		interDailyBreakdown(0.),
		intraDailyModulation(2.),
		intermonthlyBreakdown(0),
		reservoirManagement(false),
		reservoirCapacity(0.),
		prepro(nullptr),
		series(nullptr),
		hydroModulable(false)
	{
	}


	PartHydro::~PartHydro()
	{
		delete prepro;
		delete series;
	}


	void PartHydro::reset()
	{
		intraDailyModulation    = 24;
		interDailyBreakdown     = 1.;
		intermonthlyBreakdown   = 1.;

		reservoirManagement = false;
		reservoirCapacity   = 0.;

		maxPower.reset(3, DAYS_PER_YEAR, true);
		reservoirLevel.reset(3, 12, true);
		reservoirLevel.fillColumn(minimum, 0.0);
		reservoirLevel.fillColumn(average, 0.5);
		reservoirLevel.fillColumn(maximum, 1.0);

		// reset of the hydro allocation - however we don't have any information
		// about the current area, which should be by default 1.
		// This work is done in Area::reset()
		allocation.clear();
		// allocation.fromArea(<current area>, 1.); // Area::reset()

		if (prepro)
			prepro->reset();
		if (series)
			series->reset();
	}



	bool PartHydro::LoadFromFolder(Study& study, const AnyString& folder)
	{
		auto& buffer = study.bufferLoadingTS;
		bool ret = true;

		// Initialize all alpha values to 0
		study.areas.each([&] (Data::Area& area)
		{
			area.hydro.interDailyBreakdown   = 1.;
			area.hydro.intraDailyModulation  = 24.;
			area.hydro.intermonthlyBreakdown = 1.;
			area.hydro.reservoirManagement   = false;
			area.hydro.reservoirCapacity     = 0.;

			// maximum capacity expectation
			if (study.header.version < 390)
			{
				area.hydro.maxPower.reset(3, DAYS_PER_YEAR, true);

				buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
					<< "maxcapacityexpectation_" << area.id << '.' << study.inputExtension;
				Matrix<uint> array;
				if (array.loadFromCSVFile(buffer, 1, 12, Matrix<>::optFixedSize | Matrix<>::optImmediate,
					&study.dataBuffer))
				{
					// days per month, immutable values for version prior to 3.9 for sure
					// these values was hard-coded before 3.9
					static const uint daysPerMonth[] = {
						31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
					};

					uint dayYear = 0;
					for (uint m = 0; m != 12; ++m)
					{
						uint nbDays = daysPerMonth[m];
						uint power = array.entry[0][m];

						for (uint d = 0; d != nbDays; ++d, ++dayYear)
						{
							for (uint x = 0; x != area.hydro.maxPower.width; ++x)
								area.hydro.maxPower.entry[x][dayYear] = power;
						}
					}
				}
				else
					area.hydro.maxPower.reset(3, DAYS_PER_YEAR, true);
				area.hydro.maxPower.markAsModified();
			}
			else
			{
				buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
					<< "maxpower_" << area.id << '.' << study.inputExtension;

				//	GUI part patch :
				//		We need to know, when estimating the RAM required by the solver, if the current area
				//		is hydro modulable. Therefore, reading the area's daily max power at this stage is 
				//		necessary.
				if(!study.usedByTheSolver)
				{
					bool enabledModeIsChanged = false;
					if(JIT::enabled)
					{
						JIT::enabled = false;	// Allowing to read the area's daily max power
						enabledModeIsChanged = true;
					}

					ret = area.hydro.maxPower.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR,
						Matrix<>::optFixedSize, &study.dataBuffer) && ret;

					if(enabledModeIsChanged)
						JIT::enabled = true;	// Back to the previous loading mode.
				}
				else
					ret = area.hydro.maxPower.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR,
						Matrix<>::optFixedSize, &study.dataBuffer) && ret;
			}

			if (study.header.version >= 390)
			{
				buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
					<< "reservoir_" << area.id << '.' << study.inputExtension;
				ret = area.hydro.reservoirLevel.loadFromCSVFile(buffer, 3, 12,
					Matrix<>::optFixedSize, &study.dataBuffer) && ret;
			}
			else
			{
				area.hydro.reservoirLevel.reset(3, 12, true);
				area.hydro.reservoirLevel.fillColumn(area.hydro.minimum, 0.0);
				area.hydro.reservoirLevel.fillColumn(area.hydro.average, 0.5);
				area.hydro.reservoirLevel.fillColumn(area.hydro.maximum, 1.0);
				area.hydro.reservoirLevel.markAsModified();
			}

			if (study.usedByTheSolver)
			{
				if (area.hydro.reservoirManagement)
				{
					if (area.hydro.reservoirCapacity < 1e-3 /*arbitrary*/)
					{
						logs.error() << area.name << ": invalid reservoir capacity (must be > 0)";
						area.hydro.reservoirManagement = false; // just in case
					}
				}

				auto& min = area.hydro.maxPower[area.hydro.minimum];
				auto& avg = area.hydro.maxPower[area.hydro.average];
				auto& max = area.hydro.maxPower[area.hydro.maximum];
				uint errors = 0;

				for (uint y = 0; y != area.hydro.maxPower.height; ++y)
				{
					if (min[y] > avg[y])
					{
						min[y] = avg[y];
						if (++errors > 30)
							logs.warning() << area.name << ": hydro max power:   skipping display... ";
						else
							logs.warning() << area.name << ": hydro max power: minimum > average at line " << (y + 1);
					}
					if (max[y] < avg[y])
					{
						max[y] = avg[y];
						if (++errors > 30)
							logs.warning() << area.name << ": hydro max power:   skipping display... ";
						else
							logs.warning() << area.name << ": hydro max power: maximum < average at line " << (y + 1);
					}
				}
			}
			else
			{
				// Is area hydro modulable ? 
				auto& max = area.hydro.maxPower[area.hydro.maximum];

				for (uint y = 0; y != area.hydro.maxPower.height; ++y)
				{
					if(max[y] > 0.)
					{
						area.hydro.hydroModulable = true;
						break;
					}
				}
			}
		});

		IniFile ini;
		if (not ini.open(buffer.clear() << folder << SEP << "hydro.ini"))
			return false;

		// The section name
		// Before 3.3, the smoothing factor was called 'alpha', for historical
		// reasons.
		// But, since 3.3, it has been renamed 'inter-daily breakdown' and a new
		// value 'intra-daily modulation' was added
		const char* const sectionName = (study.header.version <= 320)
			? "alpha" : "inter-daily-breakdown";

		IniFile::Section* section;
		IniFile::Property* property;

		if ((section = ini.find(sectionName)))
		{
			if ((property = section->firstProperty))
			{
				// Browse all properties
				for (; property; property = property->next)
				{
					AreaName id = property->key;
					id.toLower();

					Area* area = study.areas.find(id);
					if (area)
						ret = property->value.to<double>(area->hydro.interDailyBreakdown) && ret;
					else
						logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
				}
			}
		}

		if (study.header.version >= 330 && (section = ini.find("intra-daily-modulation")))
		{
			if ((property = section->firstProperty))
			{
				AreaName id;

				// Browse all properties
				for (; property; property = property->next)
				{
					id = property->key;
					id.toLower();

					auto* area = study.areas.find(id);
					if (area)
					{
						ret = property->value.to<double>(area->hydro.intraDailyModulation) && ret;
						if (area->hydro.intraDailyModulation < 1.)
						{
							logs.error() << area->id << ": Invalid intra-daily modulation. It must be >= 1.0, Got "
								<< area->hydro.intraDailyModulation << " (truncated to 1)";
							area->hydro.intraDailyModulation = 1.;
						}
					}
					else
						logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
				}
			}
		}

		if (study.header.version >= 390 && (section = ini.find("reservoir")))
		{
			if ((property = section->firstProperty))
			{
				// Browse all properties
				for (; property; property = property->next)
				{
					AreaName id = property->key;
					id.toLower();

					auto* area = study.areas.find(id);
					if (area)
						ret = property->value.to<bool>(area->hydro.reservoirManagement) && ret;
					else
						logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
				}
			}
		}

		if (study.header.version >= 390 && (section = ini.find("reservoir capacity")))
		{
			if ((property = section->firstProperty))
			{
				// Browse all properties
				for (; property; property = property->next)
				{
					AreaName id = property->key;
					id.toLower();

					auto* area = study.areas.find(id);
					if (area)
					{
						ret = property->value.to<double>(area->hydro.reservoirCapacity) && ret;
						if (area->hydro.reservoirCapacity < 1e-6)
						{
							logs.error() << area->id << ": Invalid reservoir capacity.";
							area->hydro.reservoirCapacity = 0.;
						}
					}
					else
						logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
				}
			}
		}

		if (study.header.version >= 390 && (section = ini.find("inter-monthly-breakdown")))
		{
			if ((property = section->firstProperty))
			{
				// Browse all properties
				for (; property; property = property->next)
				{
					AreaName id = property->key;
					id.toLower();

					auto* area = study.areas.find(id);
					if (area)
					{
						ret = property->value.to<double>(area->hydro.intermonthlyBreakdown) && ret;
						if (area->hydro.intermonthlyBreakdown < 0)
						{
							logs.error() << area->id << ": Invalid intermonthly breakdown";
							area->hydro.intermonthlyBreakdown = 0.;
						}
					}
					else
						logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
				}
			}
		}

		return ret;
	}



	bool PartHydro::SaveToFolder(const AreaList& areas, const AnyString& folder)
	{
		if (!folder)
		{
			logs.error() << "hydro: invalid empty folder";
			assert(false && "invalid empty folder");
			return false;
		}

		String buffer;
		buffer.clear() << folder << SEP << "common" << SEP << "capacity";

		// Init
		IniFile ini;
		auto* s     = ini.addSection("inter-daily-breakdown");
		auto* smod  = ini.addSection("intra-daily-modulation");
		auto* sIMB  = ini.addSection("inter-monthly-breakdown");
		auto* sreservoir = ini.addSection("reservoir");
		auto* sreservoirCapacity = ini.addSection("reservoir capacity");

		// return status
		bool ret = true;

		// Add all alpha values for each area
		areas.each([&] (const Data::Area& area)
		{
			s->add(area.id, area.hydro.interDailyBreakdown);
			smod->add(area.id, area.hydro.intraDailyModulation);
			sIMB->add(area.id, area.hydro.intermonthlyBreakdown);

			if (area.hydro.reservoirManagement)
				sreservoir->add(area.id, true);
			if (area.hydro.reservoirCapacity > 1e-6)
				sreservoirCapacity->add(area.id, area.hydro.reservoirCapacity);

			// max power
			buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
				<< "maxpower_" << area.id << ".txt";
			ret = area.hydro.maxPower.saveToCSVFile(buffer, /*decimal*/ 0) && ret;
			// reservoir
			buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
				<< "reservoir_" << area.id << ".txt";
			ret = area.hydro.reservoirLevel.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
		});

		// Write the ini file
		buffer.clear() << folder << SEP << "hydro.ini";
		return ini.save(buffer) && ret;
	}



	bool PartHydro::invalidate(bool reload) const
	{
		bool ret = true;
		ret = maxPower.invalidate(reload) && ret;
		ret = reservoirLevel.invalidate(reload) && ret;

		if (series)
			ret = series->invalidate(reload) && ret;
		if (prepro)
			ret = prepro->invalidate(reload) && ret;

		return ret;
	}


	void PartHydro::markAsModified() const
	{
		maxPower.markAsModified();
		reservoirLevel.markAsModified();

		if (series)
			series->markAsModified();
		if (prepro)
			prepro->markAsModified();
	}


	void PartHydro::copyFrom(const PartHydro& rhs)
	{
		// max power
		{
			maxPower = rhs.maxPower;
			maxPower.unloadFromMemory();
			rhs.maxPower.unloadFromMemory();
		}
		// reservoir levels
		{
			reservoirLevel = rhs.reservoirLevel;
			reservoirLevel.unloadFromMemory();
			rhs.reservoirLevel.unloadFromMemory();
		}
		// values
		{
			interDailyBreakdown     = rhs.interDailyBreakdown;
			intraDailyModulation    = rhs.intraDailyModulation;
			intermonthlyBreakdown   = rhs.intermonthlyBreakdown;
			reservoirManagement     = rhs.reservoirManagement;
			reservoirCapacity       = rhs.reservoirCapacity;
		}
	}





} // namespace Data
} // namespace Antares

