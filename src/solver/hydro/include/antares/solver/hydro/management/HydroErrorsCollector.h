/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once
#include <map>
#include <string>

namespace Antares
{

class HydroErrorsCollector
{
public:
    struct AreaSingleErrorMessage
    {
        std::string message = "";
        unsigned int message_number = 0;
    };

    class AreaReference
    {
    public:
        AreaReference(HydroErrorsCollector* collector, const std::string& name);
        template<class T>
        AreaReference& operator<<(const T& msg);

        ~AreaReference()
        {
            // std::cout << areasErrorMap_ << std::endl;
        }

    private:
        AreaSingleErrorMessage& areaSingleErrorMessage_;
    };

    AreaReference operator()(const std::string& name);
    HydroErrorsCollector() = default;
    void CheckForErrors() const;

private:
    // for log
    std::multimap<std::string, AreaSingleErrorMessage> areasErrorMap_;
};

template<class T>
HydroErrorsCollector::AreaReference& HydroErrorsCollector::AreaReference::operator<<(const T& msg)
{
    std::ostringstream strfy;
    strfy << msg;
    areaSingleErrorMessage_.message += strfy.str();
    return *this;
}

} // namespace Antares
