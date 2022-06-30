#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Benchmarking
{
/*
    ===  Study info item ===
*/
class FileContentLine
{
public:
    FileContentLine(std::string name) : name_(name)
    {}
    virtual std::string name() { return name_; }
    virtual std::string value() = 0;
protected:
    std::string name_ = "";
};

/*
class FileContentLine_string : public FileContentLine
{
public:
    FileContentLine_string(std::string name) : FileContentLine(name)
    {}
    std::string value() override { return ""; }
};
*/

class FileContentLine_intValue : public FileContentLine
{
public:
    FileContentLine_intValue(std::string name, unsigned int value) : FileContentLine(name), value_(value)
    {}
    std::string value() override { return std::to_string(value_); }
private:
    unsigned int value_ = 0;
};

class FileContentLine_charValue : public FileContentLine
{
public:
    FileContentLine_charValue(std::string name, const char* value) :
        FileContentLine(name), value_(value)
    {
    }
    std::string value() override
    {
        return value_;
    }

private:
    const char* value_ = "";
};

class FileContentLine_timeValue : public FileContentLine
{
public:
    FileContentLine_timeValue(std::string name, unsigned int duration, int nbCalls)
        : FileContentLine(name), duration_(duration), nb_calls_(nbCalls)
    {}

    std::string value() override
    {
        return std::to_string(duration_) + "\t" + std::to_string(nb_calls_);
    }
private:
    unsigned int duration_ = 0;
    int nb_calls_ = 0;
};

/*
    === class FileContent ===
*/
class FileContent
{
public:
    FileContent() = default;

    using iterator = std::vector<std::unique_ptr<FileContentLine>>::iterator;
    iterator begin();
    iterator end();

    void addItem(FileContentLine* item); 

private:
    // File content lines
    std::vector<std::unique_ptr<FileContentLine>> items_;
};

} // namespace Benchmarking

