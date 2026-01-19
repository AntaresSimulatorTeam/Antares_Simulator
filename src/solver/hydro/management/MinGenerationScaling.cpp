// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/management/MinGenerationScaling.h"

#include <numeric>

namespace Antares
{
MinGenerationScaling::MinGenerationScaling(Data::AreaList& areas, const Date::Calendar& calendar):
    areas_(areas),
    calendar_(calendar)
{
}

void MinGenerationScaling::Run(uint year)
{
    areas_.each(
      // un-const because now data is a member of area  [&](const Data::Area& area)
      [this, &year](Data::Area& area)
      {
          const auto& srcmingen = area.hydro.series->mingen.getColumn(year);

          auto& data = area.hydro.managementData[year];
          double totalYearMingen = 0.0;

          for (uint month = 0; month != 12; ++month)
          {
              uint realmonth = calendar_.months[month].realmonth;
              uint firstDayOfMonth = calendar_.months[month].daysYear.first;
              uint firstDayOfNextMonth = calendar_.months[month].daysYear.end;

              double totalMonthMingen = std::accumulate(srcmingen + firstDayOfMonth * 24,
                                                        srcmingen + firstDayOfNextMonth * 24,
                                                        0.);

              data.totalMonthMingen[realmonth] = totalMonthMingen;
              totalYearMingen += totalMonthMingen;

              if (!(area.hydro.reservoirCapacity < 1e-4))
              {
                  if (area.hydro.reservoirManagement)
                  {
                      // Set monthly mingen, used later for h2o_m
                      data.mingens[realmonth] = totalMonthMingen / (area.hydro.reservoirCapacity);
                      assert(!std::isnan(data.mingens[month]) && "nan value detect in mingen");
                  }
                  else
                  {
                      data.mingens[realmonth] = totalMonthMingen;
                  }
              }
              else
              {
                  data.mingens[realmonth] = totalMonthMingen;
              }

              // Set daily mingen, used later for h2o_d
              uint simulationMonth = calendar_.mapping.months[realmonth];
              auto daysPerMonth = calendar_.months[simulationMonth].days;
              uint firstDay = calendar_.months[simulationMonth].daysYear.first;
              uint endDay = firstDay + daysPerMonth;

              for (uint day = firstDay; day != endDay; ++day)
              {
                  data.dailyMinGen[day] = std::accumulate(srcmingen + day * 24,
                                                          srcmingen + day * 24 + 24,
                                                          0.);
              }
          }
          data.totalYearMingen = totalYearMingen;
      });
}
} // namespace Antares
