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

#include "color.h"


using namespace Yuni;


namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{

	Color::Color(const AnyString& areaname) :
		pOriginalAreaName(areaname)
	{
		pInfos.caption << "Color of the area";
		pInfos.behavior = bhSkip;
	}


	Color::~Color()
	{}


	bool Color::prepareWL(Context&)
	{
		pInfos.message.clear();
		pInfos.state = stReady;
		switch (pInfos.behavior)
		{
			case bhOverwrite:
				pInfos.message << "The color of the area will be copied";
				break;
			default:
				pInfos.state = stDisabled;
				break;
		}

		return true;
	}


	bool Color::performWL(Context& ctx)
	{
		if (ctx.area && ctx.extStudy && ctx.area->ui)
		{
			auto* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
			if (source && source->ui)
			{
				// color
				for (uint i = 0; i != 3; ++i)
					ctx.area->ui->color[i] = source->ui->color[i];
				// even the cache
				ctx.area->ui->cacheColorHSV = source->ui->cacheColorHSV;
				return true;
			}
		}
		return false;
	}





} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

