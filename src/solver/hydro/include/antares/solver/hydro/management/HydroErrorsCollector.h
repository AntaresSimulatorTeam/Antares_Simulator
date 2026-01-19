// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Antares
{

class HydroErrorsCollector final
{
public:
    class AreaReference final
    {
    public:
        AreaReference(HydroErrorsCollector* collector, const std::string& name);
        template<class T>
        AreaReference& operator<<(const T& msg);

    private:
        std::string& areaSingleErrorMessage_;
    };

    AreaReference operator()(const std::string& name);
    HydroErrorsCollector() = default;
    void CheckForErrors() const;

private:
    std::map<std::string, std::vector<std::string>> areasErrorMap_;
    std::string& CurrentMessage(const std::string& name);
};

template<class T>
HydroErrorsCollector::AreaReference& HydroErrorsCollector::AreaReference::operator<<(const T& msg)
{
    std::ostringstream strfy;
    strfy << msg;
    areaSingleErrorMessage_ += strfy.str();
    return *this;
}

} // namespace Antares
