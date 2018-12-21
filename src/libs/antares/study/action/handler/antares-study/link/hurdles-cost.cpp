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

#include "hurdles-cost.h"
#include "../../../../area/constants.h"


namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Link
{

	HurdlesCost::HurdlesCost(const AnyString& fromarea, const AnyString& toarea) :
		pOriginalFromAreaName(fromarea),
		pOriginalToAreaName(toarea)
	{
		pInfos.caption << "Hurdles-Cost";
	}


	HurdlesCost::~HurdlesCost()
	{}


	bool HurdlesCost::prepareWL(Context&)
	{
		pInfos.message.clear();
		pInfos.state = stReady;
		switch (pInfos.behavior)
		{
			case bhOverwrite:
				pInfos.message << "The hurdles-cost will be copied";
				break;
			default:
				pInfos.state = stNothingToDo;
				break;
		}

		return true;
	}


	bool HurdlesCost::performWL(Context& ctx)
	{
		if (ctx.link && ctx.extStudy)
		{
			Data::AreaName idFrom;
			Data::AreaName idTo;
			TransformNameIntoID(pOriginalFromAreaName, idFrom);
			TransformNameIntoID(pOriginalToAreaName,   idTo);

			Data::AreaLink* source;
			if (pOriginalFromAreaName < pOriginalToAreaName)
				source = ctx.extStudy->areas.findLink(idFrom, idTo);
			else
				source = ctx.extStudy->areas.findLink(idTo, idFrom);

			if (source && source != ctx.link)
			{
				source->data.invalidate(true);
				ctx.link->data.invalidate(true);

				ctx.link->data.pasteToColumn((uint) Data::fhlHurdlesCostDirect,
					source->data.entry[Data::fhlHurdlesCostDirect]);
				ctx.link->data.pasteToColumn((uint) Data::fhlHurdlesCostIndirect,
					source->data.entry[Data::fhlHurdlesCostIndirect]);
				ctx.link->useHurdlesCost = source->useHurdlesCost;

				ctx.link->assetType = source->assetType;
				return true;
			}
		}
		return false;
	}





} // namespace Link
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

