// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>

#include "opt_period_string_generator.h"

std::shared_ptr<OptPeriodStringGenerator> createOptPeriodAsString(bool isOptimizationWeekly,
                                                                  unsigned int day,
                                                                  unsigned int week,
                                                                  unsigned int year);

std::string createCriterionFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                    const unsigned int optNumber);

std::string createMPSfilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                              const unsigned int optNumber);

std::string createSolutionFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                   const unsigned int optNumber);

std::string createMarginalCostFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                       const unsigned int optNumber);

std::string createReducedCostFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                      const unsigned int optNumber);
