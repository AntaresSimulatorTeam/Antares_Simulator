
#pragma once

#include <set>
#include <string>
#include <vector>

namespace Antares::Data::ShortTermStorage
{

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField);

}
