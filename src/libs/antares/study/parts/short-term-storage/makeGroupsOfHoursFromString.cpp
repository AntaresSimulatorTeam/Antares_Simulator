
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"

#include <algorithm>
#include <regex>

namespace Antares::Data::ShortTermStorage
{

static bool onlyCommasOrSpacesOutsideBrackets(const std::string& hoursField)
{
    std::regex spaces_but_no_comma_outside_brackets(R"(\][^\[\]]*[^,\s][^\[\]]*\[)");
    auto matchBegin = std::sregex_iterator(hoursField.begin(),
                                           hoursField.end(),
                                           spaces_but_no_comma_outside_brackets);
    return matchBegin == std::sregex_iterator();
}

static bool oneCommaOutsideBrackets(const std::string& hoursField)
{
    std::regex there_are_commas_outside_brackets(R"(\](,|\s)+\[)");
    auto matchBegin = std::sregex_iterator(hoursField.begin(),
                                           hoursField.end(),
                                           there_are_commas_outside_brackets);
    auto matchEnd = std::sregex_iterator();
    for (auto it = matchBegin; it != matchEnd; ++it)
    {
        std::string match = (*it)[0].str();
        auto count = std::ranges::count(match, ',');
        if (count != 1)
        {
            return false;
        }
    }
    return true;
}

static void checkNothingFancyOutsideBrackets(const std::string& hoursField)
{
    // Outside square brackets, we want only spaces and 1 comma, for ex : : "...] , [..."
    if (!onlyCommasOrSpacesOutsideBrackets(hoursField))
    {
        throw std::invalid_argument("strange char outside square brackets");
    }
    if (!oneCommaOutsideBrackets(hoursField))
    {
        throw std::invalid_argument("multiple commas outside square brackets");
    }
}

static void checkNoNestedSquareBrackets(const std::string& hoursField)
{


    std::regex twoBracketsOpened(R"(\[[^\[\]]*\[)");
    auto matchBegin = std::sregex_iterator(hoursField.begin(),
                                           hoursField.end(),
                                           twoBracketsOpened);
    if (matchBegin != std::sregex_iterator())
    {
        throw std::invalid_argument("2 square brackets opened");
    }

    std::regex twoBracketsClosed(R"(\][^\[\]]*\])");
    matchBegin = std::sregex_iterator(hoursField.begin(),
                                      hoursField.end(),
                                      twoBracketsClosed);
    if (matchBegin != std::sregex_iterator())
    {
        throw std::invalid_argument("2 square brackets closed");
    }
}

static std::vector<std::string> splitIntoGroups(const std::string& hoursField)
{
    std::vector<std::string> my_groups;

    std::regex betweenSquareBrackets(R"(\[([^\[\]]*?)\])");
    auto matchBegin = std::sregex_iterator(hoursField.begin(),
                                           hoursField.end(),
                                           betweenSquareBrackets);
    auto matchEnd = std::sregex_iterator();
    for (auto it = matchBegin; it != matchEnd; ++it)
    {
        std::string match = (*it)[1].str();
        my_groups.push_back(match);
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

static std::vector<std::set<int>> toGroupsOfHours(
  const std::vector<std::vector<std::string>>& groups)
{
    std::vector<std::set<int>> groupsOfHours;
    for (const auto& group: groups)
    {
        std::set<int> groupOfInts;
        for (const std::string& h: group)
        {
            try
            {
                int hour = std::stoi(h);
                groupOfInts.insert(hour);
            }
            catch (std::out_of_range&)
            {
                throw std::out_of_range("hour " + h + " is out of range");
            }
        }
        groupsOfHours.push_back(groupOfInts);
    }
    return groupsOfHours;
}

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField)
{
    checkNothingFancyOutsideBrackets(hoursField);
    checkNoNestedSquareBrackets(hoursField);
    auto groups = splitIntoGroups(hoursField);
    if (groups.empty())
    {
        throw std::invalid_argument("there are no group of hours");
    }
    auto groupsOfHoursAsStrings = splitGroupsIntoHoursAsString(groups);
    return toGroupsOfHours(groupsOfHoursAsStrings);
}
} // namespace Antares::Data::ShortTermStorage
