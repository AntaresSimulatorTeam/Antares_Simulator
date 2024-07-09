
#include "antares/solver/hydro/management/PrepareInflows.h"

namespace Antares
{

PrepareInflows::PrepareInflows(Data::AreaList& areas, const Date::Calendar& calendar):
    areas_(areas),
    calendar_(calendar)
{
}

void PrepareInflows::Run(uint year)
{
    LoadInflows(year);
    ChangeInflowsToAccommodateFinalLevels(year);
}

void PrepareInflows::LoadInflows(uint year)
{
    areas_.each(
      [this, year](Data::Area& area)
      {
          const auto& srcinflows = area.hydro.series->storage.getColumn(year);

          auto& data = area.hydro.managementData[year];
          double totalYearInflows = 0.0;

          for (uint month = 0; month != 12; ++month)
          {
              uint realmonth = calendar_.months[month].realmonth;

              double totalMonthInflows = 0.0;

              uint firstDayOfMonth = calendar_.months[month].daysYear.first;

              uint firstDayOfNextMonth = calendar_.months[month].daysYear.end;

              for (uint d = firstDayOfMonth; d != firstDayOfNextMonth; ++d)
              {
                  totalMonthInflows += srcinflows[d];
              }

              data.totalMonthInflows[realmonth] = totalMonthInflows;
              totalYearInflows += totalMonthInflows;

              if (not(area.hydro.reservoirCapacity < 1e-4))
              {
                  if (area.hydro.reservoirManagement)
                  {
                      data.inflows[realmonth] = totalMonthInflows / (area.hydro.reservoirCapacity);
                      assert(!std::isnan(data.inflows[month]) && "nan value detect in inflows");
                  }
                  else
                  {
                      data.inflows[realmonth] = totalMonthInflows;
                  }
              }
              else
              {
                  data.inflows[realmonth] = totalMonthInflows;
              }
          }
          data.totalYearInflows = totalYearInflows;
      });
}

void PrepareInflows::ChangeInflowsToAccommodateFinalLevels(uint year)
{
    areas_.each(
      [&year](Data::Area& area)
      {
          auto& data = area.hydro.managementData[year];

          if (!area.hydro.deltaBetweenFinalAndInitialLevels[year].has_value())
          {
              return;
          }

          // Must be done before prepareMonthlyTargetGenerations
          double delta = area.hydro.deltaBetweenFinalAndInitialLevels[year].value();
          if (delta < 0)
          {
              data.inflows[0] -= delta;
          }
          else if (delta > 0)
          {
              data.inflows[11] -= delta;
          }
      });
}

} // namespace Antares
