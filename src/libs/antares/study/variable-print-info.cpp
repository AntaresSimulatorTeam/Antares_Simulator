/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "variable-print-info.h"
#include <algorithm>
#include <assert.h>
#include <antares/study.h>
#include "../solver/variable/categories.h"

using namespace Antares::Solver::Variable;

namespace Antares
{
namespace Data
{
// ============================================================
// One variable print information
// ============================================================
VariablePrintInfo::VariablePrintInfo(uint dataLvl, uint fileLvl) :
    to_be_printed_(true),
    dataLevel_(dataLvl),
    fileLevel_(fileLvl)
{
}

void VariablePrintInfo::enablePrint(bool b)
{
    to_be_printed_ = b;
}
bool VariablePrintInfo::isPrinted() const
{
    return to_be_printed_;
}

void VariablePrintInfo::reverse()
{
    to_be_printed_ = !to_be_printed_;
}
uint VariablePrintInfo::getMaxColumnsCount()
{
    return maxNumberColumns_;
}

void VariablePrintInfo::setMaxColumns(uint maxColumnsNumber)
{
    maxNumberColumns_ = std::max(maxColumnsNumber, maxNumberColumns_);
}

// ============================================================
// Variables print information collector
// ============================================================
variablePrintInfoCollector::variablePrintInfoCollector(AllVariablesPrintInfo* allvarsprintinfo) :
 allvarsinfo(allvarsprintinfo)
{
}

void variablePrintInfoCollector::add(const AnyString& name,
                                     uint dataLevel,
                                     uint fileLevel)
{
    allvarsinfo->add(name.to<std::string>(), VariablePrintInfo(dataLevel, fileLevel));
}

// ============================================================
// All variables print information
// ============================================================

static std::string to_uppercase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

void AllVariablesPrintInfo::add(std::string name, VariablePrintInfo v)
{
    std::string upperCaseName = to_uppercase(name);
    if (not exists(upperCaseName))
    {
        index_to_name[(unsigned int)allVarsPrintInfo.size()] = upperCaseName;
        allVarsPrintInfo.insert(std::pair<std::string, VariablePrintInfo>(upperCaseName, v));
    }
}

void AllVariablesPrintInfo::clear()
{
    allVarsPrintInfo.clear();
    index_to_name.clear();
}

VariablePrintInfo& AllVariablesPrintInfo::operator[](uint i)
{
    std::string name = index_to_name[i];
    return allVarsPrintInfo.at(name);
}

size_t AllVariablesPrintInfo::size() const
{
    return allVarsPrintInfo.size();
}

bool AllVariablesPrintInfo::exists(std::string name)
{
    return allVarsPrintInfo.find(to_uppercase(name)) != allVarsPrintInfo.end();
}

void AllVariablesPrintInfo::setPrintStatus(std::string varname, bool printStatus)
{
    allVarsPrintInfo.at(to_uppercase(varname)).enablePrint(printStatus);
}

void AllVariablesPrintInfo::setPrintStatus(unsigned int index, bool printStatus)
{
    std::string name = index_to_name[index];
    setPrintStatus(name, printStatus);
}

void AllVariablesPrintInfo::setMaxColumns(std::string varname, uint maxColumnsNumber)
{
    allVarsPrintInfo.at(to_uppercase(varname)).setMaxColumns(maxColumnsNumber);
}

std::string AllVariablesPrintInfo::name_of(unsigned int index) const
{
    return index_to_name.at(index);
}

void AllVariablesPrintInfo::prepareForSimulation(bool isThematicTrimmingEnabled,
                                                 const std::vector<std::string>& excluded_vars)
{
    // Initializing output variables status
    if (!isThematicTrimmingEnabled)
        setAllPrintStatusesTo(true);

    for (const auto& varname : excluded_vars)
    {
        setPrintStatus(varname, false); // varname is supposed to in uppercase already
    }

    // Counting zonal and link output selected variables
    countSelectedAreaVars();
    countSelectedLinkVars();
}

bool AllVariablesPrintInfo::isPrinted(std::string var_name) const
{
    return allVarsPrintInfo.at(to_uppercase(var_name)).isPrinted();
}

void AllVariablesPrintInfo::setAllPrintStatusesTo(bool b)
{
    for (auto& [name, variable] : allVarsPrintInfo)
    {
        variable.enablePrint(b);
    }

}

void AllVariablesPrintInfo::reverseAll()
{
    for (auto& [name, variable] : allVarsPrintInfo)
    {
        variable.reverse();
    }

}

unsigned int AllVariablesPrintInfo::numberOfEnabledVariables()
{
    return std::count_if(allVarsPrintInfo.begin(), 
                         allVarsPrintInfo.end(), 
                         [](auto& p) {return p.second.isPrinted(); });
}

std::vector<std::string> AllVariablesPrintInfo::namesOfVariablesWithPrintStatus(bool printStatus)
{
    std::vector<std::string> vector_to_return;
    for (auto& [name, variable] : allVarsPrintInfo)
    {
        if (variable.isPrinted() == printStatus)
            vector_to_return.push_back(name);
    }
    return vector_to_return;
}

std::vector<std::string> AllVariablesPrintInfo::namesOfEnabledVariables()
{
    return namesOfVariablesWithPrintStatus(true);
}

std::vector<std::string> AllVariablesPrintInfo::namesOfDisabledVariables()
{
    return namesOfVariablesWithPrintStatus(false);
}

void AllVariablesPrintInfo::computeMaxColumnsCountInReports()
{
    /*
        Among all reports a study can create, which is the one that contains the largest
        number of columns and especially what is this number ?
        If there are some unselected variables, the previous number is reduced.
        Note that synthesis reports always contain more columns than year by year reports.
        So the computed max number of columns is actually the max number of columns in a synthesis report.
    */

    // Looping over all kinds of data levels (area report, link reports, districts reports, thermal
    // reports,...) and for a given data level, looping over file levels 
    // (values reports, years ids reports, details reports, ...) the
    // code can produce. 
    // For one particular kind of report, looping over (selected) output variables
    // it contains, and incrementing a counter with as many columns as the current variable can take
    // up at most in a report.

    for (uint CDataLevel = 1; CDataLevel <= Category::maxDataLevel; CDataLevel *= 2)
    {
        for (uint CFileLevel = 1; CFileLevel <= Category::maxFileLevel; CFileLevel *= 2)
        {
            uint currentColumnsCount = 0;
            for (auto& [name, variable] : allVarsPrintInfo)
            {
                if (variable.isPrinted() &&
                    variable.getFileLevel() & CFileLevel &&
                    variable.getDataLevel() & CDataLevel)
                {
                    // For the current output variable, we retrieve the max number
                    // of columns it takes in a sysnthesis report. 
                    currentColumnsCount += variable.getMaxColumnsCount();
                }
            }

            totalMaxColumnsCount_ = std::max(totalMaxColumnsCount_, currentColumnsCount);
        }
    }
}

void AllVariablesPrintInfo::countSelectedAreaVars()
{
    for (auto& [name, variable] : allVarsPrintInfo)
    {
        if (variable.isPrinted() && variable.getDataLevel() == Category::area)
            numberSelectedAreaVariables++;
    }
}

void AllVariablesPrintInfo::countSelectedLinkVars()
{
    for (auto& [name, variable] : allVarsPrintInfo)
    {
        if (variable.isPrinted() && variable.getDataLevel() == Category::link)
            numberSelectedLinkVariables++;
    }
}


} // namespace Data
} // namespace Antares
