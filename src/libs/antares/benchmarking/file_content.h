#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Benchmarking
{

class FileContent
{
public:
    FileContent() = default;

    using iterator = std::vector<std::string>::iterator;
    iterator begin();
    iterator end();

    void addTitleLine(std::string title);
    void addDurationLine(std::string name, unsigned int duration, int nbCalls);
    void addNameValueLine(std::string name, int value);
    void addNameValueLine(std::string name, std::string value);

    std::vector<std::string> content() { return lines_; }
private:
    // File content lines
    std::vector<std::string> lines_;
};

} // namespace Benchmarking

