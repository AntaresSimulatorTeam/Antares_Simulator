#pragma once

#include <antares/wx-wrapper.h>

namespace Antares
{
namespace Window
{
namespace Inspector
{
// Min up/down time for thermal clusters
extern const wxChar* arrayMinUpDownTime[];

// Thermal clusters
extern const unsigned int arrayClusterGroupCount;
extern const wxChar* arrayClusterGroup[];

// Thermal laws
extern const unsigned int thermalLawCount;
extern const wxChar* thermalLaws[];

// Thermal TS generation
extern const unsigned int localGenTSCount;
extern const wxChar* localGenTS[];

// Renewable clusters
extern const unsigned int arrayRnClusterGroupCount;
extern const wxChar* arrayRnClusterGroup[];
extern const unsigned int renewableTSModeCount;
extern const wxChar* renewableTSMode[];

// Calendar
extern const wxChar* weekday[];
extern const wxChar* calendarMonths[];
extern const wxChar* calendarWeeks[];

extern const wxChar* buildingMode[];
extern const wxChar* playlist[];
// Trimming
extern const wxChar* geographicTrimming[];
extern const wxChar* thematicTrimming[];

extern const wxChar* studyMode[];
} // namespace Inspector
} // namespace Window
} // namespace Antares
