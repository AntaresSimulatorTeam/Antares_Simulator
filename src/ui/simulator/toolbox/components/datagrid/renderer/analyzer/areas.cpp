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

#include "areas.h"
#include "../../../../../application/study.h"
#include "../../component.h"
#include <antares/study.h>

using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
namespace Analyzer
{


	Areas::Areas() :
		pMapFile(nullptr)
	{
	}


	void Areas::initializeFromStudy()
	{
		if (!(!study))
		{
			pRecord.mutex.lock();
			pRecord.array.resize(study->areas.size());
			uint indx = 0;

			study->areas.each([&] (const Data::Area& area)
			{
				assert(indx < pRecord.size());
				assert(area.load.prepro);
				typedef Analyzer::Areas::Record Record;

				Record& record      = pRecord.array[indx];
				record.enabled      = true;
				record.status       = Record::stNotFound;
				record.areaIndex    = area.index;
				record.wxAreaName   << wxStringFromUTF8(area.name);
				record.fileToSearch << area.name << ".txt";
				record.fileToSearch.toLower();
				record.mWidth       = (uint) -1;
				record.mHeight      = (uint) -1;
				//record.distribution = Data::XCast::dtBeta;
				record.distribution = area.load.prepro->xcast.distribution;
				record.absolute     = true;

				++indx;
			});
			pRecord.canRunAnalyzer = false;
			pRecord.mutex.unlock();
		}
	}


	void Areas::reloadDistributionLawsFromStudy(Data::TimeSeries ts)
	{
		if (!(!study))
		{
			uint indx = 0;
			pRecord.mutex.lock();

			study->areas.each([&] (const Data::Area& area)
			{
				assert(indx < pRecord.size());
				assert(area.load.prepro);
				typedef Analyzer::Areas::Record Record;

				Record& record = pRecord.array[indx];
				switch (ts)
				{
					case Data::timeSeriesWind:
						record.distribution = area.wind.prepro->xcast.distribution;
						break;
					case Data::timeSeriesLoad:
						record.distribution = area.load.prepro->xcast.distribution;
						break;
					case Data::timeSeriesSolar:
						record.distribution = area.solar.prepro->xcast.distribution;
						break;
					default:
						record.distribution = Data::XCast::dtBeta;
				}
				++indx;
			});
			pRecord.mutex.unlock();
		}
	}


	void Areas::resetStatus(Record::Status status, bool invalidateSize)
	{
		Yuni::MutexLocker locker(pRecord.mutex);
		auto end = pRecord.array.end();
		for (auto i = pRecord.array.begin(); i != end; ++i)
		{
			i->status  = status;
			if (invalidateSize)
			{
				i->mWidth  = (uint) -1;
				i->mHeight = (uint) -1;
			}
		}
	}


	void Areas::resetStatusWaiting(bool invalidateSize)
	{
		resetStatus(Record::stWaiting, invalidateSize);
	}


	Areas::~Areas()
	{
		destroyBoundEvents();
	}


	wxString Areas::columnCaption(int colIndx) const
	{
		static const wxChar* const names[] =
		{
			wxT("Active"),
			wxT("Distribution"),
			wxT("    Data"),
			wxT("                File to analyze"), wxT("       Status")
		};
		if (colIndx < 5)
			return names[colIndx];
		return wxEmptyString;
	}


	wxString Areas::rowCaption(int rowIndx) const
	{
		Yuni::MutexLocker locker(pRecord.mutex);
		if (rowIndx < (int)pRecord.size())
			return pRecord.array[rowIndx].wxAreaName;
		return wxEmptyString;
	}


	IRenderer::CellStyle Areas::cellStyle(int col, int row) const
	{
		Yuni::MutexLocker locker(pRecord.mutex);
		if (row >= (int)pRecord.size())
			return IRenderer::cellStyleError;
		const Record& record = pRecord.array[row];

		if (record.enabled)
		{
			if (col == 4)
			{
				switch (record.status)
				{
					case Record::stReady:
						return IRenderer::cellStyleConstraintWeight;
					case Record::stError:
						return IRenderer::cellStyleError;
					default:
						return IRenderer::cellStyleDefaultDisabled;
				}
			}
			return IRenderer::cellStyleDefault;
		}
		return IRenderer::cellStyleDefaultDisabled;
	}


	wxColour Areas::cellBackgroundColor(int, int) const
	{
		return wxColour();
	}


	wxColour Areas::cellTextColor(int, int) const
	{
		return wxColour();
	}


	wxString Areas::cellValue(int x, int y) const
	{
		Yuni::MutexLocker locker(pRecord.mutex);
		if (y >= (int)pRecord.size())
			return wxEmptyString;
		const Record& record = pRecord.array[y];

		switch (x)
		{
			case 0:
				return (record.enabled) ? wxT("Yes") : wxT("No");
			case 1:
				return wxStringFromUTF8(Data::XCast::DistributionToCString(record.distribution));
			case 2:
				return (record.absolute ? wxT("raw") : wxT("detrended"));
			case 3:
				return wxStringFromUTF8(record.fileToSearch);
			case 4:
				{
					switch (record.status)
					{
						case Record::stNotFound:
							return wxT("not found");
						case Record::stWaiting:
							return wxT("waiting");
						case Record::stReady:
							return wxT("ready");
						case Record::stError:
							return wxT("invalid");
					}
				}
		}
		return wxEmptyString;
	}


	double Areas::cellNumericValue(int x,int y) const
	{
		Yuni::MutexLocker locker(pRecord.mutex);
		if (y >= (int)pRecord.size())
			return IRenderer::cellStyleError;
		const Record& record = pRecord.array[y];

		switch (x)
		{
			case 0: return (record.enabled) ? +1. : 0.;
			case 3: return 0.;
			case 4: return (record.status == Record::stReady)  ? +1. : 0.;
		}
		return 0.;
	}


	bool Areas::cellValue(int x, int y, const String& value)
	{
		if (x == 0)
		{
			Yuni::MutexLocker locker(pRecord.mutex);
			if (y >= (int)pRecord.size())
				return false;
			pOnBeforeUpdate(x, y);
			Record& record = pRecord.array[y];

			record.enabled = value.to<bool>();
			record.status = Record::stWaiting;
			pRecord.canRunAnalyzer = false;

			// Re-launch the timer for update
			pOnAfterUpdate(x, y);
			return true;
		}
		if (x == 1) // distribution
		{
			Yuni::MutexLocker locker(pRecord.mutex);
			if (y >= (int)pRecord.size())
				return false;
			Record& record = pRecord.array[y];
			if (!value.empty())
			{
				auto d = Data::XCast::StringToDistribution(value);
				if (d != Data::XCast::dtNone and d != record.distribution)
				{
					record.distribution = d;
					return true;
				}
			}
			return false;
		}
		if (x == 2) // absolute
		{
			Yuni::MutexLocker locker(pRecord.mutex);
			if (y >= (int)pRecord.size())
				return false;
			Record& record = pRecord.array[y];
			CString<20, false> v = value;
			v.toLower();

			bool newvalue;

			if (v == "r" || v == "raw")
				newvalue = true;
			else if (v == "d" || v == "detrended")
				newvalue = false;
			else
				return false;

			if (newvalue != record.absolute)
			{
				record.absolute = newvalue;
				return true;
			}
			return false;
		}
		if (x == 3)
		{
			Yuni::MutexLocker locker(pRecord.mutex);
			if (y >= (int)pRecord.size())
				return false;
			pOnBeforeUpdate(x, y);
			Record& record = pRecord.array[y];

			if (value != record.fileToSearch)
			{
				record.mWidth  = (uint) -1;
				record.mHeight = (uint) -1;
				record.fileToSearch = value;
				record.fileToSearch.toLower();
			}
			record.status = Record::stWaiting;
			pRecord.canRunAnalyzer = false;

			// Re-launch the timer for update
			pOnAfterUpdate(x, y);
			return true;

		}
		return false;
	}


	bool Areas::valid() const
	{
		return not pRecord.empty();
	}





} // namespace Analyzer
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

