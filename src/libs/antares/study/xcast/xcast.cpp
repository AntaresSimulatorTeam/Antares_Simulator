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

#include "xcast.h"
#include "../../logs.h"
#include "../../inifile.h"
#include "../study.h"
#include "../memory-usage.h"
#include <limits>


using namespace Yuni;


#define SEP IO::Separator


namespace Antares
{
namespace Data
{

	static void XCastNormalizeMuStrictlyLessThan24(Matrix<float>& data)
	{
		data.invalidate(true); // load data if needed
		bool modified = false;
		for (uint i = 0; i != data.height; ++i)
		{
			if (data[XCast::dataCoeffMu][i] > 23.f)
			{
				data[XCast::dataCoeffMu][i] = 23.f;
				modified = true;
			}
		}
		if (modified)
			data.markAsModified();
	}




	const char* XCast::TSTranslationUseToCString(TSTranslationUse use)
	{
		static const char* const names[3] =
		{
			"never", "before-conversion", "after-conversion"
		};
		assert((int)use < 3);
		return names[use];
	}


	XCast::TSTranslationUse XCast::CStringToTSTranslationUse(const AnyString& str)
	{
		if (not str.empty())
		{
			CString<40, false> s(str);
			s.toLower();
			if (s == "never" || s == "do not use" || s == "none" || s == "no")
				return tsTranslationNone;
			if (s == "before-conversion" || s == "add before conversion" || s == "before conversion" || s == "before" || s == "before scaling" || s == "add before scaling")
				return tsTranslationBeforeConversion;
			if (s == "after-conversion" || s == "add after conversion" || s == "after conversion" || s == "after" || s == "after scaling" || s == "add after scaling")
				return tsTranslationAfterConversion;
		}
		return tsTranslationNone;
	}


	const char* XCast::DistributionToCString(XCast::Distribution d)
	{
		assert((int)d < dtMax);
		static const char* const names[dtMax] =
		{
			"",
			"Uniform",
			"Beta",
			"Normal",
			"Weibull",
			"Gamma"
		};
		return names[d];
	}


	const char* XCast::DistributionToNameID(XCast::Distribution d)
	{
		static const char* const names[dtMax] =
		{
			"unknown",
			"Uniform", "Beta", "Normal", "WeibullShapeA", "GammaShapeA",
		};
		assert((int)d < dtMax);
		return names[d];
	}


	XCast::Distribution  XCast::StringToDistribution(AnyString text)
	{
		// temporary string for text manipulation
		text.trim(" \r\t\n");
		CString<20,false> id(text);
		id.toLower();

		if (id.size() == 1)
		{
			switch (id.first()) // one letter, mainly used from the interface
			{
				case 'b': return dtBeta;
				case 'u': return dtUniform;
				case 'n': return dtNormal;
				case 'w': return dtWeibullShapeA;
				case 'g': return dtGammaShapeA;
			}
		}
		else
		{
			// The most usefull probability distribution is the Beta distribution
			if (id == "beta")
				return dtBeta;
			if (id == "uniform")
				return dtUniform;
			if (id == "normal" || id == "normale")
				return dtNormal;
			if (id == "weibullshapea" || id == "weibull" || id == "weibul")
				return dtWeibullShapeA;
			if (id == "gammashapea" || id == "gamma")
				return dtGammaShapeA;
			// auto-fix for intermediate studies in 3.3.x. Can be removed in future releases
			if (id == "weibulshapea")
				return dtWeibullShapeA;
		}
		return dtNone;
	}




	XCast::XCast(TimeSeries ts) :
		useTranslation(tsTranslationNone),
		distribution(dtBeta),
		capacity(0),
		useConversion(false),
		timeSeries(ts)
	{
		K.resize(12, 24);
		data.resize((uint)dataMax, 12);
		// Do nothing
	}


	XCast::~XCast()
	{
		data.clear();
		K.clear();
		translation.clear();
		conversion.clear();
	}


	void XCast::resetToDefaultValues()
	{
		data.reset(dataMax, 12, true);
		data.fillColumn(dataCoeffAlpha, 1.f);
		data.fillColumn(dataCoeffBeta,  1.f);
		data.fillColumn(dataCoeffDelta, 1.f);
		data.fillColumn(dataCoeffTheta, 1.f);
		data.fillColumn(dataCoeffMu,    1.f);
		K.reset(12, 24, true);
		distribution = dtBeta;
		capacity = 0.;
		useConversion = false;
		resetTransferFunction();

		// Time-series translation
		translation.reset(1, HOURS_PER_YEAR);
		useTranslation = tsTranslationNone;
	}



	bool XCast::loadFromFolder(Study& study, const AnyString& folder)
	{
		// reset
		distribution   = dtBeta;
		capacity       = 0.;
		useTranslation = tsTranslationNone;
		useConversion  = false;

		// A temporary buffer for filename manipulations
		Clob buffer;
		// A temporary buffer for reading matrices
		Matrix<>::BufferType readBuffer;
		// Return value
		bool ret = true;
		// Settings
		buffer.clear() << folder << SEP << "settings.ini";

		IniFile ini;
		if (ini.open(buffer))
		{
			// For each section
			const IniFile::Property* p;
			CString<30,false> key;

			ini.each([&] (const IniFile::Section& section)
			{
				// For each property
				if (section.name == "general")
				{
					for (p = section.firstProperty; p != nullptr; p = p->next)
					{
						key = p->key;
						key.toLower();
						if (key == "distribution")
						{
							distribution = StringToDistribution(p->value);
							if (distribution == dtNone)
							{
								logs.warning() << buffer
									<< ": Invalid probability distribution. The beta distribution will be used";
								distribution = dtBeta;
							}
							continue;
						}
						if (key == "capacity")
						{
							capacity = p->value.to<double>();
							if (capacity < 0.)
							{
								logs.warning() << buffer << ": The capacity can not be a negative value";
								capacity = 0.;
							}
							continue;
						}
						if (key == "conversion" || key == "transfer-function" || key == "convertion")
						{
							useConversion = p->value.to<bool>();
							continue;
						}
						if (key == "translation" || key == "ts-average")
						{
							useTranslation = CStringToTSTranslationUse(p->value);
							continue;
						}

						logs.warning() << buffer << ": Unknown property '" << p->key << "'";
					}
				}
				else
					logs.warning() << buffer << ": unknown section '" << section.name << "'";
			});
		}
		else
		{
			logs.error() << "I/O Error: unable to open '" << buffer << "'";
			ret = false;
		}

		// It would be better to load the matrices after the settings in order to
		// fix invalid data

		// Coefficients
		buffer.clear() << folder << SEP << "data.txt";

		// Since 3.5, Beta' = 1 / Beta
		if (study.header.version < 350 && distribution == dtGammaShapeA)
		{
			ret = data.loadFromCSVFile(buffer, (uint)dataMax, 12, Matrix<>::optFixedSize | Matrix<>::optImmediate, &readBuffer) && ret;
			for (uint y = 0; y != data.height; ++y)
				data[dataCoeffBeta][y] = 1.f / data[dataCoeffBeta][y];
			data.markAsModified();
		}
		else
		{
			// Performing normal loading
			ret = data.loadFromCSVFile(buffer, (uint)dataMax, 12, Matrix<>::optFixedSize, &readBuffer) && ret;
		}

		// Before 4.3, mu could be falsy equal to 24
		if (study.header.version < 430)
			XCastNormalizeMuStrictlyLessThan24(data);

		// K
		buffer.clear() << folder << SEP << "k.txt";
		ret = K.loadFromCSVFile(buffer, 12, 24, Matrix<>::optFixedSize, &readBuffer) && ret;

		uint opts = Matrix<>::optNone;

		// Time-series translation
		buffer.clear() << folder << SEP << "translation.txt";

		ret = translation.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, opts, &readBuffer) && ret;
		if (!JIT::usedFromGUI)
		{
			if (translation.empty())
			{
				// This is not really an error
				useTranslation = tsTranslationNone;
				translation.reset(1, HOURS_PER_YEAR);
			}
			else
			{
				if (translation.width != 1 || translation.height != HOURS_PER_YEAR)
				{
					logs.warning() << folder << ": invalid size for the time-series translation.";
					translation.resizeWithoutDataLost(1, HOURS_PER_YEAR);
					useTranslation = tsTranslationNone;
				}
			}
		}

		opts = Matrix<>::optNone;

		// Transfer function
		buffer.clear() << folder << SEP << "conversion.txt";
		ret = conversion.loadFromCSVFile(buffer, 3, 2, opts, &readBuffer) && ret;
		if (not JIT::enabled)
		{
			if (conversion.width >= 3 && conversion.width <= conversionMaxPoints)
			{
				// We will overwrite the left and the right value
				// Warning !!! std::numeric_limits must not be used
				//  it produces unwanted behavior on Linux
				conversion[0][0] = (float) (- 1.0e+19); // - std::numeric_limits<float>::max();
				conversion[0][1] = conversion[1][1];
				for (uint x = 1; x < conversion.width - 1; ++x)
				{
					if (conversion[x][0] <= -1.0e+19 || conversion[x][0] >= +1.0e+19)
						logs.error() << "TS-Generator: Conversion: Invalid range: " << buffer;
				}
				conversion[conversion.width - 1][0] = (float) 1.0e+19; // + std::numeric_limits<float>::max();
				conversion[conversion.width - 1][1] = conversion[conversion.width - 2][1];
			}
			else
			{
				logs.warning() << "Invalid transfer function: '" << buffer << "'";
				resetTransferFunction();
				useConversion = false;
			}
		}

		// Flush memory to swap files (if support enabled)
		flush();

		return ret;
	}


	void XCast::resetTransferFunction()
	{
		conversion.reset(3, 2);
		conversion[0][0] = (float)(- 1.0e+19); // - std::numeric_limits<float>::max();
		conversion[0][1] = 0.f;
		conversion[1][0] = 0.f;
		conversion[1][1] = 0.f;
		conversion[2][0] = (float)(+ 1.0e+19); // + std::numeric_limits<float>::max();
		conversion[2][1] = 0.f;
	}


	bool XCast::saveToFolder(const AnyString& folder) const
	{
		if (!IO::Directory::Create(folder))
		{
			logs.error() << "I/O Error: Impossible to create '" << folder << "'";
			return false;
		}

		// result
		bool ret = true;
		// A temporary buffer for filename manipulations
		Clob buffer;

		// Coefficients
		buffer.clear() << folder << SEP << "data.txt";
		ret = data.saveToCSVFile(buffer) && ret;

		// K
		buffer.clear() << folder << SEP << "k.txt";
		ret = K.saveToCSVFile(buffer);

		// TimeSeriesAverage
		buffer.clear() << folder << SEP << "translation.txt";
		ret = translation.saveToCSVFile(buffer);

		// Transfer function
		buffer.clear() << folder << SEP << "conversion.txt";
		ret = conversion.saveToCSVFile(buffer);

		// Settings
		IniFile ini;
		IniFile::Section* s = ini.addSection("general");
		if (distribution != dtBeta)
			s->add("distribution", DistributionToNameID(distribution));
		if (!Math::Zero(capacity))
			s->add("capacity",     capacity);
		if (useConversion)
			s->add("conversion",   useConversion);
		if (useTranslation != tsTranslationNone)
			s->add("translation",  TSTranslationUseToCString(useTranslation));

		// Writing the INI file
		buffer.clear() << folder << SEP << "settings.ini";
		if (s->empty())
		{
			// If the section is empty, an empty file will take less
			// disk space and the parsing will be faster (of course)
			return IO::File::CreateEmptyFile(buffer) && ret;
		}
		return ini.save(buffer) && ret;
	}


	void XCast::estimateMemoryUsage(StudyMemoryUsage& u) const
	{
		if (timeSeries & u.study.parameters.timeSeriesToGenerate)
		{
			u.requiredMemoryForInput += sizeof(XCast);
			data.estimateMemoryUsage(u);
			K.estimateMemoryUsage(u);
			translation.estimateMemoryUsage(u);
			conversion.estimateMemoryUsage(u);
		}
	}


	bool XCast::invalidate(bool reload) const
	{
		bool ret = true;
		ret = data.invalidate(reload) && ret;
		ret = K.invalidate(reload) && ret;
		ret = translation.invalidate(reload) && ret;
		ret = conversion.invalidate(reload) && ret;
		return ret;
	}


	void XCast::markAsModified() const
	{
		data.markAsModified();
		K.markAsModified();
		translation.markAsModified();
		conversion.markAsModified();
	}


	void XCast::copyFrom(const XCast& rhs)
	{
		// Coeffs
		data = rhs.data;
		rhs.data.unloadFromMemory();
		// K
		K = rhs.K;
		rhs.K.unloadFromMemory();
		// translation
		translation = rhs.translation;
		rhs.translation.unloadFromMemory();
		useTranslation = rhs.useTranslation;

		// Conversion
		conversion = rhs.conversion;
		rhs.conversion.unloadFromMemory();
		useConversion = rhs.useConversion;

		// capacity
		capacity     = rhs.capacity;
		// distribution
		distribution = rhs.distribution;
	}




} // namespace Data
} // namespace Antares

