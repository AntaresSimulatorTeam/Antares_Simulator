// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_CONSTANTS_H__
#define __ANTARES_LIBS_CONSTANTS_H__

#include <array>

const unsigned int HOURS_PER_DAY = 24;

const unsigned int MONTHS_PER_YEAR = 12;
const unsigned int WEEKS_PER_YEAR = 53;
const unsigned int DAYS_PER_YEAR = 365;
const unsigned int HOURS_PER_YEAR = 8760;

namespace Antares::Constants
{
constexpr std::array<unsigned int, MONTHS_PER_YEAR>
  daysPerMonth = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr unsigned int nbHoursInAWeek = 168;
constexpr double noiseAmplitude = 1e-3;

} // namespace Antares::Constants

enum AntaresConstants
{
    ant_k_area_name_max_length = 128,
    ant_k_cluster_name_max_length = 128,
    ant_k_constraint_name_max_length = 128,
};

#endif /* __ANTARES_LIBS_CONSTANTS_H__ */
