#pragma once

#include <string>
#include <map>

#include <memory>

namespace Benchmarking
{

class FileContent
{
public:
    FileContent() = default;

    using iterator = std::map<std::string, std::map<std::string, std::string>>::iterator;
    iterator firstSection();
    iterator endSections();

    void addItemToSection(std::string section, std::string key, int value);
    void addItemToSection(std::string section, std::string key, std::string value);
    void addDurationItem(std::string name, unsigned int duration, int nbCalls);

private:
    // Data of the file content
    std::map<std::string, std::map<std::string, std::string>> sections_;

};

} // namespace Benchmarking

