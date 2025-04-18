
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"

#include <regex>

#include <boost/algorithm/string.hpp>

namespace Antares::Data::ShortTermStorage
{

static void throwExceptionWithMsg(const bool doWeThrow, const std::string& msg)
{
    if (doWeThrow)
    {
        throw std::invalid_argument(msg);
    }
}

static bool onlyCommasOutsideBrackets(const std::string& hoursField)
{
    const std::regex strange_char_outside_brackets(R"(\][^\[\]]*[^,][^\[\]]*\[)");
    return !std::regex_search(hoursField, strange_char_outside_brackets);
}

static bool oneCommaOutsideBrackets(const std::string& hoursField)
{
    const std::regex two_or_more_commas_outside_brackets(R"(\][,]{2,}\[)");
    return !std::regex_search(hoursField, two_or_more_commas_outside_brackets);
}

static bool twoBracketsOpened(const std::string& hoursField)
{
    const std::regex twoBracketsOpened(R"(\[[^\[\]]*\[)");
    return !std::regex_search(hoursField, twoBracketsOpened);
}

static bool twoBracketsClosed(const std::string& hoursField)
{
    const std::regex twoBracketsClosed(R"(\][^\[\]]*\])");
    return !std::regex_search(hoursField, twoBracketsClosed);
}

static void checkNothingFancyOutsideBrackets(const std::string& hoursField)
{
    // Outside square brackets, we want only 1 comma, for ex : : "...] , [..."
    throwExceptionWithMsg(!onlyCommasOutsideBrackets(hoursField), "strange char outside brackets");
    throwExceptionWithMsg(!oneCommaOutsideBrackets(hoursField), "multiple commas outside brackets");
}

static void checkNoNestedSquareBrackets(const std::string& hoursField)
{
    throwExceptionWithMsg(!twoBracketsOpened(hoursField), "2 square brackets opened");
    throwExceptionWithMsg(!twoBracketsClosed(hoursField), "2 square brackets closed");
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

    std::regex commaSeparatedDigits(R"(^\d+(,\d+)*$)");
    for (const auto& group: groups)
    {
        if (!std::regex_match(group, commaSeparatedDigits))
        {
            throw std::invalid_argument("splitting each group into hours > wrong format");
        }
        std::vector<std::string> hoursAsStrings;
        boost::split(hoursAsStrings, group, boost::is_any_of(","));
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

std::vector<std::set<int>> makeGroupsOfHours(std::string& hoursField)
{
    std::erase_if(hoursField, ::isspace); // Removing all spaces from hour field
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
