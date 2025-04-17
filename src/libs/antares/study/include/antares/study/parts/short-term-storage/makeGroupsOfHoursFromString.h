
#pragma once

#include <string>
#include <vector>
#include <set>

namespace Antares::Data::ShortTermStorage
{

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField);

}
