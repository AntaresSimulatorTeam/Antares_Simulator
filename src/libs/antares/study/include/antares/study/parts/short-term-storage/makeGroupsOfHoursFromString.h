
#pragma once

#include <string>
#include <vector>

namespace Antares::Data::ShortTermStorage
{

std::vector<std::vector<unsigned>> makeGroupsOfHours(const std::string& hoursField);

}
