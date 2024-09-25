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
#include <mutex>
#include <string>

namespace Benchmarking
{
class FileContent
{
public:
    FileContent() = default;

    using iterator = std::map<std::string, std::map<std::string, std::string>>::iterator;
    iterator end();
    iterator begin();

    void addItemToSection(const std::string& section, const std::string& key, int value);
    void addItemToSection(const std::string& section,
                          const std::string& key,
                          const std::string& value);
    void addDurationItem(const std::string& name, unsigned int duration, int nbCalls);

    std::string saveToBufferAsIni();

private:
    std::mutex pSectionsMutex;
    // Data of the file content
    std::map<std::string,                        // Sections as keys
             std::map<std::string, std::string>> // Section parameters as name / value
      sections_;
};
} // namespace Benchmarking
