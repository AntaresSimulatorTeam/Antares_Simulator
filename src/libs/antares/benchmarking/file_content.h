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
    virtual std::string content() = 0;
protected:
    std::string name_ = "";
};

class TitleLine : public FileContentLine
{
public:
    TitleLine(std::string name) : FileContentLine(name)
    {}
    std::string content() override { return name_; }
};


class FileContentLine_keyValue : public FileContentLine
{
public:
    FileContentLine_keyValue(std::string name) : FileContentLine(name)
    {}
    virtual std::string content()
    {
        return name() + " : " + value();
    }
protected:
    virtual std::string value() = 0;
};

class FileContentLine_intValue : public FileContentLine_keyValue
{
public:
    FileContentLine_intValue(std::string name, unsigned int value) : FileContentLine_keyValue(name), value_(value)
    {}

private:
    std::string value() override { return std::to_string(value_); }
    unsigned int value_ = 0;
};

class FileContentLine_charValue : public FileContentLine_keyValue
{
public:
    FileContentLine_charValue(std::string name, const char* value) 
        : FileContentLine_keyValue(name), value_(value)
    {
    }
    std::string value() override
    {
        return value_;
    }

private:
    const char* value_ = "";
};

class FileContentLine_timeValue : public FileContentLine_keyValue
{
public:
    FileContentLine_timeValue(std::string name, unsigned int duration, int nbCalls)
        : FileContentLine_keyValue(name), duration_(duration), nb_calls_(nbCalls)
    {}

private:
    std::string value() override
    {
        return std::to_string(duration_) + "\t" + std::to_string(nb_calls_);
    }

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

