/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <functional>
#include <set>
#include <string>

#include "yuni/core/event/interfaces.h"

std::function<bool(const std::exception&)> checkMessage(std::string expected_message);
std::function<bool(const std::exception&)> containsMessage(std::string expected_message);

namespace Antares::UnitTests
{
class CaptureAntaresLogs
    : public Yuni::IEventObserver<CaptureAntaresLogs, Yuni::Policy::SingleThreaded>
{
public:
    CaptureAntaresLogs();
    ~CaptureAntaresLogs();

    const std::set<std::string>& getFatals() const;
    const std::set<std::string>& getErrors() const;
    const std::set<std::string>& getWarnings() const;
    const std::set<std::string>& getInfos() const;

private:
    void onLogMessage(int level, const std::string& message);

    std::set<std::string> fatals_;
    std::set<std::string> errors_;
    std::set<std::string> warnings_;
    std::set<std::string> infos_;
};
} // namespace Antares::UnitTests
