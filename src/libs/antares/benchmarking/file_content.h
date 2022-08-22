#pragma once

#include <string>
#include <map>

namespace Benchmarking
{

class FileContent
{
public:
    FileContent() = default;

    using iterator = std::map<std::string, std::map<std::string, std::string>>::iterator;
    iterator firstSection();
    iterator endSections();

    void addItemToSection(const std::string& section, const std::string& key, int value);
    void addItemToSection(const std::string& section, const std::string& key, const std::string& value);
    void addDurationItem(const std::string& name, unsigned int duration, int nbCalls);

private:
    // Data of the file content
    std::map<std::string,                           // Sections as keys
             std::map<std::string, std::string>>    // Section parameters as name / value
             sections_;

};

} // namespace Benchmarking

