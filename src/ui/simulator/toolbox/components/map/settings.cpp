/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "settings.h"

namespace Antares
{
namespace Map
{
wxColour Settings::background = wxColour(235, 235, 235);

wxColour Settings::text = wxColour(30, 30, 30);
wxColour Settings::textLight = wxColour(80, 80, 80);

wxColour Settings::grid = wxColour(225, 225, 225);
wxColour Settings::gridCenter = wxColour(235, 177, 138);

wxColour Settings::defaultNodeBackground = wxColour(223, 136, 72);
wxColour Settings::defaultNodeShadow = wxColour(160, 160, 160);

wxColour Settings::selectionNodeBorder = wxColour(134, 163, 226);
wxColour Settings::selectionNodeBackground = wxColour(114, 143, 206);

wxColour Settings::clusterBorder = wxColour(20, 180, 20);
wxColour Settings::clusterBackground = wxColour(140, 255, 140);
wxColour Settings::clusterBorderSelected = wxColour(180, 20, 20);
wxColour Settings::clusterBackgroundSelected = wxColour(255, 140, 140);

wxColour Settings::selectionBoxBorder = wxColour(200, 203, 255);
wxColour Settings::selectionBoxBackground = wxColour(220, 220, 255);
wxColour Settings::selectionBoxBorderHighlighted = wxColour(255, 200, 203);
wxColour Settings::selectionBoxBackgroundHighlighted = wxColour(255, 220, 220);

wxColour Settings::mouseSelectionBorder = wxColour(134, 163, 226);
wxColour Settings::mouseSelectionBackground = wxColour(184, 213, 255);

wxColour Settings::connection = wxColour(130, 120, 120);
wxColour Settings::connectionHighlighted = wxColour(255, 30, 30);

} // namespace Map
} // namespace Antares
