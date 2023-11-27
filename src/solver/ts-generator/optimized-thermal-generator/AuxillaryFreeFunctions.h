//
// Created by milos on 14/11/23.
//

#pragma once

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// auxillary functions
std::array<double, DAYS_PER_YEAR> calculateDailySums(
  const std::array<double, HOURS_PER_YEAR>& hourlyValues);
std::array<double, HOURS_PER_YEAR> calculateAverageTs(const Matrix<double>& tsValue,
                                                      const Matrix<uint32_t>& tsNumbers);
bool checkClusterExist(const Data::ThermalCluster& cluster);
int dayOfTheYear(int optimizationDay);

// calculate Average time-series functions
std::array<double, HOURS_PER_YEAR> calculateAverageLoadTs(const Data::Area& area);
std::array<double, HOURS_PER_YEAR> calculateAverageRorTs(const Data::Area& area);
std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTs(
  const Data::Parameters::RenewableGeneration modelingType,
  const Data::Area& area);
std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsAggregated(const Data::Area& area);
std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsClusters(const Data::Area& area);

// calculate parameters functions - per cluster
int calculateNumberOfMaintenances(const Data::ThermalCluster& cluster, int timeHorizon);
int calculateAverageMaintenanceDuration(const Data::ThermalCluster& cluster);
std::array<double, DAYS_PER_YEAR> calculateMaxUnitOutput(const Data::ThermalCluster& cluster);
std::array<double, DAYS_PER_YEAR> calculateAvrUnitDailyCost(const Data::ThermalCluster& cluster);

// post-time step optimization - functions
int findFirstOne(const std::vector<int> vector, int limit);

} // namespace Antares::Solver::TSGenerator
