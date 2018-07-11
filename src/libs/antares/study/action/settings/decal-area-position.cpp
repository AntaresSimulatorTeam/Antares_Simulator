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

#include "decal-area-position.h"


namespace Antares
{
namespace Action
{
namespace Settings
{

	DecalAreaPosition::DecalAreaPosition(bool enabled)
		:pValueX("25"), pValueY("-20")
	{
		pInfos.caption = "Decal area coordinates";
		if (!enabled)
			pInfos.behavior = bhSkip;
	}



	void DecalAreaPosition::prepareSkipWL(Context& ctx)
	{
		// reset the property
		ctx.property["area.coordinate.x.offset"].clear();
		ctx.property["area.coordinate.y.offset"].clear();
	}


	bool DecalAreaPosition::prepareWL(Context& ctx)
	{
		ctx.property["area.coordinate.x.offset"] = pValueX;
		ctx.property["area.coordinate.y.offset"] = pValueY;

		if (!pValueX && !pValueY)
		{
			pInfos.state = stDisabled;
		}
		else
		{
			pInfos.message.clear();
			pInfos.state = stReady;
			if (not pValueX.empty())
				pInfos.message << "x: +" << pValueX << "   ";
			if (not pValueY.empty())
				pInfos.message << "y: +" << pValueY;
		}
		return true;
	}


	void DecalAreaPosition::behaviorToText(Behavior behavior, Yuni::String& out)
	{
		switch (behavior)
		{
			case bhOverwrite:
				out = "enabled";
				break;
			case bhMerge:
				out = "enabled";
				break;
			case bhSkip:
				out = "skip";
				break;
			case bhMax:
				out.clear();
				break;
		}
	}




} // namespace Settings
} // namespace Action
} // namespace Antares

