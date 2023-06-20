#pragma once

#include <antares/wx-wrapper.h>

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
extern const unsigned int thermalLawCount;
extern const wxChar* const thermalLaws[];

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

extern const wxChar* const studyMode[];
extern const wxChar* const adequacyPatchMode[];
} // namespace Inspector
} // namespace Window
} // namespace Antares
