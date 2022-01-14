#pragma once

#include <stdexcept>

namespace Error
{
class ReadingStudy : public std::runtime_error
{
public:
    explicit ReadingStudy() : std::runtime_error("Got a fatal error reading the study.")
    {
    }
};

class NoAreas : public std::runtime_error
{
public:
    explicit NoAreas() :
     std::runtime_error("No area found. A valid study contains contains at least one.")
    {
    }
};

class InvalidFileName : public std::runtime_error
{
public:
    explicit InvalidFileName() : std::runtime_error("Invalid file names detected.")
    {
    }
};

class RuntimeInfoInitialization : public std::runtime_error
{
public:
    explicit RuntimeInfoInitialization() : std::runtime_error("Error initializing runtime infos.")
    {
    }
};
} // namespace Error
