// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <mutex>
#include <string>

namespace Benchmarking
{
class FileContent final
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
    void addDurationItem(const std::string& name,
                         unsigned int duration_ms,
                         const std::string& duration,
                         int nbCalls);

    std::string saveToBufferAsIni();

private:
    std::mutex pSectionsMutex;
    // Data of the file content
    std::map<std::string,                        // Sections as keys
             std::map<std::string, std::string>> // Section parameters as name / value
      sections_;
};
} // namespace Benchmarking
