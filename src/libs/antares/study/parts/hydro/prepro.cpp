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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include "../../study.h"
#include "../../memory-usage.h"
#include "prepro.h"
#include "../../../inifile.h"
#include "../../../array/array1d.h"
#include "../../../logs.h"

using namespace Antares;
using namespace Yuni;

#define SEP IO::Separator



namespace Antares
{
namespace Data
{


	static bool PreproHydroSaveSettings(PreproHydro* h, const char* filename)
	{
		IniFile ini;
		auto* s = ini.addSection("prepro");
		s->add("intermonthly-correlation", h->intermonthlyCorrelation);
		return ini.save(filename);
	}


	static bool PreproHydroLoadSettings(PreproHydro* h, const char* filename)
	{
		IniFile ini;
		IniFile::Section* s;
		IniFile::Property* p;

		// Reset
		h->intermonthlyCorrelation = 0.5f;

		// Try to load the file
		if (ini.open(filename))
		{
			bool ret = false;
			if ((s = ini.find("prepro")))
			{
				p = s->find("intermonthly-correlation");
				if (p || (p = s->find("intermonthlycorrelationvalue")))
				{
					const CString<20, false> t = p->value;
					ret = t.to(h->intermonthlyCorrelation);
				}
				else
					logs.error() << filename << ": prepro/intermonthlyCorrelation: The property is missing";
			}
			else
				logs.error() << filename << ": [prepro]: The section is missing";

			return ret;
		}
		else
			logs.error() << filename << ": Impossible to open the file";

		return false;
	}







	PreproHydro::PreproHydro()
	{
	}


	uint64 PreproHydroMemoryUsage(PreproHydro* h)
	{
		return (h) ? sizeof(double) : 0;
	}


	void PreproHydro::reset()
	{
		intermonthlyCorrelation = 0.5;
		// data
		data.reset(hydroPreproMax, 12, true);
		// Expectation
		auto& col = data[expectation];
		for (uint i = 0; i != 12; ++i)
			col[i] = 0.;
	}

	void PreproHydro::copyFrom(const PreproHydro& rhs)
	{
		intermonthlyCorrelation = rhs.intermonthlyCorrelation;
		data = rhs.data;
		rhs.data.unloadFromMemory();
	}



	bool PreproHydro::saveToFolder(const AreaName& areaID, const char* folder)
	{
		assert(folder);
		assert('\0' != *folder);

		Clob buffer;
		// Make sure the folder is created
		buffer.clear() << folder << SEP << areaID;
		if (IO::Directory::Create(buffer))
		{
			// Writing the ini file
			buffer.clear() << folder << SEP << areaID << SEP << "prepro.ini";
			bool ret = (PreproHydroSaveSettings(this, buffer.c_str()) ? true : false);

			buffer.clear() << folder << SEP << areaID << SEP << "energy.txt";
			if (data.jit)
			{
				assert(0 != (data.jit->options & Matrix<>::optFixedSize));
				assert(data.jit->minWidth == hydroPreproMax);
			}
			ret = data.saveToCSVFile(buffer, /*decimal*/ 3) && ret;

			return ret;
		}
		return false;
	}



	bool PreproHydro::loadFromFolder(Study& s, const AreaName& areaID, const char* folder)
	{
		/* Asserts */
		assert(folder);
		assert('\0' != *folder);

		enum
		{
			mtrxOption = Matrix<>::optFixedSize | Matrix<>::optImmediate,
		};

		data.resize(hydroPreproMax, 12, true);
		String& buffer = s.bufferLoadingTS;

		buffer.clear() << folder << SEP << areaID << SEP << "prepro.ini";
		bool ret = (PreproHydroLoadSettings(this, buffer.c_str()) ? true : false);
		if (intermonthlyCorrelation < 0. || intermonthlyCorrelation > 1.)
		{
			logs.error() << "Hydro: Prepro: `" << areaID
				<< "`: Intermonthly correlation value: The value must be between 0 and 1";
			if (intermonthlyCorrelation < 0.)
				intermonthlyCorrelation = 0.;
			else
				intermonthlyCorrelation = 1.;
		}

		if (s.header.version < 350)
		{
			data.reset(hydroPreproMax, 12, true);

			if (s.header.version <= 320)
			{
				Matrix<> tmpExpectationMtx;
				Matrix<> tmpStdDeviationMtx;

				buffer.clear() << folder << SEP << areaID << SEP << "logexpectation." << s.inputExtension;
				ret = tmpExpectationMtx.loadFromCSVFile(buffer, 1, 12, mtrxOption) && ret;
				buffer.clear() << folder << SEP << areaID << SEP << "standarddeviation." << s.inputExtension;
				ret = tmpStdDeviationMtx.loadFromCSVFile(buffer, 1, 12, mtrxOption) && ret;

				if (tmpExpectationMtx.height != 12 || tmpExpectationMtx.width != 1)
					tmpExpectationMtx.reset(1, 12, true);
				if (tmpStdDeviationMtx.height != 12 || tmpStdDeviationMtx.width != 1)
					tmpStdDeviationMtx.reset(1, 12, true);

				auto& tmpExpectation = tmpExpectationMtx[0];
				auto& tmpStdDeviation = tmpStdDeviationMtx[0];

				double e, s;
				for (uint i = 0; i != 12; ++i)
				{
					// E = exp(E' + S'*S'/2)
					e = exp(tmpExpectation[i] + tmpStdDeviation[i] * tmpStdDeviation[i] / 2.);
					// S = exp(E' + S'*S'/2) * sqrt(exp(S'*S') - 1)
					s = e * sqrt(exp(tmpStdDeviation[i] * tmpStdDeviation[i]) - 1.);

					tmpExpectation[i]  = e;
					tmpStdDeviation[i] = s;
				}
				data.pasteToColumn(expectation, tmpExpectation);
				data.pasteToColumn(stdDeviation, tmpStdDeviation);
			}
			else
			{
				{
					buffer.clear() << folder << SEP << areaID << SEP << "expectation." << s.inputExtension;
					Matrix<> m;
					if (!m.loadFromCSVFile(buffer, 1, 12, mtrxOption) || m.width != 1 || m.height != 12)
						m.reset(1, 12, true);
					data.pasteToColumn(expectation, m[0]);
				}
				{
					buffer.clear() << folder << SEP << areaID << SEP << "standarddeviation." << s.inputExtension;
					Matrix<> m;
					if (!m.loadFromCSVFile(buffer, 1, 12, mtrxOption) || m.width != 1 || m.height != 12)
						m.reset(1, 12, true);
					data.pasteToColumn(stdDeviation, m[0]);
				}
			}
			{
				buffer.clear() << folder << SEP << areaID << SEP << "minenergy." << s.inputExtension;
				Matrix<> m;
				if (!m.loadFromCSVFile(buffer, 1, 12, mtrxOption) || m.width != 1 || m.height != 12)
					m.reset(1, 12, true);
				data.pasteToColumn(minimumEnergy, m[0]);
			}
			{
				buffer.clear() << folder << SEP << areaID << SEP << "maxenergy." << s.inputExtension;
				Matrix<> m;
				if (!m.loadFromCSVFile(buffer, 1, 12, mtrxOption) || m.width != 1 || m.height != 12)
					m.reset(1, 12, true);
				data.pasteToColumn(maximumEnergy, m[0]);
			}
			{
				buffer.clear() << folder << SEP << areaID << SEP << "poweroverwater." << s.inputExtension;
				Matrix<> m;
				if (!m.loadFromCSVFile(buffer, 1, 12, mtrxOption) || m.width != 1 || m.height != 12)
					m.reset(1, 12, true);
				data.pasteToColumn(powerOverWater, m[0]);
			}
		}
		else
		{
			buffer.clear() << folder << SEP << areaID << SEP << "energy.txt";
			ret = data.loadFromCSVFile(buffer, hydroPreproMax, 12, Matrix<>::optFixedSize, &s.dataBuffer) && ret;
		}

		if (JIT::enabled)
			return ret;

		// Checks
		{
			auto& col = data[powerOverWater];
			for (uint i = 0; i != data.height; ++i)
			{
				const double d = col[i];
				if (d < 0. || d > 1.)
					logs.error() << "Hydro: Prepro: " << areaID << ": invalid value for ROR (line: " << (i+1) << ")";
			}
		}

		{
			auto& colMin = data[minimumEnergy];
			auto& colMax = data[maximumEnergy];

			for (uint i = 0; i != data.height; ++i)
			{
				if (colMin[i] < 0.)
				{
					ret = false;
					logs.error() << "Hydro: Prepro: `" << areaID
						<< "`: minimum energy: At least one value is negative (line: " << (i+1) << ')';
					continue;
				}
				if (colMin[i] > colMax[i])
				{
					ret = false;
					logs.error() << "Hydro: Prepro: `" << areaID
						<< "`: the minimum energy must be less than the maximum energy (line: " << (i+1) << ')';
				}
			}
		}

		{
			auto& colExp = data[expectation];

			for (uint i = 0; i != data.height; i++)
			{
				if (colExp[i] < 0.)
				{
					ret = false;
					logs.error() << "Hydro: Prepro: `" << areaID
						<< "`: invalid value for expectation (line: " << (i+1) << ")";
				}
			}
		}

		{
			auto& colStdDev = data[stdDeviation];

			for (uint i = 0; i != data.height; i++)
			{
				if (colStdDev[i] < 0.)
				{
					ret = false;
					logs.error() << "Hydro: Prepro: `" << areaID
						<< "`: invalid value for standard deviation (line: " << (i+1) << ")";
				}
			}
		}

		return ret;
	}


	bool PreproHydro::invalidate(bool reload) const
	{
		return data.invalidate(reload);
	}


	void PreproHydro::markAsModified() const
	{
		return data.markAsModified();
	}


	void PreproHydro::estimateMemoryUsage(StudyMemoryUsage& u) const
	{
		if (timeSeriesHydro & u.study.parameters.timeSeriesToGenerate)
		{
			data.estimateMemoryUsage(u, true, hydroPreproMax, 12);
			u.requiredMemoryForInput += sizeof(PreproHydro);
		}
	}





} // namespace Data
} // namespace Antares

