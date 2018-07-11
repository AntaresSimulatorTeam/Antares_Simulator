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

#include "layer.h"

namespace Antares
{
namespace Window
{
namespace OutputViewer
{


	Layer::Layer(LayerType t) :
		index((uint) -1),
		type(t),
		detached(false),
		customSelectionType(stNone)
	{
		resetAlternativeCaption();
	}


	Layer::Layer(const Layer& rhs) :
		index(rhs.index),
		selection(rhs.selection),
		type(rhs.type),
		detached(rhs.detached),
		customSelectionType(rhs.customSelectionType),
		customAreaOrLink(rhs.customAreaOrLink)
	{
		resetAlternativeCaption();
	}


	Layer::~Layer()
	{
	}


	void Layer::resetAlternativeCaption()
	{
		switch (type)
		{
			case ltOutput:     pAlternativeCaption.clear();break;
			case ltSum:        pAlternativeCaption = wxT("Sum");break;
			case ltDiff:       pAlternativeCaption = wxT("Differences");break;
			case ltAverage:    pAlternativeCaption = wxT("Average");break;
			case ltMin:        pAlternativeCaption = wxT("Minimum");break;
			case ltMax:        pAlternativeCaption = wxT("Maximum");break;
		}
	}






} // namespace OutputViewer
} // namespace Window
} // namespace Antares


