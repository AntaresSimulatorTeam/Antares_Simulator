#pragma once

#include <string>
#include <memory>
#include <optional>
#include "../optimisation/opt_period_string_generator_base.h"
#include <yuni/core/string.h> // TODO[FOM] Remove this include

// TODO[FOM] Remove this function
std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     uint year,
                                     uint week);

std::shared_ptr<OptPeriodStringGenerator> createOptPeriodAsString(bool isOptimizationWeekly,
                                                                  unsigned int day,
                                                                  unsigned int week,
                                                                  unsigned int year);

std::string createCriterionFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                    const unsigned int optNumber,
                                    const std::optional<unsigned int> adqPatchOptNumber
                                    = std::nullopt);

std::string createMPSfilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                              const unsigned int optNumber,
                              const std::optional<unsigned int> adqPatchOptNumber = std::nullopt);
