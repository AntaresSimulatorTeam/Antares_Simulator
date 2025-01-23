#pragma once

#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{

class ScGroup_AlreadyExists: public std::invalid_argument
{
public:
    ScGroup_AlreadyExists(const std::string& groupId);
};

class ScGroup_DoesNotExist: public std::invalid_argument
{
public:
    ScGroup_DoesNotExist(const std::string& groupId);
};

class ScGroup_ScenarioNotExist: public std::invalid_argument
{
public:
    ScGroup_ScenarioNotExist(const std::string& groupId, const unsigned scenario);
};

} // namespace Antares::Solver::Modeler::DataSeries
