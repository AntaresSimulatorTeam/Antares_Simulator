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
#include <antares/study.h>
#include "studydata.h"

using namespace Yuni;


namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{

	StudyData::StudyData() :
		mode(Data::Correlation::modeNone)
	{
		for (uint realmonth = 0; realmonth != 12; ++realmonth)
			correlation[realmonth] = nullptr;
	}


	StudyData::~StudyData()
	{
		switch (mode)
		{
			case Data::Correlation::modeMonthly:
				{
					for (uint realmonth = 0; realmonth != 12; ++realmonth)
						delete correlation[realmonth];
					break;
				}
			case Data::Correlation::modeAnnual:
				{
					delete correlation[0];
					break;
				}
			case Data::Correlation::modeNone:
				break;
		}
	}


	void StudyData::prepareMatrix(Matrix<float>& m, const Matrix<float>& source) const
	{
		uint areaCount = (uint) localareas.size();
		m.resize(areaCount, areaCount);
		m.fillUnit();

		for (uint x = 1; x < m.width; ++x)
		{
			uint areaXindx = localareas[x]->index;
			auto& sourceX = source[areaXindx];
			auto& mX = m[x];

			for (uint y = 0; y < x; ++y)
			{
				uint areaYindx = localareas[y]->index;
				float d = sourceX[areaYindx];
				mX[y] = d;
				m[y][x] = d;
			}
		}
		m.flush();
		source.flush();
	}


	void StudyData::reloadDataFromAreaList(const Data::Correlation& originalCorrelation)
	{
		
		mode = originalCorrelation.mode();

		if (!localareas.empty())
		{
			switch (mode)
			{
				case Data::Correlation::modeAnnual:
					{
						
						auto* m = new Matrix<float>();
						prepareMatrix(*m, *(originalCorrelation.annual));

						for (uint realmonth = 0; realmonth != 12; ++realmonth)
							correlation[realmonth] = m;
						break;
					}
				case Data::Correlation::modeMonthly:
					{
						
						for (uint realmonth = 0; realmonth != 12; ++realmonth)
						{
							auto* m = new Matrix<float>();
							correlation[realmonth] = m;
							prepareMatrix(*m, originalCorrelation.monthly[realmonth]);
						}
						break;
					}
				case Data::Correlation::modeNone:
					break;
			} 
		}
		else
		{
			for (uint realmonth = 0; realmonth != 12; ++realmonth)
				correlation[realmonth] = nullptr;
		}
	}





} 
} 
} 
} 

