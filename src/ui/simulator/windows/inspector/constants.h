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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <wx/wx.h>

namespace Antares
{
namespace Window
{
namespace Inspector
{
// Min up/down time for thermal clusters
extern const wxChar* const arrayMinUpDownTime[];

// Thermal clusters
extern const unsigned int arrayClusterGroupCount;
extern const wxChar* const arrayClusterGroup[];

// Thermal laws
extern const unsigned int LawCount;
extern const wxChar* const Laws[];

// Thermal cost generation
extern const unsigned int costgenerationCount;
extern const wxChar* const costgeneration[];

// Thermal TS generation
extern const unsigned int localGenTSCount;
extern const wxChar* const localGenTS[];

// Renewable clusters
extern const unsigned int arrayRnClusterGroupCount;
extern const wxChar* const arrayRnClusterGroup[];
extern const unsigned int renewableTSModeCount;
extern const wxChar* const renewableTSMode[];

// Calendar
extern const wxChar* const weekday[];
extern const wxChar* const calendarMonths[];
extern const wxChar* const calendarWeeks[];

extern const wxChar* const buildingMode[];
extern const wxChar* const playlist[];
// Trimming
extern const wxChar* const geographicTrimming[];
extern const wxChar* const thematicTrimming[];

extern const wxChar* const simulationMode[];
extern const wxChar* const adequacyPatchMode[];
} // namespace Inspector
} // namespace Window
} // namespace Antares
