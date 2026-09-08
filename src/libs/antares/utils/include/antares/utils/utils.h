// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_UTILS_H__
#define __ANTARES_LIBS_UTILS_H__

#include <chrono>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares
{
/*!
** \brief Transform an arbitrary string into an ID
**
** All invalid caracters will be replaced by `_`.
*/
template<class StringT>
void TransformNameIntoID(const AnyString& name, StringT& out);
std::string transformNameIntoID(const std::string& name);

/*!
** \brief Convert a string into a boolean
**
** Replicates the legacy behavior of `Yuni::CString::to<bool>()`:
** a single character equal to `1`, `y`, `o` or `t` (any case) or a
** word equal to `true`, `on` or `yes` (any case) yields `true`.
*/
bool stringToBool(const std::string& s);

/*!
** \brief Convert a string into a numeric value
**
** Replicates the legacy behavior of `Yuni::CString::to<T>(out)`: an
** empty string converts to zero, and the whole string must be a valid
** number, otherwise `out` is left unchanged and the function returns `false`.
*/
bool stringToDouble(const std::string& s, double& out);
bool stringToFloat(const std::string& s, float& out);
bool stringToInt(const std::string& s, int& out);
bool stringToUint(const std::string& s, unsigned int& out);

/*!
** \brief Value-returning variants of the conversions above
**
** An empty string converts to zero, and a string that is not a valid
** number converts to zero as well.
*/
double stringToDouble(const std::string& s);
float stringToFloat(const std::string& s);
int stringToInt(const std::string& s);
unsigned int stringToUint(const std::string& s);

/*!
** \brief Return a lower-cased copy of a string
*/
std::string stringToLower(const std::string& s);

/*!
** \brief Return a copy of a string without leading/trailing whitespaces
**
** Whitespaces are space, tabulation, carriage return and line feed.
*/
std::string stringTrim(const std::string& s);

std::tm getCurrentTime();
std::string formatTime(const std::tm& localTime, const std::string& format);

std::vector<std::pair<std::string, std::string>> splitStringIntoPairs(const std::string& s,
                                                                      char delimiter1,
                                                                      char delimiter2);

namespace Utils
{

bool compareCaseInsensitive(const std::string& str1, const std::string& str2);

bool isZero(double d);
double round(double d, unsigned precision);
double ceil(double d);
double floor(double d);

bool isPathValid(const std::string& path);

std::map<std::string, unsigned> giveNumbersToStrings(const std::vector<std::string>& strs);
std::map<std::string, unsigned> giveNumbersToStrings(const std::set<std::string>& strs);
bool checkAllElementsIdenticalOrOne(std::vector<unsigned> w);
bool checkAllElementsIdenticalOrOne(std::vector<std::pair<unsigned, std::string>>& p);

bool generatePathWithSuffix(std::filesystem::path& outputPath, const std::string& suffix = "");

class TimeMeasurement final
{
    using clock = std::chrono::steady_clock;

public:
    TimeMeasurement();
    void tick();
    long duration_ms() const;
    std::string toString() const;
    std::string toStringInSeconds() const;
    void reset();

private:
    clock::time_point start_;
    clock::time_point end_;
};

} // namespace Utils
} // namespace Antares

#include "utils.hxx"

#endif // __ANTARES_LIBS_UTILS_H__
