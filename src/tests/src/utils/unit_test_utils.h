// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
