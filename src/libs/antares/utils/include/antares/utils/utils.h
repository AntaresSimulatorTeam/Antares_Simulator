// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_UTILS_H__
#define __ANTARES_LIBS_UTILS_H__

#include <chrono>
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

std::tm getCurrentTime();
std::string formatTime(const std::tm& localTime, const std::string& format);

/*!
** \brief Beautify a name, for renaming an area for example
*/
void BeautifyName(YString& out, AnyString oldname);
void BeautifyName(std::string& out, const std::string& oldname);

std::vector<std::pair<std::string, std::string>> splitStringIntoPairs(const std::string& s,
                                                                      char delimiter1,
                                                                      char delimiter2);

namespace Utils
{

bool isZero(double d);
double round(double d, unsigned precision);
double ceil(double d);
double floor(double d);

bool isPathValid(const std::string& path);

std::map<std::string, unsigned> giveNumbersToStrings(const std::vector<std::string>& strs);
std::map<std::string, unsigned> giveNumbersToStrings(const std::set<std::string>& strs);
bool checkAllElementsIdenticalOrOne(std::vector<unsigned> w);
bool checkAllElementsIdenticalOrOne(std::vector<std::pair<unsigned, std::string>>& p);

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
