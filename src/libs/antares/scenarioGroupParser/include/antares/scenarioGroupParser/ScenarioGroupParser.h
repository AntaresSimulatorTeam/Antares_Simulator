// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <BaseErrorListener.h>
#include <string>

namespace Antares
{
class ScenarioGroupParser final
{
public:
    struct Line
    {
        std::string groupName;
        int year{0};
        int timeSeriesNumber{0};
    };

    Line parseLine(const std::string& line, antlr4::BaseErrorListener* errorListener = nullptr);
};
} // namespace Antares
