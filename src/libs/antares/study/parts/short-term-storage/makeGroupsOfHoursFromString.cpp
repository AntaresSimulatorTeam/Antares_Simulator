
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"

#include <regex>

namespace Antares::Data::ShortTermStorage
{
static std::vector<std::string> splitIntoGroups(const std::string& hoursField)
{
    std::vector<std::string> my_groups;

    std::regex betweenSquareBrackets(R"(\[(.*?)\])");
    auto matchBegin = std::sregex_iterator(hoursField.begin(),
                                           hoursField.end(),
                                           betweenSquareBrackets);
    auto matchEnd = std::sregex_iterator();
    for (auto it = matchBegin; it != matchEnd; ++it)
    {
        my_groups.push_back((*it)[1].str());
    }

    return my_groups;
}

static std::vector<std::vector<std::string>> splitGroupsIntoHoursAsString(
  const std::vector<std::string>& groups)
{
    std::vector<std::vector<std::string>> to_return;

    std::regex commaSeparatedDigits(R"(^\s*\d+\s*(,\s*\d+)*\s*$)");
    for (const auto& s: groups)
    {
        if (!std::regex_match(s, commaSeparatedDigits))
        {
            throw std::invalid_argument("splitting each group into hours > wrong format");
        }
        std::vector<std::string> hoursAsStrings;
        std::regex catchDigit(R"(\d+)");
        auto matchBegin = std::sregex_iterator(s.begin(), s.end(), catchDigit);
        auto matchEnd = std::sregex_iterator();
        for (auto it = matchBegin; it != matchEnd; ++it)
        {
            hoursAsStrings.push_back((*it)[0].str());
        }
        to_return.push_back(hoursAsStrings);
    }
    return to_return;
}

static std::vector<std::vector<unsigned>> toGroupsOfHours(
  const std::vector<std::vector<std::string>>& groups)
{
    std::vector<std::vector<unsigned>> groupsOfHours;
    for (const auto& group: groups)
    {
        std::vector<unsigned> groupOfInts;
        for (const std::string& h: group)
        {
            int hour = std::stoi(h);
            groupOfInts.push_back(hour);
        }
        groupsOfHours.push_back(groupOfInts);
    }
    return groupsOfHours;
}

std::vector<std::vector<unsigned>> makeGroupsOfHours(const std::string& hoursField)
{
    auto groups = splitIntoGroups(hoursField);
    auto groupsOfHoursAsStrings = splitGroupsIntoHoursAsString(groups);
    return toGroupsOfHours(groupsOfHoursAsStrings);
}
} // namespace Antares::Data::ShortTermStorage
