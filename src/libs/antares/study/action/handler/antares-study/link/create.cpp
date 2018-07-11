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

#include "create.h"
#include "../../../../../utils.h"
#include "ntc.h"
#include "hurdles-cost.h"
#include "impedances.h"
#include "comments.h"
#include "copper-plate.h"
#include "unload-data.h"
#include "filtering.h"

using namespace Yuni;



namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Link
{



	Create::Create(const Data::AreaName& fromarea, const Data::AreaName& toarea) :
		pOriginalFromAreaName(fromarea),
		pOriginalToAreaName(toarea)
	{
		pInfos.behavior = bhOverwrite;
	}


	Create::Create(const Data::AreaName& fromarea, const Data::AreaName& targetfromarea,
			const Data::AreaName& toarea, const Data::AreaName& targettoarea) :
		pOriginalFromAreaName(fromarea),
		pTargetFromAreaName(targetfromarea),
		pOriginalToAreaName(toarea),
		pTargetToAreaName(targettoarea)
	{
		pInfos.behavior = bhOverwrite;
	}


	Create::~Create()
	{}


	bool Create::prepareWL(Context& ctx)
	{
		bool explicitTarget = true;

		if (!pTargetFromAreaName)
		{
			pTargetFromAreaName = ctx.areaNameMapping[pOriginalFromAreaName];
			explicitTarget = false;
		}
		if (!pTargetToAreaName)
		{
			pTargetToAreaName = ctx.areaNameMapping[pOriginalToAreaName];
			explicitTarget = false;
		}

		Data::AreaName from;
		Data::AreaName to;
		if (!explicitTarget)
		{
			from = pTargetFromAreaName;
			to = pTargetToAreaName;
		}

		if (!from)
			from = pOriginalFromAreaName;
		if (!to)
			to = pOriginalToAreaName;

		pInfos.message.clear();
		pInfos.caption.clear() << "Link " << from << " / " << to;
		// Default state
		pInfos.state = stReady;

		Antares::Data::Area* areaFrom;
		Antares::Data::Area* areaTo;
		Data::AreaName id;
		TransformNameIntoID(from, id);
		areaFrom = ctx.study.areas.find(id);
		id.clear();
		TransformNameIntoID(to, id);
		areaTo = ctx.study.areas.find(id);

		if (areaFrom && areaTo)
		{
			if (areaFrom->findLinkByID(areaTo->id) || areaTo->findLinkByID(areaFrom->id))
			{
				switch (pInfos.behavior)
				{
					case bhMerge:
						pInfos.message.clear() << "The link " << from << " / " << to << " already exists and will remain untouched";
						break;
					case bhOverwrite:
						pInfos.message.clear() << "The link " << from << " / " << to << " will be reset";
						break;
					default:
						break;
				}
			}
			else
			{
				pInfos.message.clear() << "The link " << from << " / " << to << " will be created";
			}
		}
		else
			pInfos.message.clear() << "The link " << from << " / " << to << " will be created";
		return true;
	}


	bool Create::performWL(Context& ctx)
	{
		Data::AreaName from = pTargetFromAreaName;
		Data::AreaName to   = pTargetToAreaName;
		if (!from)
			from = pOriginalFromAreaName;
		if (!to)
			to = pOriginalToAreaName;

		ctx.message.clear() << "Updating the link " << from << " / " << to;
		ctx.updateMessageUI(ctx.message);

		Antares::Data::Area* areaFrom;
		Antares::Data::Area* areaTo;
		Data::AreaName id;
		TransformNameIntoID(from, id);
		areaFrom = ctx.study.areas.find(id);
		id.clear();
		TransformNameIntoID(to, id);
		areaTo = ctx.study.areas.find(id);

		if (areaFrom && areaTo)
		{
			ctx.link = areaFrom->findLinkByID(areaTo->id);
			if (!ctx.link)
				ctx.link = areaTo->findLinkByID(areaFrom->id);
			if (!ctx.link)
			{
				logs.debug() << "[study-action] The link " << areaFrom->id << " - " << areaTo->id << " has been created";
				ctx.link = AreaListAddLink(&(ctx.study.areas), areaFrom->id.c_str(), areaTo->id.c_str(), false);
				ctx.link->resetToDefaultValues();
				ctx.autoselectLinks.push_back(ctx.link);
				return true;
			}
			logs.debug() << "[study-action] link " << areaFrom->id << " - " << areaTo->id;
			ctx.autoselectLinks.push_back(ctx.link);

			switch (pInfos.behavior)
			{
				case bhOverwrite:
					logs.debug() << "[study-action] the link " << areaFrom->id << " - " << areaTo->id << " has been reset";
					ctx.link->resetToDefaultValues();
					break;
				default:
					break;
			}
			return true;
		}

		return false;
	}


	void Create::createActionsForAStandardLinkCopy(Context&)
	{
		// NTC
		*this += new NTC(pOriginalFromAreaName, pOriginalToAreaName);
		// Impedances
		*this += new Impedances(pOriginalFromAreaName, pOriginalToAreaName);
		// Hurdles-cost
		*this += new HurdlesCost(pOriginalFromAreaName, pOriginalToAreaName);
		// Copper-plate
		*this += new CopperPlate(pOriginalFromAreaName, pOriginalToAreaName);
		// Comments
		*this += new Comments(pOriginalFromAreaName, pOriginalToAreaName);
		// Unload
		*this += new UnloadData(pOriginalFromAreaName, pOriginalToAreaName);
		// filtering
		*this += new Filtering(pOriginalFromAreaName, pOriginalToAreaName);
	}





} // namespace Link
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

