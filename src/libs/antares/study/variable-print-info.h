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

#ifndef __SOLVER_VARIABLE_PRINT_POLICY_H__
#define __SOLVER_VARIABLE_PRINT_POLICY_H__

#include <string>
#include <vector>
#include <map>
#include <yuni/yuni.h>
#include <yuni/core/fwd.h>
#include <yuni/core/string.h>

namespace Antares
{
namespace Data
{
// Represents an output variable (wears the same name) and mainly answers the question :
// Is the real variable printed in all output reports ? Or is it not printed in any report ?
class VariablePrintInfo
{
public:
    VariablePrintInfo(AnyString vname, uint dataLvl, uint fileLvl);
    ~VariablePrintInfo(){};

    // Getting name of the (represented) output variable
    std::string name();

    // Do we enable or disable variable's print in output reports ?
    void enablePrint(bool b);
    bool isPrinted();

    uint getMaxColumnsCount();
    void setMaxColumns(uint maxColumnsNumber);
    uint getDataLevel()
    {
        return dataLevel;
    }
    uint getFileLevel()
    {
        return fileLevel;
    }

private:
    // Current variable's name
    AnyString varname;
    // Is the variable printed ?
    bool to_be_printed;

    // The number of columns the output variable takes in a SYNTHESIS report.
    // Recall that synthesis reports always contain more columns than
    // any other reports (for instance year-by-year reports)
    uint maxNumberColumns_ = 0;

    // Which reports the output variable has columns in ?
    // Example : areas/values-<time-interval>.txt
    // dataLevel can be : areas, links, bindingConstraint
    // fileLevel can be : values-<time-interval>.txt, details-<time-interval>.txt, id-<time-interval>.txt, ...
    uint dataLevel;
    uint fileLevel;
};

class AllVariablesPrintInfo;

class variablePrintInfoCollector
{
public:
    variablePrintInfoCollector(AllVariablesPrintInfo* allvarsprintinfo);
    void add(const AnyString& name, uint dataLevel, uint fileLevel);

private:
    AllVariablesPrintInfo* allvarsinfo;
};

// Variables print info collection. Mainly a vector of pointers to print info.
// This collection is filled with as many print info as we can find output variables in the output
// variables Antares's static list.
class AllVariablesPrintInfo
{
public:
    // Public methods
    AllVariablesPrintInfo() = default;
    ~AllVariablesPrintInfo();

    void add(VariablePrintInfo* v);
    void clear();
    VariablePrintInfo* operator[](uint i) const;
    size_t size() const;
    bool isEmpty() const;

    bool setPrintStatus(std::string varname, bool printStatus);
    void setMaxColumns(std::string varname, uint maxColumnsNumber);

    void prepareForSimulation(bool isThematicTrimmingEnabled,
                              const std::vector<std::string>& excluded_vars = {});

    // Classic search, then get the print status
    bool isPrinted(std::string var_name) const;

    uint getTotalMaxColumnsCount() const
    {
        return totalMaxColumnsCount_;
    }

    uint getNbSelectedZonalVars() const
    {
        return numberSelectedAreaVariables;
    }
    uint getNbSelectedLinkVars() const
    {
        return numberSelectedLinkVariables;
    }

    void computeMaxColumnsCountInReports();
    void setAllPrintStatusesTo(bool b);

private:
    void countSelectedAreaVars();
    void countSelectedLinkVars();

private:
    // Contains print info for all variables
    std::vector<VariablePrintInfo*> allVarsPrintInfo;
    // std::map<std::string, VariablePrintInfo> allVarsPrintInfo;

    // Max columns count a report of any kind can contain, depending on the number of selected
    // variables. The less variables are selected, the smallest this count is.
    uint totalMaxColumnsCount_ = 0;

    // Number of selected zonal variables
    uint numberSelectedAreaVariables = 0;
    // Number of selected link variables
    uint numberSelectedLinkVariables = 0;
};

} // namespace Data
} // namespace Antares

#endif // __SOLVER_VARIABLE_PRINT_POLICY_H__
