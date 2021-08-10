/*
** Copyright 2007-2018 RTE
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
VariablePrintInfo::VariablePrintInfo(AnyString vname, uint maxNbCols, uint dataLvl, uint fileLvl) :
 varname(""),
 to_be_printed(true),
 maxNumberColumns(maxNbCols),
 dataLevel(dataLvl),
 fileLevel(fileLvl)
{
    varname = vname;
}

string VariablePrintInfo::name()
{
    return varname.to<string>();
}
void VariablePrintInfo::enablePrint(bool b)
{
    to_be_printed = b;
}
bool VariablePrintInfo::isPrinted()
{
    return to_be_printed;
}
uint VariablePrintInfo::getMaxColumnsCount()
{
    return maxNumberColumns;
}

// ============================================================
// Variables print information collector
// ============================================================
variablePrintInfoCollector::variablePrintInfoCollector(AllVariablesPrintInfo* allvarsprintinfo) :
 allvarsinfo(allvarsprintinfo)
{
}

void variablePrintInfoCollector::add(const AnyString& name,
                                     uint nbGlobalResults,
                                     uint dataLevel,
                                     uint fileLevel)
{
    allvarsinfo->add(new VariablePrintInfo(name, nbGlobalResults, dataLevel, fileLevel));
}

// ============================================================
// All variables print information
// ============================================================
AllVariablesPrintInfo::AllVariablesPrintInfo() :
 maxColumnsCount(0), numberSelectedAreaVariables(0), numberSelectedLinkVariables(0)
{
    // Re-initializing the iterator
    it_info = allVarsPrintInfo.begin();
}

AllVariablesPrintInfo::~AllVariablesPrintInfo()
{
    clear();
}

void AllVariablesPrintInfo::add(VariablePrintInfo* v)
{
    allVarsPrintInfo.push_back(v);
}

void AllVariablesPrintInfo::clear()
{
    // Destroying objects in lists
    // ---------------------------
    // Deleting variable' print info objects pointed in the list
    vector<VariablePrintInfo*>::iterator it = allVarsPrintInfo.begin();
    for (it = allVarsPrintInfo.begin(); it != allVarsPrintInfo.end(); ++it)
        delete *it;

    // After destroying objects in list, clearing lists
    // ------------------------------------------------
    // Clearing variables' print info list
    allVarsPrintInfo.clear();
}

VariablePrintInfo* AllVariablesPrintInfo::operator[](uint i) const
{
    return allVarsPrintInfo[i];
}

size_t AllVariablesPrintInfo::size() const
{
    return allVarsPrintInfo.size();
}

bool AllVariablesPrintInfo::isEmpty() const
{
    return size() == 0;
}

// Resetting iterator at the beginning of the list of all variables' print info
void AllVariablesPrintInfo::resetInfoIterator() const
{
    it_info = allVarsPrintInfo.begin();
}

bool AllVariablesPrintInfo::setPrintStatus(string varname, bool printStatus)
{
    /*
            From the position of the iterator on the print info collection, shifting right until
            reaching the print info associated to 'varname' argument. Then setting the good print
       info object with 'printStatus' argument. If searched variable print info not found, returning
       'false' meaing we have an error.
    */
    std::transform(varname.begin(), varname.end(), varname.begin(), ::toupper);

    for (; it_info != allVarsPrintInfo.end(); it_info++)
    {
        string current_var_name = (*it_info)->name();
        std::transform(
          current_var_name.begin(), current_var_name.end(), current_var_name.begin(), ::toupper);
        if (varname == current_var_name)
        {
            (*it_info)->enablePrint(printStatus);
            return true;
        }
    }
    return false;
}

void AllVariablesPrintInfo::prepareForSimulation(bool userSelection,
                                                 const std::vector<std::string>& excluded_vars)
{
    assert(!isEmpty() && "The variable print info list must not be empty at this point");

    // Initializing output variables status
    if (!userSelection)
        setAllPrintStatusesTo(true);

    for (const auto& varname : excluded_vars)
    {
        const bool res = setPrintStatus(varname, false);
        if (not res)
            logs.info() << "Variable " << varname << " not found. Could not remove it";
    }

    // Computing the max number columns a report of any kind can contain.
    computeMaxColumnsCountInReports();

    // Counting zonal and link output selected variables
    countSelectedAreaVars();
    countSelectedLinkVars();
}

bool AllVariablesPrintInfo::searchIncrementally_getPrintStatus(string var_name) const
{
    // Finds out if an output variable is selected for print or not.
    // The search for the variable in the print info list is incremental :
    // it resumes where it was left at the previous call.
    // This function is meant to be called over the whole list of variables,
    // not to find the print status of one isolated variable.
    // We want to avoid to search from the start of the print info list at each call.

    resetInfoIterator();
    for (; it_info != allVarsPrintInfo.end(); it_info++)
    {
        if ((*it_info)->name() == var_name)
        {
            return (*it_info)->isPrinted();
        }
    }

    // This is the case where we have an adequacy-draft variable :
    // in the case of other study modes, we never get here.
    resetInfoIterator();
    return true;
}

bool AllVariablesPrintInfo::isPrinted(string var_name) const
{
    // Finds out if an output variable selected for print or not.
    // The search for a variable starts from the beginning of the variable print info list.

    for (; it_info != allVarsPrintInfo.end(); it_info++)
    {
        if ((*it_info)->name() == var_name)
        {
            return (*it_info)->isPrinted();
        }
    }

    // This point is not supposed to be reached (except in draft mode),
    // because the searched variables should be found.
    return true;
}

void AllVariablesPrintInfo::setAllPrintStatusesTo(bool b)
{
    for (uint i = 0; i < size(); ++i)
        allVarsPrintInfo[i]->enablePrint(b);
}

void AllVariablesPrintInfo::computeMaxColumnsCountInReports()
{
    /*
            Among all reports a study can create, which is the one that contains the largest
            number of columns and especially what is this number ?
            If there are some unselected variables, the previous number is reduced.
            This number is a rough over-estimation, not the exact maximum number a report can
       contain.
    */

    uint CFileLevel = 1;
    uint CDataLevel = 1;

    // Looping over all kinds of data levels (area report, link reports, districts reports, thermal
    // reports,...) and file levels (values reports, years ids reports, details reports, ...) the
    // code can produce. For one particular kind of report, looping over (selected) output variables
    // it contains, and incrementing a counter with as many columns as the current variable can take
    // up at most in a report.
    while (CDataLevel <= Category::maxDataLevel && CFileLevel <= Category::maxFileLevel)
    {
        uint currentColumnsCount = 0;

        vector<VariablePrintInfo*>::iterator it = allVarsPrintInfo.begin();
        for (; it != allVarsPrintInfo.end(); it++)
        {
            if ((*it)->isPrinted() && (*it)->getFileLevel() & CFileLevel
                && (*it)->getDataLevel() & CDataLevel)
                currentColumnsCount += (*it)->getMaxColumnsCount();
        }

        if (currentColumnsCount > maxColumnsCount)
            maxColumnsCount = currentColumnsCount;

        CFileLevel = (CFileLevel * 2 > (int)Category::maxFileLevel) ? 1 : CFileLevel * 2;
        CDataLevel = (CFileLevel * 2 > (int)Category::maxFileLevel) ? CDataLevel * 2 : CDataLevel;
    }
}

void AllVariablesPrintInfo::countSelectedAreaVars()
{
    resetInfoIterator();
    for (; it_info != allVarsPrintInfo.end(); it_info++)
    {
        if ((*it_info)->isPrinted() && (*it_info)->getDataLevel() == Category::area)
            numberSelectedAreaVariables++;
    }
}

void AllVariablesPrintInfo::countSelectedLinkVars()
{
    resetInfoIterator();
    for (; it_info != allVarsPrintInfo.end(); it_info++)
    {
        if ((*it_info)->isPrinted() && (*it_info)->getDataLevel() == Category::link)
            numberSelectedLinkVariables++;
    }
}

} // namespace Data
} // namespace Antares