// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "settings.h"

namespace Antares::Map
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

} // namespace Antares::Map
